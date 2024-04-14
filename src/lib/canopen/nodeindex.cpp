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

#include "nodeindex.h"

#include "node.h"
#include "nodeod.h"
#include "nodesubindex.h"

NodeIndex::NodeIndex(quint16 index)
{
    _nodeOd = nullptr;
    _index = index;
    _objectType = VAR;
}

NodeIndex::NodeIndex(const NodeIndex &other)
{
    _nodeOd = nullptr;

    _index = other._index;
    _name = other._name;
    _objectType = other._objectType;

    for (NodeSubIndex *subIndex : other._nodeSubIndexes)
    {
        _nodeSubIndexes.insert(subIndex->subIndex(), new NodeSubIndex(*subIndex));
    }
}

NodeIndex::~NodeIndex()
{
    qDeleteAll(_nodeSubIndexes);
}

quint8 NodeIndex::busId() const
{
    if (_nodeOd != nullptr)
    {
        return _nodeOd->node()->busId();
    }
    return 0xFF;
}

quint8 NodeIndex::nodeId() const
{
    if (_nodeOd != nullptr)
    {
        return _nodeOd->node()->nodeId();
    }
    return 0xFF;
}

Node *NodeIndex::node() const
{
    if (_nodeOd != nullptr)
    {
        return _nodeOd->node();
    }
    return nullptr;
}

NodeOd *NodeIndex::nodeOd() const
{
    return _nodeOd;
}

NodeObjectId NodeIndex::objectId() const
{
    return NodeObjectId(busId(), nodeId(), _index, 0xFF);
}

quint16 NodeIndex::index() const
{
    return _index;
}

void NodeIndex::setIndex(quint16 index)
{
    _index = index;
}

const QString &NodeIndex::name() const
{
    return _name;
}

void NodeIndex::setName(const QString &name)
{
    _name = name;
}

/**
 * @brief object type getter
 * @return 8bits object type code
 */
NodeIndex::ObjectType NodeIndex::objectType() const
{
    return _objectType;
}

/**
 * @brief object type setter
 * @param 8 bits objects type code
 */
void NodeIndex::setObjectType(ObjectType objectType)
{
    _objectType = objectType;
}

/**
 * @brief converts an object type code to his corresponding string
 * @param 8 bits object type code
 * @return object type string
 */
QString NodeIndex::objectTypeStr(const ObjectType &objectType)
{
    switch (objectType)
    {
        case NONE:
            return QStringLiteral("NONE");
        case OBJECT_NULL:
            return QStringLiteral("NULL");
        case OBJECT_DOMAIN:
            return QStringLiteral("DOMAIN");
        case DEFTYPE:
            return QStringLiteral("DEFTYPE");
        case DEFSTRUCT:
            return QStringLiteral("DEFSTRUCT");
        case VAR:
            return QStringLiteral("VAR");
        case ARRAY:
            return QStringLiteral("ARRAY");
        case RECORD:
            return QStringLiteral("RECORD");
    }
    return QStringLiteral("");
}

/**
 * @brief max sub-index setter
 * @param max sub-index
 */
const QMap<uint8_t, NodeSubIndex *> &NodeIndex::subIndexes() const
{
    return _nodeSubIndexes;
}

/**
 * @brief return the subIndex with the number subIndex
 * @param subi-index number
 * @return a sub-index
 */
NodeSubIndex *NodeIndex::subIndex(uint8_t subIndex) const
{
    if (_nodeSubIndexes.contains(subIndex))
    {
        return _nodeSubIndexes.value(subIndex);
    }

    return nullptr;
}

/**
 * @brief inserts a new sub-index, if the sub-index already exists,
 * replaces it by the new sub-index
 * @param subIndex
 */
void NodeIndex::addSubIndex(NodeSubIndex *subIndex)
{
    _nodeSubIndexes.insert(subIndex->subIndex(), subIndex);
    subIndex->_nodeIndex = this;
}

/**
 * @brief returns the number of sub-indexes.
 * @return nummber of sub-indexes
 */
int NodeIndex::subIndexesCount()
{
    return _nodeSubIndexes.count();
}

/**
 * @brief returns true if the map contains a sub-index with the number subIndex;
 *  otherwise returns false
 * @param subIndex
 * @return true if the map contains a sub-index with the number subIndex,
 * otherwise returns false
 */
bool NodeIndex::subIndexExist(uint8_t subIndex)
{
    return _nodeSubIndexes.contains(subIndex);
}
