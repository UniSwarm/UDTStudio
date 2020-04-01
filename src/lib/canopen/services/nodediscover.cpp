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

#include "nodediscover.h"

#include <QDebug>

#include "canopenbus.h"

NodeDiscover::NodeDiscover(CanOpenBus *bus)
    : Service (bus)
{
    for (quint8 nodeId = 1; nodeId <= 127; nodeId++)
    {
        _cobIds.append(0x700u + nodeId);
    }
    connect(&_exploreTimer, &QTimer::timeout, this, &NodeDiscover::exploreNext);
}

QString NodeDiscover::type() const
{
    return QLatin1String("NodeDiscover");
}

void NodeDiscover::parseFrame(const QCanBusFrame &frame)
{
    if ((frame.frameId() >= 0x701) && (frame.frameId() <= 0x7FF) /*&& frame.frameType() == QCanBusFrame::DataFrame*/)
    {
        uint8_t nodeId = frame.frameId() & 0x7F;
        if (bus()->existNode(nodeId) == false)
        {
            Node *node = new Node(nodeId);

            switch (frame.payload().at(0) & 0x7F)
            {
            case 4:  // Stopped
                node->setStatus(Node::Status::STOPPED);
                break;
            case 5:  // Operational
                node->setStatus(Node::Status::STARTED);
                break;
            case 127:  // Pre-operational
                node->setStatus(Node::Status::PREOP);
                break;
            default:
                qDebug() << "> ServiceDispatcher::parseFrame : error status of node";
                break;
            }
            qDebug() << "> ServiceDispatcher::parseFrame" << "Add NodeID : " << node << "Status of node : " << node->statusStr();
            bus()->addNode(node);
        }
    }
}

void NodeDiscover::explore()
{
    _exploreId = 1;
    _exploreTimer.start(8);
}

void NodeDiscover::exploreNext()
{
    if (_exploreId > 127)
    {
        _exploreTimer.stop();
        return;
    }

    QCanBusFrame frameNodeGuarding;
    frameNodeGuarding.setFrameId(0x700 + _exploreId);
    frameNodeGuarding.setFrameType(QCanBusFrame::RemoteRequestFrame);
    canDevice()->writeFrame(frameNodeGuarding);

    _exploreId++;
}
