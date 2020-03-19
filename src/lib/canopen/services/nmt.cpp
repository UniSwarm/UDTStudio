/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#include "nmt.h"

#include "canopenbus.h"

NMT::NMT(CanOpenBus *bus)
    : Service (bus)
{

}

void NMT::sendNmt(uint8_t node_id, uint8_t cmd)
{
    if (!_bus)
        return;
    if (!_bus->canDevice())
        return;

    QByteArray nmtStopPayload;
    nmtStopPayload.append(static_cast<char>(cmd));
    nmtStopPayload.append(static_cast<char>(node_id));
    QCanBusFrame frameNmt;
    frameNmt.setFrameId(0x000);
    frameNmt.setPayload(nmtStopPayload);
    _bus->canDevice()->writeFrame(frameNmt);
}

void NMT::sendStart(uint8_t node_id)
{
    sendNmt(node_id, 0x01);
}

void NMT::sendStop(uint8_t node_id)
{
    sendNmt(node_id, 0x02);
}

void NMT::parseFrame(const QCanBusFrame &frame)
{

}
