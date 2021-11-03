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

#define TIMER_READ_STATUS_DISPLAY 500
#define ATTEMPT_MAX_ERROR         3

#define PROGRAM_CONTROL_STOP            0x0
#define PROGRAM_CONTROL_START           0x1
#define PROGRAM_CONTROL_RESET           0x2
#define PROGRAM_CONTROL_CLEAR           0x3
#define PROGRAM_CONTROL_UPDATE_START    0x80
#define PROGRAM_CONTROL_UPDATE_FINISHED 0x81

#define BOOTLOADER_STATUS_OBJECT_OK        0x1
#define BOOTLOADER_STATUS_OBJECT_NOK       0x2
#define BOOTLOADER_STATUS_OBJECT_CLEAR_NOK 0x4
#define BOOTLOADER_STATUS_OBJECT_CK_NOK    0x8

Bootloader::Bootloader(Node *node)
    : _node(node)
{
    sendObject = false;
    _node = node;
    _ufwModel = nullptr;
    _ufwUpdate = new UfwUpdate(_node);
    connect(_ufwUpdate, &UfwUpdate::finished, this, &Bootloader::processEndUpload);

    _bootloaderKeyObjectId = IndexDb::getObjectId(IndexDb::OD_BOOTLOADER_KEY);
    _bootloaderChecksumObjectId = IndexDb::getObjectId(IndexDb::OD_BOOTLOADER_CHECKSUM);
    _bootloaderStatusObjectId = IndexDb::getObjectId(IndexDb::OD_BOOTLOADER_STATUS);
    _programControlObjectId = IndexDb::getObjectId(IndexDb::OD_PROGRAM_CONTROL_1);

    setNodeInterrest(_node);
    registerObjId({0x1000, 0});
    registerObjId({0x2050, 1});
    registerObjId({0x2050, 2});
    registerObjId({0x2050, 3});
    registerObjId({0x1F51, 1});

    _statusProgram = NONE;
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
        case Bootloader::STATUS_CHECK_FILE_AND_DEVICE:
            return QString(tr("Checking the settings"));
        case Bootloader::STATUS_DEVICE_STOP_IN_PROGRESS:
            return QString(tr("Device stop in progress"));
        case Bootloader::STATUS_DEVICE_CLEAR_IN_PROGRESS:
            return QString(tr("Device clear in progress"));
        case Bootloader::STATUS_DEVICE_UPDATE_IN_PROGRESS:
            return QString(tr("Device update in progress"));
        case Bootloader::STATUS_CHECKING_UPDATE:
            return QString(tr("Checking the update"));
        case Bootloader::STATUS_UPDATE_SUCCESSFUL:
            return QString(tr("Update successful"));
    }
}

bool Bootloader::openUfw(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        setStatus(STATUS_ERROR_OPEN_FILE);
        return false;
    }
    else
    {
        _ufwParser = new UfwParser();
        _ufwModel = _ufwParser->parse(fileName);
        file.close();

        if (!_ufwModel)
        {
            setStatus(STATUS_ERROR_ERROR_PARSER);
            return false;
        }
        /*if (_ufwModel->deviceType() != _node->nodeOd()->value(IndexDb::getObjectId(IndexDb::OD_PRODUCT_CODE)).toUInt())
        {
            setStatus(STATUS_ERROR_FILE_NOT_CORRESPONDING);
            delete _ufwParser;
        }
        else
        {*/
        _ufwUpdate->setUfw(_ufwModel);
        emit parserUfwFinished();
        setStatus(STATUS_FILE_ANALYZED_OK);
        _state = STATE_FREE;
        //}
    }

    return true;
}

void Bootloader::startUpdate()
{
    if (!_ufwModel)
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
    _counterError = ATTEMPT_MAX_ERROR;
    _node->nodeOd()->createBootloaderObjects();
    readStatusProgram();
}

uint32_t Bootloader::deviceType()
{
    if (!_ufwModel)
    {
        return 0;
    }
    return _ufwModel->deviceType();
}

QString Bootloader::versionSoftware()
{
    if (!_ufwModel)
    {
        return QString();
    }
    return _ufwModel->softwareVersion();
}

QString Bootloader::buildDate()
{
    if (!_ufwModel)
    {
        return QString();
    }
    return _ufwModel->buildDate();
}

void Bootloader::stopProgram()
{
    uint8_t data = PROGRAM_CONTROL_STOP;
    _node->writeObject(_programControlObjectId, data);
    _statusTimer.singleShot(TIMER_READ_STATUS_DISPLAY, this, SLOT(readStatusProgram()));
}

void Bootloader::startProgram()
{
    uint8_t data = PROGRAM_CONTROL_START;
    _node->writeObject(_programControlObjectId, data);
    _statusTimer.singleShot(TIMER_READ_STATUS_DISPLAY, this, SLOT(readStatusProgram()));
}

void Bootloader::resetProgram()
{
    uint8_t data = PROGRAM_CONTROL_RESET;
    _node->writeObject(_programControlObjectId, data);
    _statusTimer.singleShot(TIMER_READ_STATUS_DISPLAY, this, SLOT(readStatusProgram()));
}

