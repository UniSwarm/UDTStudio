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
#include "errorcontrol.h"
#include "canopenbus.h"

ErrorControl::ErrorControl(Node *node) : Service(node)
{

    _cobId = 0x700;
    _cobIds.append(_cobId + node->nodeId());
    toggleBit = false;
}

QString ErrorControl::type() const
{
    return QLatin1String("ErrorControl");
}

void ErrorControl::parseFrame(const QCanBusFrame &frame)
{
    if (static_cast<uint8_t>(frame.payload()[0]) == 0x0)
    {
        // BootUp
        toggleBit = false;
    }

    manageErrorControl(frame);
}

void ErrorControl::receiveHeartBeat()
{
    // heartbeat consumers
    QCanBusFrame frameNodeGuarding;
    frameNodeGuarding.setFrameId(_cobId + _node->nodeId());
    frameNodeGuarding.setFrameType(QCanBusFrame::RemoteRequestFrame);
    _node->bus()->canDevice()->writeFrame(frameNodeGuarding);
}

void ErrorControl::sendNodeGuarding()
{
    QCanBusFrame frameNodeGuarding;
    frameNodeGuarding.setFrameId(_cobId + _node->nodeId());
    frameNodeGuarding.setFrameType(QCanBusFrame::RemoteRequestFrame);
    _node->bus()->canDevice()->writeFrame(frameNodeGuarding);
}

void ErrorControl::manageErrorControl(const QCanBusFrame &frame)
{
    if (toggleBit == frame.payload()[0] >> 7)
    {
        // ERROR TogleBit -> connection lost
        return;
    }
    toggleBit = frame.payload()[0] >> 7;

    switch (frame.payload()[0] & 0x7F)
    {
    case 4:  // Stopped
        _node->setStatus(Node::Status::STOPPED);
        break;
    case 5:  // Operational
        _node->setStatus(Node::Status::STARTED);
        break;
    case 127:  // Pre-operational
        _node->setStatus(Node::Status::PREOP);
        break;
    default:
        qDebug()<< "Error control : error state" << QString::number(frame.frameId(), 16 ).toUpper() << frame.payload().toHex().toUpper();
        break;
    }
}
