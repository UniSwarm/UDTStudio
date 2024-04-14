/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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
#include <QDir>
#include <QProcessEnvironment>

#include "../profile/nodeprofilefactory.h"
#include "canopenbus.h"

#include "db/oddb.h"

NodeDiscover::NodeDiscover(CanOpenBus *bus)
    : Service(bus)
{
    for (quint8 nodeId = 1; nodeId <= 127; nodeId++)
    {
        _cobIds.append(0x700U + nodeId);
    }

    _exploreBusNodeId = 0;
    connect(&_exploreBusTimer, &QTimer::timeout, this, &NodeDiscover::exploreBusNext);

    _exploreNodeState = -1;
    _exploreNodeCurrentId = 0;
    connect(&_exploreNodeTimer, &QTimer::timeout, this, &NodeDiscover::exploreNodeNext);
}

NodeDiscover::~NodeDiscover()
{
}

QString NodeDiscover::type() const
{
    return QStringLiteral("NodeDiscover");
}

void NodeDiscover::parseFrame(const QCanBusFrame &frame)
{
    if ((frame.frameId() >= 0x701) && (frame.frameId() <= 0x7FF) && frame.frameType() == QCanBusFrame::DataFrame)
    {
        uint8_t nodeId = frame.frameId() & 0x7F;
        if (!bus()->existNode(nodeId))
        {
            Node *node = new Node(nodeId);

            if (frame.payload().size() == 1)
            {
                switch (frame.payload().at(0) & 0x7F)
                {
                    case 0:  // Bootup
                        node->setStatus(Node::Status::INIT);
                        node->setStatus(Node::Status::PREOP);
                        break;

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
                        break;
                }
            }
            bus()->addNode(node);

            exploreNode(nodeId);
        }
    }
}

void NodeDiscover::exploreBus()
{
    if (_exploreBusNodeId != 0)
    {
        return;
    }
    if (!bus()->canWrite())
    {
        return;
    }
    _exploreBusNodeId = 1;
    _exploreBusTimer.start(8);
}

void NodeDiscover::exploreNode(quint8 nodeId)
{
    _nodeIdToExplore.enqueue(nodeId);

    if (_exploreNodeState == -1)
    {
        _exploreNodeCurrentId = _nodeIdToExplore.dequeue();
        _exploreNodeState = 0;
        _exploreNodeTimer.start(20);
    }
}

void NodeDiscover::exploreBusNext()
{
    if (!bus()->canWrite())
    {
        return;
    }

    if (_exploreBusNodeId > 127)
    {
        _exploreBusNodeId = 0;
        _exploreBusTimer.stop();
        emit exploreFinished();
        return;
    }

    QCanBusFrame frameNodeGuarding;
    frameNodeGuarding.setFrameId(0x700 + _exploreBusNodeId);
    frameNodeGuarding.setFrameType(QCanBusFrame::RemoteRequestFrame);
    bus()->writeFrame(frameNodeGuarding);

    _exploreBusNodeId++;
}

void NodeDiscover::exploreNodeNext()
{
    QList<NodeObjectId> objectsId{{0x1000, 0x0}, {0x1018, 0x1}, {0x1018, 0x2}, {0x1018, 0x3}};

    Node *node = bus()->node(_exploreNodeCurrentId);
    if (_exploreNodeState >= objectsId.size()
        && (node->nodeOd()->value(objectsId[_exploreNodeState - 1]).isValid() || node->nodeOd()->errorObject(objectsId[_exploreNodeState - 1]) != 0))
    {
        // explore node finished
        Node *node = bus()->node(_exploreNodeCurrentId);
        QString file = OdDb::file(node->nodeOd()->value(0x1000).toUInt(),
                                  node->nodeOd()->value(0x1018, 1).toUInt(),
                                  node->nodeOd()->value(0x1018, 2).toUInt(),
                                  node->nodeOd()->value(0x1018, 3).toUInt());

        // load object eds
        if (!file.isEmpty())
        {
            node->nodeOd()->loadEds(file);
            node->reset();
            NodeProfileFactory::profileFactory(node);
        }

        if (_nodeIdToExplore.isEmpty())
        {
            _exploreNodeState = -1;
            _exploreNodeTimer.stop();
        }
        else
        {
            _exploreNodeCurrentId = _nodeIdToExplore.dequeue();
            _exploreNodeState = 0;
        }
    }
    else
    {
        if ((_exploreNodeState >= 1
             && (!node->nodeOd()->value(objectsId[_exploreNodeState - 1]).isValid() || node->nodeOd()->errorObject(objectsId[_exploreNodeState - 1]) != 0)))
        {
            _nodeIdToExplore.enqueue(_exploreNodeCurrentId);
            _exploreNodeCurrentId = _nodeIdToExplore.dequeue();
            _exploreNodeState = 0;
        }
        else
        {
            node->readObject(objectsId[_exploreNodeState]);
            _exploreNodeState++;
        }
    }
}
