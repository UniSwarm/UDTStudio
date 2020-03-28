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

#include "nodeod.h"
#include "parser/edsparser.h"
#include "node.h"

NodeOd::NodeOd(Node *node)
    : _node(node)
{
}

NodeOd::~NodeOd()
{
    qDeleteAll(_nodeIndexes);
    _nodeIndexes.clear();
}

/**
 * @brief returns the value associated with the key index
 * @param index
 * @return an Index*
 */
NodeIndex *NodeOd::index(uint16_t index) const
{
    return _nodeIndexes.value(index);
}

/**
 * @brief inserts a new index. If the index already exist, replaces it with the new index
 * @param index
 */
void NodeOd::addIndex(NodeIndex *index)
{
    _nodeIndexes.insert(index->index(), index);
    index->_nodeOd = this;
}

/**
 * @brief returns the number of index in the map
 * @return number of index
 */
int NodeOd::indexCount() const
{
    return _nodeIndexes.count();
}

/**
 * @brief returns true if the map contains an item with key key; otherwise returns false.
 * @param key
 * @return boolean
 */
bool NodeOd::indexExist(uint16_t key) const
{
    return _nodeIndexes.contains(key);
}

bool NodeOd::loadEds(const QString &fileName)
{
    EdsParser parser;
    DeviceDescription *deviceDescription = parser.parse(fileName);
    _fileName = fileName;

    foreach (Index *index, deviceDescription->indexes())
    {
        NodeIndex *nodeIndex = new NodeIndex(index->index());
        nodeIndex->setName(index->name());
        nodeIndex->setObjectType(static_cast<NodeIndex::ObjectType>(index->objectType()));
        addIndex(nodeIndex);

        foreach (SubIndex *subIndex, index->subIndexes())
        {
            NodeSubIndex *nodeSubIndex = new NodeSubIndex(subIndex->subIndex());
            nodeSubIndex->setName(subIndex->name());
            nodeSubIndex->setAccessType(static_cast<NodeSubIndex::AccessType>(subIndex->accessType()));
            nodeSubIndex->setValue(subIndex->value());
            nodeSubIndex->setDataType(static_cast<NodeSubIndex::DataType>(subIndex->dataType()));
            nodeSubIndex->setLowLimit(subIndex->lowLimit());
            nodeSubIndex->setHighLimit(subIndex->highLimit());
            nodeIndex->addSubIndex(nodeSubIndex);
        }
    }
    qDebug() << ">loadEds :" << fileName;
    return true;
}
