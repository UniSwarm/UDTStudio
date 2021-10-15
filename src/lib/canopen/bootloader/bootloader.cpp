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

#define TIMER_READ_STATUS_DISPLAY 400

#define PROGRAM_CONTROL_STOP            0x0
#define PROGRAM_CONTROL_START           0x1
#define PROGRAM_CONTROL_RESET           0x2
#define PROGRAM_CONTROL_CLEAR           0x3
#define PROGRAM_CONTROL_UPDATE_FINISHED 0x80

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

Bootloader::StatusProgram Bootloader::statusProgram()
{
    return _statusProgram;
}

bool Bootloader::openUfw(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly))
    {
        emit status(tr("Error open file"));
        return false;
    }
    else
    {
        _ufwParser = new UfwParser();
        _ufwModel = _ufwParser->parse(fileName);
        file.close();

        _ufwUpdate->setUfw(_ufwModel);
        emit parserUfwFinished();
        emit status(tr("File analyzed"));
        _state = STATE_FREE;
    }

    return true;
}

void Bootloader::startUpdate()
{
    if (!_ufwModel)
    {
        emit status(tr("No file"));
        return;
    }
    if (_state != STATE_FREE)
    {
        emit status(tr("Update already in progress"));
        return;
    }

    emit status(tr("Check file and device"));
    _state = STATE_CHECK_MODE;
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

void Bootloader::updateFinishedProgram()
{
    uint8_t data = PROGRAM_CONTROL_UPDATE_FINISHED;
    _node->writeObject(_programControlObjectId, data);
}

void Bootloader::sendKey()
{
    _node->writeObject(_bootloaderKeyObjectId, _ufwModel->deviceType());
}

void Bootloader::readStatusProgram()
{
    _node->readObject(_programControlObjectId);
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
            emit status(tr("Device stopped"));
            _node->nodeOd()->createBootloaderObjects();

            if (_ufwModel)
            {
                sendKey();
            }
            clearProgram();
            emit status(tr("Device clear in progress"));
            break;

        case StatusProgram::PROGRAM_STARTED:
            emit status(tr("Device started"));
            if (_ufwModel)
            {
                sendKey();
            }
            stopProgram();
            break;

        case StatusProgram::NO_PROGRAM:
            _node->nodeOd()->createBootloaderObjects();

            break;

        case StatusProgram::UPDATE_IN_PROGRESS:
        case StatusProgram::SUCCESSFUL:
            break;
        case StatusProgram::NOT_SUCCESSFUL:
            _state = STATE_NOT_OK;
            break;
        }
        break;
    }
    case STATE_STOP_PROGRAM:
    {
        _node->nodeOd()->createBootloaderObjects();
        if (_ufwModel)
        {
            sendKey();
            clearProgram();
        }
        break;
    }

    case STATE_CLEAR_PROGRAM:
    {
        _node->nodeOd()->createBootloaderObjects();
        if (_ufwModel)
        {
            sendKey();
            clearProgram();
        }
        break;
    }

    case STATE_UPDATE_PROGRAM:
    {
        updateProgram();
        emit status(tr("Device update in progress"));
        break;
    }

    case STATE_UPLOADED_PROGRAM_FINISHED:
    {
        emit status(tr("Update check"));
        _node->writeObject(_bootloaderChecksumObjectId, _ufwUpdate->checksum());
        updateFinishedProgram();
        _node->readObject(_bootloaderStatusObjectId);
        _state = STATE_CHECK;
        break;
    }

    case STATE_CHECK:
        break;

    case STATE_OK:
        emit finished(true);
        resetProgram();
        _state = STATE_FREE;
        emit status(tr("Update successful"));
        break;

    case STATE_NOT_OK:
        emit finished(false);
        _state = STATE_FREE;
        emit status(tr("Update failed"));
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
        else if (flags == (SDO::FlagsRequest::Error + SDO::FlagsRequest::Write))
        {
            quint32 error = static_cast<quint32>(_node->nodeOd()->errorObject(_programControlObjectId));
            if (error == SDO::CO_SDO_ABORT_CODE_CANNOT_TRANSFERRED_1)
            {
                emit status(tr("Command failed (1)"));
            }
            else if (error == SDO::CO_SDO_ABORT_CODE_CANNOT_TRANSFERRED_2)
            {
                emit status(tr("Authentification failed"));
            }
            else if (error == SDO::CO_SDO_ABORT_CODE_CANNOT_TRANSFERRED_3)
            {
                emit status(tr("Command failed (2)"));
            }
            else
            {
                emit status(tr("Command failed (3)"));
            }
            _statusProgram = NOT_SUCCESSFUL;
            _state = STATE_NOT_OK;
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
            if (status)
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
