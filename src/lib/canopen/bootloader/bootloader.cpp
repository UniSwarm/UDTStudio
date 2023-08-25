/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

#include "bootloader.h"

#include "indexdb.h"
#include "model/ufwmodel.h"
#include "node.h"
#include "parser/ufwparser.h"
#include "utility/ufwupdate.h"

#include <QDebug>
#include <QFile>
#include <QtEndian>

enum
{
    TIMER_READ_STATUS_DISPLAY = 500
};

enum PROGRAM_CONTROL
{
    PROGRAM_CONTROL_STOP = 0x00,
    PROGRAM_CONTROL_START = 0x01,
    PROGRAM_CONTROL_RESET = 0x02,
    PROGRAM_CONTROL_CLEAR = 0x03,
    PROGRAM_CONTROL_UPDATE_START = 0x80,
    PROGRAM_CONTROL_UPDATE_END = 0x81,
    PROGRAM_CONTROL_CLEAR_IN_PROGRESS = 0x82,
    PROGRAM_CONTROL_MANU_DATA_START = 0x83
};

enum BOOTLOADER_STATUS
{
    BOOTLOADER_STATUS_OBJECT_OK = 0x01,
    BOOTLOADER_STATUS_OBJECT_NOK = 0x02,
    BOOTLOADER_STATUS_OBJECT_CLEAR_NOK = 0x04,
    BOOTLOADER_STATUS_OBJECT_CK_NOK = 0x08,
    BOOTLOADER_STATUS_OBJECT_STOP_NOK = 0x10,
    BOOTLOADER_STATUS_OBJECT_OUT_OF_MEMORY_NOK = 0x20,
    BOOTLOADER_STATUS_OBJECT_SIZE_NOK = 0x40,
    BOOTLOADER_STATUS_OBJECT_KEY_NOK = 0x80
};

Bootloader::Bootloader(Node *node)
    : _node(node)
{
    _node = node;
    _ufwModel = nullptr;
    _ufwUpdate = new UfwUpdate(_node);
    connect(_ufwUpdate, &UfwUpdate::finished, this, &Bootloader::processEndUpload);

    _programDataObjectId = IndexDb::getObjectId(IndexDb::OD_PROGRAM_DATA_1);
    _programControlObjectId = IndexDb::getObjectId(IndexDb::OD_PROGRAM_CONTROL_1);
    _bootloaderKeyObjectId = IndexDb::getObjectId(IndexDb::OD_BOOTLOADER_KEY);
    _bootloaderChecksumObjectId = IndexDb::getObjectId(IndexDb::OD_BOOTLOADER_CHECKSUM);
    _bootloaderStatusObjectId = IndexDb::getObjectId(IndexDb::OD_BOOTLOADER_STATUS);

    setNodeInterrest(_node);
    registerObjId({0x1000, 0});
    registerObjId(_programDataObjectId);
    registerObjId(_programControlObjectId);
    registerObjId(_bootloaderKeyObjectId);
    registerObjId(_bootloaderStatusObjectId);

    _error = 0;
    _deviceOtp = 0;
    _status = STATUS_ERROR_NO_FILE;
    _state = STATE_FREE;
    _statusProgram = NONE;
    _mode = MODE_NONE;
}

Bootloader::~Bootloader()
{
    delete _ufwUpdate;
    delete _ufwModel;
    unRegisterFullOd();
}

Node *Bootloader::node() const
{
    return _node;
}

Bootloader::Status Bootloader::status() const
{
    return _status;
}

void Bootloader::setStatus(Status status)
{
    _status = status;
    emit statusEvent();
}

QString Bootloader::statusStr(Status status) const
{
    switch (status)
    {
        case Bootloader::STATUS_ERROR_OPEN_FILE:
            return QString(tr("Error open file"));
        case Bootloader::STATUS_ERROR_NO_FILE:
            return QString(tr("No file"));
        case Bootloader::STATUS_ERROR_ERROR_PARSER:
            return QString(tr("Error parser"));
        case Bootloader::STATUS_ERROR_FILE_NOT_CORRESPONDING:
            return QString(tr("File not corresponding"));
        case Bootloader::STATUS_ERROR_UPDATE_FAILED:
            return QString(tr("Update failed"));
        case Bootloader::STATUS_FILE_ANALYZED_OK:
            return QString(tr("File analyzed ok"));
        case Bootloader::STATUS_UPDATE_ALREADY_IN_PROGRESS:
            return QString(tr("Update already in progress"));
        case Bootloader::STATUS_MEMORY_ALREADY_WRITTEN:
            return QString(tr("Error : Memory already written"));
        case Bootloader::STATUS_CHECK_FILE_AND_DEVICE:
            return QString(tr("Checking the settings"));
        case Bootloader::STATUS_DEVICE_STOP_IN_PROGRESS:
            return QString(tr("Device stop in progress"));
        case Bootloader::STATUS_DEVICE_CLEAR_IN_PROGRESS:
            return QString(tr("Device clear in progress"));
        case Bootloader::STATUS_DEVICE_WRITING_OTP_IN_PROGRESS:
            return QString(tr("Device writing otp in progress"));
        case Bootloader::STATUS_DEVICE_UPDATE_IN_PROGRESS:
            return QString(tr("Device update in progress"));
        case Bootloader::STATUS_CHECKING_UPDATE:
            return QString(tr("Checking the update"));
        case Bootloader::STATUS_UPDATE_SUCCESSFUL:
            return QString(tr("Update successful"));
    }
    return QString();
}

