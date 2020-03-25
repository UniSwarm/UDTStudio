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

#include "nodeod.h"
#include "parser/edsparser.h"
#include "node.h"

NodeOd::NodeOd(Node *node)
{
    _nodeId = node->nodeId();
}

NodeOd::~NodeOd()
{
    qDeleteAll(_indexes);
    _indexes.clear();
}

/**
 * @brief returns the value associated with the key index
 * @param index
 * @return an Index*
 */
Index *NodeOd::index(uint16_t index) const
{
    return _indexes.value(index);
}

/**
 * @brief inserts a new index. If the index already exist, replaces it with the new index
 * @param index
 */
void NodeOd::addIndex(Index *index)
{
    _indexes.insert(index->index(), index);
}

/**
 * @brief returns the number of index in the map
 * @return number of index
 */
int NodeOd::indexCount() const
{
    return _indexes.count();
}

/**
 * @brief returns true if the map contains an item with key key; otherwise returns false.
 * @param key
 * @return boolean
 */
bool NodeOd::indexExist(uint16_t key) const
{
    return _indexes.contains(key);
}

bool NodeOd::loadEds(const QString &fileName)
{
    EdsParser parser;
    DeviceDescription *deviceDescription = parser.parse(fileName);

    foreach (Index *index, deviceDescription->indexes())
        addIndex(new Index(*index));

    foreach (Index *index, _indexes)
    {
        foreach (SubIndex *subIndex, index->subIndexes())
        {
            QString value = subIndex->value().toString();
            uint8_t base = 10;
            if (value.startsWith("0x"))
            {
                base = 16;
            }

            bool ok;
            subIndex->setValue(value.toUInt(&ok, base) + _nodeId);
        }
    }

    return true;
}
