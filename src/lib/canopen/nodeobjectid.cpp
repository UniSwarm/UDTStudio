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

#include "nodeobjectid.h"

#include <QDebug>
#include <QStringList>

#include "canopen.h"

NodeObjectId::NodeObjectId()
    : busId(0xFF), nodeId(0xFF), index(0xFFFF), subIndex(0xFF), dataType(QMetaType::Type::UnknownType)
{
}

NodeObjectId::NodeObjectId(quint8 busId, quint8 nodeId, quint16 index, quint8 subIndex, QMetaType::Type dataType)
    : busId(busId), nodeId(nodeId), index(index), subIndex(subIndex), dataType(dataType)
{
}

NodeObjectId::NodeObjectId(quint16 index, quint8 subIndex, QMetaType::Type dataType)
    : busId(0xFF), nodeId(0xFF), index(index), subIndex(subIndex), dataType(dataType)
{
}

NodeObjectId::NodeObjectId(const NodeObjectId &other)
{
    busId = other.busId;
    nodeId = other.nodeId;
    index = other.index;
    subIndex = other.subIndex;
    dataType = other.dataType;
}

bool operator==(const NodeObjectId &a, const NodeObjectId &b)
{
    return (a.busId == b.busId
            && a.nodeId == b.nodeId
            && a.index == b.index
            && a.subIndex == b.subIndex);
}

quint64 NodeObjectId::key() const
{
    quint64 key = 0;
    key += static_cast<quint64>(busId) << 24;
    key += static_cast<quint64>(nodeId) << 16;
    key += static_cast<quint64>(index) << 8;
    key += static_cast<quint64>(subIndex);
    return key;
}

bool NodeObjectId::isValid() const
{
    if (busId != 0xFF && nodeId != 0xFF && index != 0xFFFF && subIndex != 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isNodeIndependant() const
{
    if (busId == 0xFF && nodeId == 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isABus() const
{
    if (busId != 0xFF && nodeId == 0xFF && index == 0xFFFF && subIndex == 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isANode() const
{
    if (busId != 0xFF && nodeId != 0xFF && index == 0xFFFF && subIndex == 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isAnIndex() const
{
    if (index != 0xFFFF && subIndex == 0xFF)
    {
        return true;
    }
    return false;
}

bool NodeObjectId::isASubIndex() const
{
    if (index != 0xFFFF && subIndex != 0xFF)
    {
        return true;
    }
    return false;
}

CanOpenBus *NodeObjectId::bus() const
{
    if (!isABus())
    {
        return nullptr;
    }
    return CanOpen::bus(busId);
}

Node *NodeObjectId::node() const
{
    if (busId == 0xFF || nodeId == 0xFF)
    {
        return nullptr;
    }
    CanOpenBus *bus = CanOpen::bus(busId);
    if (!bus)
    {
        return nullptr;
    }
    return bus->node(nodeId);
}

NodeIndex *NodeObjectId::nodeIndex() const
{
    if (busId == 0xFF || nodeId == 0xFF || index == 0xFFFF)
    {
        return nullptr;
    }
    CanOpenBus *bus = CanOpen::bus(busId);
    if (!bus)
    {
        return nullptr;
    }
    Node *node = bus->node(nodeId);
    if (!node)
    {
        return nullptr;
    }
    return node->nodeOd()->index(index);
}

NodeSubIndex *NodeObjectId::nodeSubIndex() const
{
    if (busId == 0xFF || nodeId == 0xFF || index == 0xFFFF || subIndex == 0xFF)
    {
        return nullptr;
    }
    CanOpenBus *bus = CanOpen::bus(busId);
    if (!bus)
    {
        return nullptr;
    }
    Node *node = bus->node(nodeId);
    if (!node)
    {
        return nullptr;
    }
    NodeIndex *nodeIndex = node->nodeOd()->index(index);
    if (!nodeIndex)
    {
        return nullptr;
    }
    return nodeIndex->subIndex(subIndex);
}

QString NodeObjectId::mimeData() const
{
    return QString("%1.%2.%3.%4")
        .arg(QString::number(busId, 16).rightJustified(2, '0'))
        .arg(QString::number(nodeId, 16).rightJustified(2, '0'))
        .arg(QString::number(index, 16).rightJustified(2, '0'))
        .arg(QString::number(subIndex, 16).rightJustified(2, '0'));
}

NodeObjectId NodeObjectId::fromMimeData(const QString mimeData)
{
    const QStringList fields = mimeData.split('.', QString::SkipEmptyParts);
    if (fields.count() < 4)
    {
        return NodeObjectId();
    }

    bool ok;
    return NodeObjectId(static_cast<quint8>(fields[0].toUShort(&ok, 16)),
                        static_cast<quint8>(fields[1].toUShort(&ok, 16)),
                        fields[2].toUShort(&ok, 16),
                        static_cast<quint8>(fields[3].toUShort(&ok, 16)));
}

NodeObjectId &NodeObjectId::operator=(const NodeObjectId &other)
{
    busId = other.busId;
    nodeId = other.nodeId;
    index = other.index;
    subIndex = other.subIndex;
    dataType = other.dataType;
    return *this;
}

QDebug operator<<(QDebug debug, const NodeObjectId &c)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << '(' << c.mimeData() << ')';
    return debug;
}