quint32 Bootloader::error() const
{
    return _error;
}

double Bootloader::updateProgress() const
{
    if (_ufwUpdate != nullptr)
    {
        return _ufwUpdate->progress();
    }
    return 0;
}

bool Bootloader::openUfw(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        setStatus(STATUS_ERROR_OPEN_FILE);
        return false;
    }

    _ufwModel = UfwParser::parse(fileName);
    file.close();

    if (_ufwModel == nullptr)
    {
        setStatus(STATUS_ERROR_ERROR_PARSER);
        return false;
    }

    _ufwUpdate->setUfw(_ufwModel);
    setStatus(STATUS_FILE_ANALYZED_OK);
    _state = STATE_FREE;

    return true;
}

void Bootloader::setOtpInformation(uint32_t address, const QString &date, uint16_t device, uint32_t serialNumber, const QString &version)
{
    char buffer[4];
    _progOtp.clear();

    qToLittleEndian(address, buffer);
    _progOtp.append(buffer, sizeof(address));

    _progOtp.append(capString(date, 19));

    _deviceOtp = device;
    qToLittleEndian(_deviceOtp, buffer);
    _progOtp.append(buffer, sizeof(_deviceOtp));

    qToLittleEndian(serialNumber, buffer);
    _progOtp.append(buffer, sizeof(serialNumber));

    _progOtp.append(capString(version, 10));
}

void Bootloader::startOtpUpload()
{
    if (_progOtp.isEmpty())
    {
        setStatus(STATUS_ERROR_NO_FILE);
        return;
    }

    _node->nodeOd()->createBootloaderObjects();

    setStatus(STATUS_CHECK_FILE_AND_DEVICE);
    _mode = MODE_OTP;
    _state = STATE_CHECK_MODE;
    readStatusProgram();
}

void Bootloader::startUpdate()
{
    if (_ufwModel == nullptr)
    {
        setStatus(STATUS_ERROR_NO_FILE);
        return;
    }

    if (_state != STATE_FREE)
    {
        setStatus(STATUS_UPDATE_ALREADY_IN_PROGRESS);
        return;
    }

    setStatus(STATUS_CHECK_FILE_AND_DEVICE);
    _state = STATE_CHECK_MODE;
    _node->sendPreop();
    _node->nodeOd()->createBootloaderObjects();
    readStatusProgram();
}

uint32_t Bootloader::deviceType()
{
    if (_ufwModel == nullptr)
    {
        return 0;
    }
    return _ufwModel->deviceType();
}

QString Bootloader::versionSoftware()
{
    if (_ufwModel == nullptr)
    {
        return QString();
    }
    return _ufwModel->softwareVersion();
}

QString Bootloader::buildDate()
{
    if (_ufwModel == nullptr)
    {
        return QString();
    }
    return _ufwModel->buildDate();
}

void Bootloader::stopProgram()
{
    uint8_t data = PROGRAM_CONTROL_STOP;
    _node->writeObject(_programControlObjectId, data);
    QTimer::singleShot(TIMER_READ_STATUS_DISPLAY,
                       this,
                       [=]()
                       {
                           readStatusProgram();
                       });
}

void Bootloader::startProgram()
{
    uint8_t data = PROGRAM_CONTROL_START;
    _node->writeObject(_programControlObjectId, data);
}

void Bootloader::resetProgram()
{
    uint8_t data = PROGRAM_CONTROL_RESET;
    _node->writeObject(_programControlObjectId, data);
}

void Bootloader::clearProgram()
{
    uint8_t data = PROGRAM_CONTROL_CLEAR;
    _node->writeObject(_programControlObjectId, data);
    QTimer::singleShot(TIMER_READ_STATUS_DISPLAY,
                       this,
                       [=]()
                       {
                           readStatusProgram();
                       });
}

