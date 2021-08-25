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

#include "ufwupdate.h"

#include "../utility/phantomremover.h"

#include "indexdb.h"
#include "node.h"

#include <QDebug>
#include <QFile>
#include <QtEndian>

UfwUpdate::UfwUpdate(Node *node, UfwParser *ufw)
    : _node(node)
    , _ufw(ufw)
{
    _programDataObjectId = IndexDb::getObjectId(IndexDb::OD_PROGRAM_DATA_1);
    setNodeInterrest(_node);
    registerObjId({0x1F50, 1});
}

void UfwUpdate::setUfw(UfwParser *ufw)
{
    _ufw = ufw;
}

int UfwUpdate::update()
{
    if (!_ufw)
    {
        return -1;
    }

    char buffer[4];
    _checksum = 0;
    for (int i = 0; i < _ufw->head().countSegment; i++)
    {
        uint32_t start = _ufw->head().segmentList.at(i)->memorySegmentStart;
        uint32_t end = _ufw->head().segmentList.at(i)->memorySegmentEnd;

        QByteArray prog = _ufw->prog().mid(static_cast<int>(start), static_cast<int>(end) - static_cast<int>(start));

        PhantomRemover phantomRemover;
        QByteArray progRemove = phantomRemover.remove(prog);

        calculateCheckSum(progRemove);

        qToLittleEndian(start, buffer);
        progRemove.prepend(buffer, sizeof(start));

        _byteArrayList.append(progRemove);
    }

    _indexList = _byteArrayList.size();

    process();

    return 0;
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
        emit isUploaded(true);
        return;
    }

    _node->writeObject(_programDataObjectId, _byteArrayList.at(_indexList));
}

void UfwUpdate::calculateCheckSum(const QByteArray &prog)
{
    int checksum = 0;
    int i = 0;
    for (i = 1; i < prog.size(); i = i + 2)
    {
        checksum += prog[i];
    }
    _checksum = (~(checksum % 256) + 1) & 0xFF;
}

void UfwUpdate::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{

    if (objId.index() == _programDataObjectId.index())
    {
        if (flags == (SDO::FlagsRequest::Error | SDO::FlagsRequest::Write))
        {
            emit isUploaded(false);
        }
        else if (flags == SDO::FlagsRequest::Write)
        {
            if (!_indexList)
            {
                _indexList = 0;
                _byteArrayList.clear();
                emit isUploaded(true);
            }
            else
            {
                process();
            }
        }
        else if (flags == SDO::FlagsRequest::Read)
        {
        }
    }
}
