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

#ifndef UFWMODEL_H
#define UFWMODEL_H

#include "canopen_global.h"

#include <QByteArray>
#include <QList>
#include <QString>

class CANOPEN_EXPORT UfwModel
{
public:
    UfwModel();

    struct Segment
    {
        uint32_t memorySegmentStart;
        uint32_t memorySegmentEnd;
    };

    uint16_t device() const;
    void setDevice(uint16_t device);
    uint32_t vendorId() const;
    void setVendorId(uint32_t vendorId);
    uint32_t productId() const;
    void setProductId(uint32_t productId);
    uint32_t revision() const;
    void setRevision(uint32_t revision);
    uint32_t serial() const;
    void setSerial(uint32_t newSerial);
    uint8_t countSegment() const;
    void setCountSegment(uint8_t countSegment);
    const QList<Segment *> &segmentList() const;
    void setSegmentList(const QList<Segment *> &segmentList);
    const QByteArray &prog() const;
    void setProg(const QByteArray &prog);

  private:
    uint16_t _device;
    //        QString version;
    //        QString date;
    uint32_t _vendorId;
    uint32_t _productId;
    uint32_t _revision;
    uint32_t _serial;
    uint8_t _countSegment;
    QList<Segment *> _segmentList;

    QByteArray _prog;
};

#endif // UFWMODEL_H
