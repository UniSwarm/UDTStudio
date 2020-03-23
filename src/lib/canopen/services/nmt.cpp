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

#include <QDebug>

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
    if ((frame.frameId() >> 7) == 0x0)
    {
        qDebug() << "NMT " << QString::number(frame.frameId(), 16).toUpper() << frame.payload().toHex().toUpper();
    }
    else if ((frame.frameId() >> 7) == 0xE)  // NMT Error control
    {
        qDebug() << "NMT Error control : Node Guarding RTR " << QString::number(frame.frameId(), 16).toUpper() << frame.payload().toHex().toUpper();
        manageErrorControl(frame);
    }
}

void NMT::manageErrorControl(const QCanBusFrame &frame)
{
    uint32_t node = frame.frameId() & 0x7F;
    switch (frame.payload()[0] & 0x7F)
    {
    case 4:  // Stopped
    case 5:  // Operational
    case 127:  // Pre-operational
        emit nodeFound(node);
        break;
    default:
        qDebug()<< "NMT Error control : error state" << QString::number(frame.frameId(), 16 ).toUpper() << frame.payload().toHex().toUpper();
        break;
    }
}

void NMT::explorerBus()
{
    if (!_bus)
    {
        return;
    }
    if (!_bus->canDevice())
    {
        return;
    }

    QCanBusFrame frameNodeGuarding;

    for (quint8 i = 1; i <= 127; i++)
    {
        frameNodeGuarding.setFrameId(0x700 + i);
        frameNodeGuarding.setFrameType(QCanBusFrame::RemoteRequestFrame);
        _bus->canDevice()->writeFrame(frameNodeGuarding);
    }
}

