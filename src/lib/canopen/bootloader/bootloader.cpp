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

#include <QFile>

Bootloader::Bootloader(Node *node)
    : _node(node)
{
    _node = node;
    _bootloaderObjectId = {_node->busId(), _node->nodeId(), 0x2050, 0, QMetaType::Type::UShort};
    _programObjectId = {_node->busId(), _node->nodeId(), 0x1F50, 1, QMetaType::Type::QByteArray};
    _programControlObjectId = {_node->busId(), _node->nodeId(), 0x1F51, 1, QMetaType::Type::UChar};
    _flashStatusObjectId = {_node->busId(), _node->nodeId(), 0x1F56, 1, QMetaType::Type::UChar};

    setNodeInterrest(_node);
    registerObjId({0x2050, 0});
    registerObjId({0x1F50, 1});
    registerObjId({0x1F51, 1});
    registerObjId({0x1F56, 1});
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

Bootloader::StatusFlash Bootloader::statusFlash()
{
    return _statusFlash;
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
    }

    return true;
}

void Bootloader::stopProgram()
{
    QVariant mdata = 0;
    mdata.convert(_programControlObjectId.dataType());
    _node->writeObject(_programControlObjectId, mdata);
}

void Bootloader::startProgram()
{
    QVariant mdata = 1;
    mdata.convert(_programControlObjectId.dataType());
    _node->writeObject(_programControlObjectId, mdata);
    _timer->singleShot(200, this, SLOT(sendSyncOne()));
}

void Bootloader::resetProgram()
{
    QVariant mdata = 2;
    mdata.convert(_programControlObjectId.dataType());
    _node->writeObject(_programControlObjectId, mdata);
    _timer->singleShot(200, this, SLOT(sendSyncOne()));
}

void Bootloader::clearProgram()
{
    QVariant mdata = 3;
    mdata.convert(_programControlObjectId.dataType());
    _node->writeObject(_programControlObjectId, mdata);
    _timer->singleShot(200, this, SLOT(sendSyncOne()));
}

void Bootloader::updateProgram()
{

}

void Bootloader::sendDevice()
{
    QVariant mdata = 0x4001;
    mdata.convert(_bootloaderObjectId.dataType());
    _node->writeObject(_bootloaderObjectId, mdata);
    _timer->singleShot(200, this, SLOT(sendSyncOne()));
}

void Bootloader::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((objId.index() == _programControlObjectId.index()) && objId.subIndex() == 1)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else if (flags == SDO::FlagsRequest::Write)
        {
        }
        else if (flags == SDO::FlagsRequest::Read)
        {
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
        }
    }

    if ((objId.index() == _bootloaderObjectId.index()) && objId.subIndex() == 0)
    {
        if (flags == SDO::FlagsRequest::Error)
        {

        }
        else
        {

        }
    }
}