void Bootloader::updateProgram()
{
    _ufwUpdate->update();
}

void Bootloader::updateStartProgram()
{
    uint8_t data = PROGRAM_CONTROL_UPDATE_START;
    _node->writeObject(_programControlObjectId, data);
}

void Bootloader::updateFinishedProgram()
{
    uint8_t data = PROGRAM_CONTROL_UPDATE_END;
    _node->writeObject(_programControlObjectId, data);
}

void Bootloader::sendKey()
{
    if (_ufwModel != nullptr)
    {
        _node->writeObject(_bootloaderKeyObjectId, _ufwModel->deviceType());
    }

    if (_mode == MODE_OTP && _progOtp != nullptr)
    {
        _node->writeObject(_bootloaderKeyObjectId, _deviceOtp);
    }
}

void Bootloader::sendOtpUploadStart()
{
    uint8_t data = PROGRAM_CONTROL_MANU_DATA_START;
    _node->writeObject(_programControlObjectId, data);
}

void Bootloader::uploadOtpData()
{
    _node->writeObject(_programDataObjectId, _progOtp);
}

QByteArray Bootloader::capString(const QString &str, int size)
{
    QByteArray bytes;
    bytes.append(str.toUtf8());
    return bytes.leftJustified(size, '\0', true);
}

void Bootloader::readStatusProgram()
{
    _node->readObject(_programControlObjectId);
}

void Bootloader::readStatusBootloader()
{
    _node->readObject(_bootloaderStatusObjectId);
}

void Bootloader::processEndUpload(bool ok)
{
    if (ok)
    {
        _state = STATE_UPLOADED_PROGRAM_FINISHED;
    }
    else
    {
        _state = STATE_NOT_OK;
    }
    process();
}

void Bootloader::process()
{
    switch (_state)
    {
        case STATE_FREE:
            _statusProgram = StatusProgram::NONE;
            break;

        case STATE_CHECK_MODE:
        {
            switch (_statusProgram)
            {
                case StatusProgram::NONE:
                    readStatusProgram();
                    break;

                case StatusProgram::PROGRAM_STOPPED:
                    if (_mode == MODE_OTP)
                    {
                        setStatus(STATUS_DEVICE_WRITING_OTP_IN_PROGRESS);
                        sendKey();
                    }
                    else
                    {
                        setStatus(STATUS_DEVICE_CLEAR_IN_PROGRESS);
                        sendKey();
                        clearProgram();
                    }
                    break;

                case StatusProgram::PROGRAM_STARTED:
                    setStatus(STATUS_DEVICE_STOP_IN_PROGRESS);
                    sendKey();
                    stopProgram();
                    break;

                case StatusProgram::NO_PROGRAM:
                    break;
            }
            break;
        }

        case STATE_STOP_PROGRAM:
            if (_mode == MODE_OTP)
            {
                setStatus(STATUS_DEVICE_WRITING_OTP_IN_PROGRESS);
            }
            else
            {
                setStatus(STATUS_DEVICE_CLEAR_IN_PROGRESS);
                clearProgram();
            }
            sendKey();
            break;

        case STATE_CLEAR_PROGRAM:
            setStatus(STATUS_DEVICE_CLEAR_IN_PROGRESS);
            sendKey();
            clearProgram();
            break;

        case STATE_UPDATE_PROGRAM:
            updateStartProgram();
            setStatus(STATUS_DEVICE_UPDATE_IN_PROGRESS);
            updateProgram();
            break;

        case STATE_UPLOADED_PROGRAM_FINISHED:
            setStatus(STATUS_CHECKING_UPDATE);
            _node->writeObject(_bootloaderChecksumObjectId, _ufwUpdate->checksum());
            updateFinishedProgram();
            QTimer::singleShot(TIMER_READ_STATUS_DISPLAY,
                               this,
                               [=]()
                               {
                                   readStatusBootloader();
                               });
            _state = STATE_CHECK;
            break;

        case STATE_UPLOAD_OTP_START:
            sendKey();
            sendOtpUploadStart();
            setStatus(STATUS_DEVICE_UPDATE_IN_PROGRESS);
            break;

        case STATE_UPLOAD_OTP_IN_PROGRESS:
            setStatus(STATUS_DEVICE_UPDATE_IN_PROGRESS);
            uploadOtpData();
            break;

        case STATE_UPLOAD_OTP_FINISHED:
            setStatus(STATUS_CHECKING_UPDATE);
            updateFinishedProgram();
            QTimer::singleShot(TIMER_READ_STATUS_DISPLAY,
                               this,
                               [=]()
                               {
                                   qDebug() << "readStatusBootloader" << __LINE__;
                                   readStatusBootloader();
                               });
            _state = STATE_CHECK;
            _mode = MODE_NONE;
            break;

        case STATE_CHECK:
            break;

        case STATE_OK:
            resetProgram();
            _state = STATE_FREE;
            _mode = MODE_NONE;
            setStatus(STATUS_UPDATE_SUCCESSFUL);
            break;

        case STATE_NOT_OK:
            if (_mode == MODE_OTP)
            {
                resetProgram();
                _state = STATE_FREE;
                if (_error == SDO::CO_SDO_ABORT_CODE_FAILED_HARDWARE_ERR)
                {
                    setStatus(STATUS_MEMORY_ALREADY_WRITTEN);
                }
                else
                {
                    setStatus(STATUS_ERROR_UPDATE_FAILED);
                }
            }
            else
            {
                _state = STATE_FREE;
                _mode = MODE_NONE;
                setStatus(STATUS_ERROR_UPDATE_FAILED);
            }
            _mode = MODE_NONE;
            break;
    }
}