void Bootloader::clearProgram()
{
    uint8_t data = PROGRAM_CONTROL_CLEAR;
    _node->writeObject(_programControlObjectId, data);
    _statusTimer.singleShot(TIMER_READ_STATUS_DISPLAY, this, SLOT(readStatusProgram()));
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
    uint8_t data = PROGRAM_CONTROL_UPDATE_FINISHED;
    _node->writeObject(_programControlObjectId, data);
}

void Bootloader::sendKey()
{
    if (_ufwModel)
    {
        _node->writeObject(_bootloaderKeyObjectId, _ufwModel->deviceType());
    }
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
                    setStatus(STATUS_DEVICE_CLEAR_IN_PROGRESS);
                    sendKey();
                    clearProgram();
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
        {
            setStatus(STATUS_DEVICE_UPDATE_IN_PROGRESS);
            sendKey();
            clearProgram();
            break;
        }

        case STATE_CLEAR_PROGRAM:
        {
            setStatus(STATUS_DEVICE_UPDATE_IN_PROGRESS);
            sendKey();
            clearProgram();
            break;
        }

        case STATE_UPDATE_PROGRAM:
        {
            updateStartProgram();
            setStatus(STATUS_DEVICE_UPDATE_IN_PROGRESS);
            updateProgram();
            break;
        }

        case STATE_UPLOADED_PROGRAM_FINISHED:
        {
            setStatus(STATUS_CHECKING_UPDATE);
            _node->writeObject(_bootloaderChecksumObjectId, _ufwUpdate->checksum());
            updateFinishedProgram();
            _statusTimer.singleShot(TIMER_READ_STATUS_DISPLAY, this, SLOT(readStatusBootloader()));
            _state = STATE_CHECK;
            break;
        }

        case STATE_CHECK:
            break;

        case STATE_OK:
            emit finished(true);
            resetProgram();
            _state = STATE_FREE;
            setStatus(STATUS_UPDATE_SUCCESSFUL);
            break;

        case STATE_NOT_OK:
            emit finished(false);
            _state = STATE_FREE;
            setStatus(STATUS_ERROR_UPDATE_FAILED);
            break;
    }
}

void Bootloader::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((objId.index() == 0x1000) && objId.subIndex() == 0)
    {
        uint32_t type = static_cast<uint32_t>(_node->nodeOd()->value(0x1000, 0).toUInt());
        if (type == 0x12D)
        {
            _node->nodeOd()->createBootloaderObjects();
        }
    }
    if ((objId.index() == _programControlObjectId.index()) && objId.subIndex() == 1)
    {
        if (flags == SDO::FlagsRequest::Read)
        {
            if (_state == STATE_FREE)
            {
                return;
            }

            uint8_t program = static_cast<uint8_t>(_node->nodeOd()->value(_programControlObjectId).toUInt());
            if (program == PROGRAM_CONTROL_STOP || program == PROGRAM_CONTROL_RESET)
            {
                _statusProgram = PROGRAM_STOPPED;
                _state = STATE_CLEAR_PROGRAM;
            }
            else if (program == PROGRAM_CONTROL_CLEAR)
            {
                _statusProgram = NO_PROGRAM;
                _state = STATE_UPDATE_PROGRAM;
            }
            else
            {
                _statusProgram = PROGRAM_STARTED;
            }
            process();
        }
        else if ((flags & SDO::FlagsRequest::Error) == SDO::FlagsRequest::Error)
        {
            quint32 error = static_cast<quint32>(_node->nodeOd()->errorObject(_programControlObjectId));
            if (error == SDO::CO_SDO_ABORT_CODE_CANNOT_TRANSFERRED_1)
            {
                emit status(tr("Command failed (1)"));
                _state = STATE_NOT_OK;
            }
            else if (error == SDO::CO_SDO_ABORT_CODE_CANNOT_TRANSFERRED_2)
            {
                emit status(tr("Authentification failed"));
                _state = STATE_NOT_OK;
            }
            else if (error == SDO::CO_SDO_ABORT_CODE_CANNOT_TRANSFERRED_3)
            {
                emit status(tr("Command failed (2)"));
                _state = STATE_NOT_OK;
            }
            else if (error == SDO::CO_SDO_ABORT_CODE_TIMED_OUT && _state == STATE_CLEAR_PROGRAM)
            {
                _statusTimer.singleShot(TIMER_READ_STATUS_DISPLAY, this, SLOT(readStatusProgram()));
            }
            else
            {
                emit status(tr("Command failed (3)"));
                _state = STATE_NOT_OK;
            }

            process();
        }
    }

    if ((objId.index() == _bootloaderChecksumObjectId.index()) && objId.subIndex() == _bootloaderChecksumObjectId.subIndex())
    {
    }

    if ((objId.index() == _bootloaderStatusObjectId.index()) && objId.subIndex() == _bootloaderStatusObjectId.subIndex())
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
}
