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

#include "node.h"
#include "parser/ufwparser.h"
#include "utility/ufwupdate.h"
#include "indexdb.h"

#include <QFile>
#include <QDebug>

#define TIMER_READ_STATUS_DISPLAY 400

Bootloader::Bootloader(Node *node)
    : _node(node)
{
    _node = node;
    _ufw = nullptr;
    _ufwUpdate = new UfwUpdate(_node);
    connect(_ufwUpdate, &UfwUpdate::isUploaded, this, &Bootloader::isUploaded);

    _bootloaderKeyObjectId = IndexDb::getObjectId(IndexDb::OD_BOOTLOADER_KEY);
    _bootloaderChecksumObjectId = IndexDb::getObjectId(IndexDb::OD_BOOTLOADER_CHECKSUM);
    _bootloaderStatusObjectId = IndexDb::getObjectId(IndexDb::OD_BOOTLOADER_STATUS);
    _programControlObjectId = IndexDb::getObjectId(IndexDb::OD_PROGRAM_CONTROL_1);

    setNodeInterrest(_node);
    registerObjId({0x2050, 1});
    registerObjId({0x2050, 2});
    registerObjId({0x2050, 3});
    registerObjId({0x1F51, 1});

    _statusProgram = NONE;
}

quint8 Bootloader::busId() const
{
    return _node->busId();
}

quint8 Bootloader::nodeId() const
{
    return _node->nodeId();
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
        return false;
    }
    else
    {
        _ufw = new UfwParser(fileName);
        _ufw->read();
        file.close();

        _ufwUpdate->setUfw(_ufw);
    }

    return true;
}

void Bootloader::update()
{
    _state = STATE_CHECK_MODE;
    getStatusProgram();
}

void Bootloader::stopProgram()
{
    uint8_t data = 0;
    _node->writeObject(_programControlObjectId, data);
    _statusTimer.singleShot(TIMER_READ_STATUS_DISPLAY, this, SLOT(getStatusProgram()));
}

void Bootloader::startProgram()
{
    uint8_t data = 1;
    _node->writeObject(_programControlObjectId, data);
}

void Bootloader::resetProgram()
{
    uint8_t data = 2;
    _node->writeObject(_programControlObjectId, data);
    _statusTimer.singleShot(TIMER_READ_STATUS_DISPLAY, this, SLOT(getStatusProgram()));
}

void Bootloader::clearProgram()
{
    uint8_t data = 3;
    _node->writeObject(_programControlObjectId, data);
    _statusTimer.singleShot(TIMER_READ_STATUS_DISPLAY, this, SLOT(getStatusProgram()));
}

void Bootloader::uploadProgram()
{
    _ufwUpdate->update();
}

void Bootloader::uploadFinishedProgram()
{
    uint8_t data = 0x80;
    _node->writeObject(_programControlObjectId, data);
}

void Bootloader::sendKey(uint16_t key)
{
    _node->writeObject(_bootloaderKeyObjectId, key);
}

void Bootloader::getStatusProgram()
{
    _node->readObject(_programControlObjectId);
}

void Bootloader::isUploaded(bool ok)
{
    if (ok)
    {
        _state = STATE_UPLOADED_FINISHED;
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
        break;

    case STATE_CHECK_MODE:

        switch (_statusProgram)
        {
        case Bootloader::NONE:
            getStatusProgram();
            break;

        case Bootloader::PROGRAM_STOPPED:
            _node->nodeOd()->createMandatoryBootloaderObject();

            if (_ufw)
            {
                sendKey(_ufw->head().device);
            }
            clearProgram();
            _state = STATE_CLEAR_PROGRAM;
            break;

        case Bootloader::PROGRAM_STARTED:
            if (_ufw)
            {
                sendKey(_ufw->head().device);
            }
            stopProgram();
            _state = STATE_STOP_PROGRAM;
            break;

        case Bootloader::NO_PROGRAM:
            _node->nodeOd()->createMandatoryBootloaderObject();
            uploadProgram();
            break;

        case Bootloader::UPDATE_IN_PROGRESS:
            break;

        case Bootloader::SUCCESSFUL:
            break;

        case Bootloader::NOT_SUCCESSFUL:
            break;
        }
        break;

    case STATE_STOP_PROGRAM:
    {
        _node->nodeOd()->createMandatoryBootloaderObject();
        if (_ufw)
        {
            sendKey(_ufw->head().device);
        }
        clearProgram();
        _state = STATE_CLEAR_PROGRAM;
        break;
    }

    case STATE_CLEAR_PROGRAM:
    {
        uploadProgram();
        break;
    }

    case STATE_UPDATE_PROGRAM:
    {
        break;
    }

    case STATE_UPLOADED_FINISHED:
    {
        _node->writeObject(_bootloaderChecksumObjectId, _ufwUpdate->checksum());

        uint8_t data = 0x80;
        _node->writeObject(_programControlObjectId, data);
        _node->readObject(_bootloaderStatusObjectId);
        _state = STATE_CHECK;
        break;
    }

    case STATE_CHECK:
    {
        _state = STATE_OK;
        break;
    }

    case STATE_OK:
        emit isUpdated(true);
        resetProgram();
        _state = STATE_FREE;
        break;

    case STATE_NOT_OK:
        emit isUpdated(false);
        _state = STATE_FREE;
        break;
    }
}

uint8_t Bootloader::calculateCheckSum(const QByteArray &prog)
{
    uint8_t checksum = 0;
    for (int i = 0; i < prog.size(); i++)
    {
        checksum += static_cast<unsigned char>(prog[i]);
    }
    return checksum;
}

void Bootloader::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((objId.index() == _programControlObjectId.index()) && objId.subIndex() == 1)
    {
        if (flags == SDO::FlagsRequest::Read)
        {
            if (_state == STATE_FREE)
            {
                return;
            }

            uint8_t program = static_cast<uint8_t>(_node->nodeOd()->value(_programControlObjectId).toUInt());
            if (program == 0 || program == 2)
            {
                // BOOTLOADER MODE
                _statusProgram = PROGRAM_STOPPED;
            }
            else if (program == 3)
            {
                // BOOTLOADER MODE
                _statusProgram = NO_PROGRAM;
            }
            else
            {
                // APP MODE
                _statusProgram = PROGRAM_STARTED;
            }
            process();
        }
    }

    if ((objId.index() == _bootloaderChecksumObjectId.index()) && objId.subIndex() == _bootloaderChecksumObjectId.subIndex())
    {
        uint8_t status = static_cast<uint8_t>(_node->nodeOd()->value(_bootloaderChecksumObjectId).toUInt());

        if (status)
        {

        }
        else
        {

        }
    }
    if ((objId.index() == _bootloaderStatusObjectId.index()) && objId.subIndex() == _bootloaderStatusObjectId.subIndex())
    {
        uint8_t status = static_cast<uint8_t>(_node->nodeOd()->value(_bootloaderStatusObjectId).toUInt());

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
