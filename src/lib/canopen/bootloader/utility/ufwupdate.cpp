/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include "ufwupdate.h"

#include "../utility/phantomremover.h"

#include "indexdb.h"
#include "node.h"

#include <QDebug>
#include <QFile>
#include <QtEndian>

#include "bootloader/model/ufwmodel.h"

UfwUpdate::UfwUpdate(Node *node, UfwModel *ufwModel)
    : _node(node),
      _ufwModel(ufwModel)
{
    _programDataObjectId = IndexDb::getObjectId(IndexDb::OD_PROGRAM_DATA_1);
    setNodeInterrest(_node);
    registerObjId({0x1F50, 1});

    _checksum = 0;
    _indexList = -1;
    _transferSize = 0;
}

void UfwUpdate::setUfw(UfwModel *ufwModel)
{
    _ufwModel = ufwModel;
}

void UfwUpdate::update()
{
    if (_ufwModel == nullptr)
    {
        emit finished(false);
        return;
    }

    if (_ufwModel->prog().size() == 0)
    {
        emit finished(false);
        return;
    }

    char buffer[4];
    uint32_t sum = 0;
    uint32_t transferSize = 0;
    for (UfwModel::Segment segment : _ufwModel->segmentList())
    {
        uint32_t start = segment.start;
        uint32_t end = segment.end;

        QByteArray prog = _ufwModel->prog().mid(static_cast<int>(start), static_cast<int>(end) - static_cast<int>(start));

        PhantomRemover phantomRemover;
        QByteArray progRemove = phantomRemover.remove(prog);

        sum += sumByte(progRemove);
        removeByte(progRemove);

        qToLittleEndian(start, buffer);
        progRemove.prepend(buffer, sizeof(start));

        _byteArrayList.append(progRemove);
        transferSize += progRemove.size();
    }

    _transferSize = transferSize;
    _checksum = (~(sum % 256) + 1) & 0xFF;
    _indexList = _byteArrayList.size();

    process();
}

uint8_t UfwUpdate::checksum() const
{
    return _checksum;
}

void UfwUpdate::process()
{
    _indexList--;

    if (_indexList < 0)
    {
        _indexList = 0;
        _byteArrayList.clear();
        emit finished(true);
        return;
    }

    _node->writeObject(_programDataObjectId, _byteArrayList.at(_indexList));
}

uint32_t UfwUpdate::sumByte(const QByteArray &prog)
{
    uint32_t checksum = 0;
    for (char byte : prog)
    {
        checksum += static_cast<uint8_t>(byte);
    }
    return checksum;
}

void UfwUpdate::removeByte(QByteArray &prog)
{
    char str[8] = {static_cast<char>(0xFF),
                   static_cast<char>(0xFF),
                   static_cast<char>(0xFF),
                   static_cast<char>(0xFF),
                   static_cast<char>(0xFF),
                   static_cast<char>(0xFF),
                   static_cast<char>(0xFF),
                   static_cast<char>(0xFF)};

    int j = 0;
    while ((j = prog.indexOf(str, j)) != -1)
    {
        prog.remove(j, 8);
    }
}

double UfwUpdate::progress() const
{
    if (_byteArrayList.isEmpty())
    {
        return 0;
    }

    int byteSent = 0;
    for (int index = _byteArrayList.size() - 1; index >= _indexList; index--)
    {
        byteSent += _byteArrayList.at(index).size();
    }

    int currentTransfertStay = _node->sdoClients().first()->currentRequestStay();
    byteSent -= currentTransfertStay;

    return static_cast<double>(byteSent) / _transferSize * 100.0;
}

void UfwUpdate::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if (_byteArrayList.isEmpty())
    {
        return;
    }

    if (objId.index() == _programDataObjectId.index())
    {
        if ((flags & NodeOd::FlagsRequest::Error) == NodeOd::FlagsRequest::Error)
        {
            _indexList = 0;
            _byteArrayList.clear();
            emit finished(false);
        }
        else if (flags == NodeOd::FlagsRequest::Write)
        {
            if (_indexList == 0)
            {
                _indexList = 0;
                _byteArrayList.clear();
                emit finished(true);
            }
            else
            {
                process();
            }
        }
        else if (flags == NodeOd::FlagsRequest::Read)
        {
        }
    }
}
