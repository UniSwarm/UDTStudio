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

#include "nodeobjectid.h"

#include <QDebug>
#include <QStringList>

#include "canopen.h"

NodeObjectId::NodeObjectId()
    : _busId(0xFF)
    , _nodeId(0xFF)
    , _index(0xFFFF)
    , _subIndex(0xFF)
    , _dataType(QMetaType::Type::UnknownType)
{
}

NodeObjectId::NodeObjectId(quint8 busId, quint8 nodeId, quint16 index, quint8 subIndex, QMetaType::Type dataType)
    : _busId(busId)
    , _nodeId(nodeId)
    , _index(index)
    , _subIndex(subIndex)
    , _dataType(dataType)
{
}

NodeObjectId::NodeObjectId(quint16 index, quint8 subIndex, QMetaType::Type dataType)
    : _busId(0xFF)
    , _nodeId(0xFF)
    , _index(index)
    , _subIndex(subIndex)
    , _dataType(dataType)
{
}

NodeObjectId::NodeObjectId(const NodeObjectId &other)
{
    _busId = other.busId();
    _nodeId = other.nodeId();
    _index = other.index();
    _subIndex = other.subIndex();
    _dataType = other.dataType();
}

bool operator==(const NodeObjectId &a, const NodeObjectId &b)
{
    return (a.busId() == b.busId() && a.nodeId() == b.nodeId() && a.index() == b.index() && a.subIndex() == b.subIndex());
}

quint64 NodeObjectId::key() const
{
    quint64 key = 0;
    key += static_cast<quint64>(_busId) << 24;
    key += static_cast<quint64>(_nodeId) << 16;
    key += static_cast<quint64>(_index) << 8;
    key += static_cast<quint64>(_subIndex);
    return key;
}

bool NodeObjectId::isValid() const
{
    return (_busId != 0xFF) && (_nodeId != 0xFF) && (_index != 0xFFFF) && (_subIndex != 0xFF);
}

bool NodeObjectId::isNodeIndependant() const
{
    return (_busId == 0xFF) && (_nodeId == 0xFF);
}

bool NodeObjectId::isABus() const
{
    return (_busId != 0xFF) && (_nodeId == 0xFF) && (_index == 0xFFFF) && (_subIndex == 0xFF);
}

bool NodeObjectId::isANode() const
{
    return (_busId != 0xFF) && (_nodeId != 0xFF) && (_index == 0xFFFF) && (_subIndex == 0xFF);
}

bool NodeObjectId::isAnIndex() const
{
    return (_index != 0xFFFF) && (_subIndex == 0xFF);
}

bool NodeObjectId::isASubIndex() const
{
    return (_index != 0xFFFF) && (_subIndex != 0xFF);
}

CanOpenBus *NodeObjectId::bus() const
{
    if (!isABus())
    {
        return nullptr;
    }
    return CanOpen::bus(_busId);
}

Node *NodeObjectId::node() const
{
    if (_busId == 0xFF || _nodeId == 0xFF)
    {
        return nullptr;
    }
    CanOpenBus *bus = CanOpen::bus(_busId);
    if (bus == nullptr)
    {
        return nullptr;
    }
    return bus->node(_nodeId);
}

NodeIndex *NodeObjectId::nodeIndex() const
{
    if (_busId == 0xFF || _nodeId == 0xFF || _index == 0xFFFF)
    {
        return nullptr;
    }
    CanOpenBus *bus = CanOpen::bus(_busId);
    if (bus == nullptr)
    {
        return nullptr;
    }
    Node *node = bus->node(_nodeId);
    if (node == nullptr)
    {
        return nullptr;
    }
    return node->nodeOd()->index(_index);
}

NodeSubIndex *NodeObjectId::nodeSubIndex() const
{
    if (_busId == 0xFF || _nodeId == 0xFF || _index == 0xFFFF || _subIndex == 0xFF)
    {
        return nullptr;
    }
    CanOpenBus *bus = CanOpen::bus(_busId);
    if (bus == nullptr)
    {
        return nullptr;
    }
    Node *node = bus->node(_nodeId);
    if (node == nullptr)
    {
        return nullptr;
    }
    return node->nodeOd()->subIndex(_index, _subIndex);
}

QString NodeObjectId::mimeData() const
{
    return QString("%1.%2.%3.%4")
        .arg(QString::number(_busId, 16).rightJustified(2, '0'),
             QString::number(_nodeId, 16).rightJustified(2, '0'),
             QString::number(_index, 16).rightJustified(4, '0'),
             QString::number(_subIndex, 16).rightJustified(2, '0'));
}

NodeObjectId NodeObjectId::fromMimeData(const QString &mimeData)
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

quint8 NodeObjectId::nodeId() const
{
    return _nodeId;
}

void NodeObjectId::setNodeId(quint8 nodeId)
{
    _nodeId = nodeId;
}

void NodeObjectId::setBusIdNodeId(quint8 busId, quint8 nodeId)
{
    _nodeId = nodeId;
    _busId = busId;
}

quint16 NodeObjectId::index() const
{
    return _index;
}

void NodeObjectId::setIndex(quint16 index)
{
    _index = index;
}

quint8 NodeObjectId::subIndex() const
{
    return _subIndex;
}

void NodeObjectId::setSubIndex(quint8 subIndex)
{
    _subIndex = subIndex;
}

QMetaType::Type NodeObjectId::dataType() const
{
    return _dataType;
}

void NodeObjectId::setDataType(QMetaType::Type dataType)
{
    _dataType = dataType;
}

quint8 NodeObjectId::busId() const
{
    return _busId;
}

void NodeObjectId::setBusId(quint8 busId)
{
    _busId = busId;
}

NodeObjectId &NodeObjectId::operator=(const NodeObjectId &other)
{
    _busId = other.busId();
    _nodeId = other.nodeId();
    _index = other.index();
    _subIndex = other.subIndex();
    _dataType = other.dataType();
    return *this;
}

quint8 NodeObjectId::bitSize() const
{
    switch (_dataType)
    {
        case QMetaType::Bool:
            return 1;

        case QMetaType::Char:
        case QMetaType::UChar:
        case QMetaType::SChar:
            return 8;

        case QMetaType::Short:
        case QMetaType::UShort:
            return 16;

        case QMetaType::UInt:
        case QMetaType::Int:
        case QMetaType::Float:
            return 32;

        case QMetaType::Double:
        case QMetaType::Long:
        case QMetaType::ULong:
            return 64;

        default:
            return 0;
    }
}

QDebug operator<<(QDebug debug, const NodeObjectId &c)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << '(' << c.mimeData() << '.' << c.dataType() << ')';
    return debug;
}
