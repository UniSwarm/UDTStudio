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
    _ufwModel = new UfwModel();
}

UfwParser::~UfwParser()
{
    delete _ufwModel;
}

UfwModel *UfwParser::parse(const QString &fileName) const
{
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

    uint8_t index = 0;

    uint16_t device;
    qFromLittleEndian<quint16>(uni, sizeof(device), &device);
    _ufwModel->setDevice(device);
    index = sizeof(_ufwModel->device());

    uint8_t count;
    qFromLittleEndian<uint8_t>(uni.mid(index, sizeof(uint8_t)), sizeof(uint8_t), &count);
    _ufwModel->setCountSegment(count);
    index += sizeof(_ufwModel->countSegment());

    uint8_t vendorId;
    qFromLittleEndian<uint8_t>(uni.mid(index, sizeof(uint8_t)), sizeof(uint8_t), &vendorId);
    _ufwModel->setCountSegment(vendorId);
    index += sizeof(_ufwModel->vendorId());

    uint8_t productId;
    qFromLittleEndian<uint8_t>(uni.mid(index, sizeof(uint8_t)), sizeof(uint8_t), &productId);
    _ufwModel->setCountSegment(productId);
    index += sizeof(_ufwModel->productId());

    uint8_t revision;
    qFromLittleEndian<uint8_t>(uni.mid(index, sizeof(uint8_t)), sizeof(uint8_t), &revision);
    _ufwModel->setCountSegment(revision);
    index += sizeof(_ufwModel->revision());

    uint8_t serial;
    qFromLittleEndian<uint8_t>(uni.mid(2, sizeof(uint8_t)), sizeof(uint8_t), &serial);
    _ufwModel->setCountSegment(serial);
    index += sizeof(_ufwModel->serial());

    QList<UfwModel::Segment *> segmentList;

    for (int i = 0; i < _ufwModel->countSegment(); i++)
    {
        UfwModel::Segment *seg = new UfwModel::Segment();

        qFromLittleEndian<uint32_t>(uni.mid(3 + i * 8, sizeof(seg->memorySegmentStart)), sizeof(seg->memorySegmentStart), &seg->memorySegmentStart);
        qFromLittleEndian<uint32_t>(uni.mid(7 + i * 8, sizeof(seg->memorySegmentEnd)), sizeof(seg->memorySegmentEnd), &seg->memorySegmentEnd);
        segmentList.append(seg);
    }
    _ufwModel->setSegmentList(segmentList);

    _ufwModel->setProg(uni.mid(index + _ufwModel->countSegment() * 8, uni.size()));

    return _ufwModel;
}
