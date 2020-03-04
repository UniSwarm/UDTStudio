/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#include "sdo.h"

#include "canopenbus.h"

#include <QDebug>

SDO::SDO(CanOpenBus *bus)
    : Service (bus)
{
}

void SDO::sendSdoReadReq(uint8_t nodeId, uint16_t index, uint8_t subindex)
{
    if (!_bus)
        return;
    if (!_bus->canDevice())
        return;

    QByteArray sdoReadReqPayload;
    uint8_t cmd = 0x40;
    QDataStream data(&sdoReadReqPayload, QIODevice::WriteOnly);
    data.setByteOrder(QDataStream::LittleEndian);
    data << cmd;
    data << index;
    data << subindex;
    data << static_cast<uint32_t>(0);

    QCanBusFrame frameNmt;
    frameNmt.setFrameId(0x600 + nodeId);
    frameNmt.setPayload(sdoReadReqPayload);
    _bus->canDevice()->writeFrame(frameNmt);
}

void SDO::sendSdoWriteReq(uint8_t nodeId, uint16_t index, uint8_t subindex, const QVariant &value, uint8_t size)
{
    if (!_bus)
        return;
    if (!_bus->canDevice())
        return;

    if (size <= 4)
    {
        // expedited with size
        QByteArray sdoWriteReqPayload;

        uint8_t cmd = 0x20 + static_cast<uint8_t>((4 - size) << 2) + 0x03;
        QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
        data.setByteOrder(QDataStream::LittleEndian);
        data << cmd;
        data << index;
        data << subindex;
        data << value.toUInt();

        QCanBusFrame frameNmt;
        frameNmt.setFrameId(0x600 + nodeId);
        frameNmt.setPayload(sdoWriteReqPayload);
        _bus->canDevice()->writeFrame(frameNmt);
    }

}

void SDO::parseFrame(const QCanBusFrame &frame)
{
    uint8_t node_id = frame.frameId() & 0x07F;

    uint8_t sdoCmd = static_cast<uint8_t>(frame.payload().at(0));
    qDebug() << QString::number(frame.frameId(), 16) << QString::number(sdoCmd, 16);
    if ((sdoCmd & 0xF0) == 0x40)    // SDO read req response
    {
        uint8_t size = 4 - ((sdoCmd & 0x0C) >> 2);
        int32_t value = *((int32_t*)(&frame.payload().data()[4]));
        qDebug() << "rec" << frame.payload().toHex() << node_id << size << value;
    }
}