void Bootloader::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if (objId.index() == 0x1000)
    {
        uint32_t value = static_cast<uint32_t>(_node->nodeOd()->value(0x1000, 0).toUInt());
        if (value == 301)
        {
            _node->nodeOd()->createBootloaderObjects();
        }
    }

    if (_state == STATE_FREE)
    {
        return;
    }

    if (objId.index() == _programControlObjectId.index() && objId.subIndex() == 1)
    {
        if (flags == NodeOd::FlagsRequest::Read)
        {
            uint8_t program = static_cast<uint8_t>(_node->nodeOd()->value(_programControlObjectId).toUInt());
            if (program == PROGRAM_CONTROL_STOP || program == PROGRAM_CONTROL_RESET)
            {
                _statusProgram = PROGRAM_STOPPED;
                if (_mode == MODE_OTP)
                {
                    _state = STATE_UPLOAD_OTP_START;
                }
                else
                {
                    _state = STATE_CLEAR_PROGRAM;
                }
                process();
            }
            else if (program == PROGRAM_CONTROL_CLEAR)
            {
                _statusProgram = NO_PROGRAM;
                if (_mode == MODE_OTP)
                {
                    _state = STATE_NOT_OK;
                }
                else
                {
                    _state = STATE_UPDATE_PROGRAM;
                }
                process();
            }
            else if (program == PROGRAM_CONTROL_CLEAR_IN_PROGRESS)
            {
                if (_mode == MODE_OTP)
                {
                    _statusProgram = NO_PROGRAM;
                    _state = STATE_NOT_OK;
                }
                else
                {
                    QTimer::singleShot(TIMER_READ_STATUS_DISPLAY,
                                       this,
                                       [=]()
                                       {
                                           readStatusProgram();
                                       });
                }
            }
            else
            {
                _statusProgram = PROGRAM_STARTED;
                process();
            }
        }
        else if (flags == NodeOd::FlagsRequest::Write && _mode == MODE_OTP && _state == STATE_UPLOAD_OTP_START)
        {
            _state = STATE_UPLOAD_OTP_IN_PROGRESS;
            process();
        }
        if ((flags & NodeOd::FlagsRequest::Error) == NodeOd::FlagsRequest::Error)
        {
            _error = static_cast<quint32>(_node->nodeOd()->errorObject(_programControlObjectId));
            _state = STATE_NOT_OK;
            process();
        }
    }

    if (objId.index() == _bootloaderStatusObjectId.index() && objId.subIndex() == _bootloaderStatusObjectId.subIndex())
    {
        uint8_t status = static_cast<uint8_t>(_node->nodeOd()->value(_bootloaderStatusObjectId).toUInt());
        if (_state == STATE_CHECK)
        {
            if ((status & BOOTLOADER_STATUS_OBJECT_OK) == BOOTLOADER_STATUS_OBJECT_OK)
            {
                _state = STATE_OK;
            }
            else
            {
                _state = STATE_NOT_OK;
            }
            process();
        }
    }

    if (objId.index() == _programDataObjectId.index())
    {
        if (_mode == MODE_OTP)
        {
            if ((flags & NodeOd::FlagsRequest::Error) == NodeOd::FlagsRequest::Error)
            {
                _progOtp.clear();
                _state = STATE_NOT_OK;
            }
            else if (flags == NodeOd::FlagsRequest::Write)
            {
                _progOtp.clear();
                _state = STATE_UPLOAD_OTP_FINISHED;
            }
            process();
        }
    }
}
