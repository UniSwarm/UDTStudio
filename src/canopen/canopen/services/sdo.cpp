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
    QByteArray nmtStopPayload;
    nmtStopPayload.append(0x40);
    nmtStopPayload.append(static_cast<char>(index & 0xFF));
    nmtStopPayload.append(static_cast<char>(index >> 8));
    nmtStopPayload.append(static_cast<char>(subindex));
    nmtStopPayload.append(static_cast<char>(0));
    nmtStopPayload.append(static_cast<char>(0));
    nmtStopPayload.append(static_cast<char>(0));
    nmtStopPayload.append(static_cast<char>(0));

    QCanBusFrame frameNmt;
    frameNmt.setFrameId(0x600 + nodeId);
    frameNmt.setPayload(nmtStopPayload);
    _bus->canDevice()->writeFrame(frameNmt);
}

void SDO::parseFrame(const QCanBusFrame &frame)
{
    bool error = false;
    uint8_t node_id = frame.frameId() & 0x07F;
    uint8_t size;

    uint8_t sdoCmd = frame.payload().at(0);
    if ((sdoCmd & 0xF0) == 0x40)    // SDO read req response
    {
        if (sdoCmd == 0x4F)
            size = 8;
        else if (sdoCmd == 0x4B)
            size = 16;
        else if (sdoCmd == 0x43)
            size = 32;
        else
            error = true;
        qDebug()<<frame.payload().toHex()<<node_id<<size;
    }
}
