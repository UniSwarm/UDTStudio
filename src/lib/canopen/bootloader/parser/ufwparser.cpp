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

#include "ufwparser.h"

#include <QFile>
#include <QtEndian>

#include "bootloader/model/ufwmodel.h"

UfwParser::UfwParser()
{
}

UfwModel *UfwParser::parse(const QString &fileName) const
{
    UfwModel *ufw;
    QByteArray uni;
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
    {
        return nullptr;
    }
    else
    {
        uni = file.read(file.size());
        file.close();
    }

    ufw = new UfwModel();


    qFromLittleEndian<quint16>(uni, sizeof(ufw->head()->device), &ufw->head()->device);
    qFromLittleEndian<quint8>(uni.mid(2, sizeof(ufw->head()->countSegment)), sizeof(ufw->head()->countSegment), &ufw->head()->countSegment);

    for (int i = 0; i < ufw->head()->countSegment; i++)
    {
        UfwModel::Segment *seg = new UfwModel::Segment();

        qFromLittleEndian<uint32_t>(uni.mid(3 + i * 8, sizeof(seg->memorySegmentStart)), sizeof(seg->memorySegmentStart), &seg->memorySegmentStart);
        qFromLittleEndian<uint32_t>(uni.mid(7 + i * 8, sizeof(seg->memorySegmentEnd)), sizeof(seg->memorySegmentEnd), &seg->memorySegmentEnd);
        ufw->head()->segmentList.append(seg);
    }

    ufw->setProg(uni.mid(3 + ufw->head()->countSegment * 8, uni.size()));

    return ufw;
}

//bool UfwParser::read()
//{
//    QByteArray uni;
//    QFile file(_fileName);
//    if (!file.open(QFile::ReadOnly))
//    {
//        return false;
//    }
//    else
//    {
//        uni = file.read(file.size());
//        file.close();
//    }

//    _head = new Head();

//    qFromLittleEndian<quint16>(uni, sizeof(_head->device), &_head->device);
//    qFromLittleEndian<quint8>(uni.mid(2, sizeof(_head->countSegment)), sizeof(_head->countSegment), &_head->countSegment);

//    for (int i = 0; i < _head->countSegment; i++)
//    {
//        Segment *seg = new Segment();

//        qFromLittleEndian<uint32_t>(uni.mid(3 + i * 8, sizeof(seg->memorySegmentStart)), sizeof(seg->memorySegmentStart), &seg->memorySegmentStart);
//        qFromLittleEndian<uint32_t>(uni.mid(7 + i * 8, sizeof(seg->memorySegmentEnd)), sizeof(seg->memorySegmentEnd), &seg->memorySegmentEnd);
//        _head->segmentList.append(seg);
//    }

//    _prog = uni.mid(3 + _head->countSegment * 8, uni.size());
//    return true;
//}

//const QByteArray &UfwParser::prog() const
//{
//    return _prog;
//}

//const UfwParser::Head &UfwParser::head() const
//{
//    return *_head;
//}
