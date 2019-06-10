/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#include "index.h"

/**
 * @brief constructor
 * @param data type
 * @param object type
 * @param access type
 * @param parameter name
 * @param index
 * @param sub-index number
 */
Index::Index(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName, const uint16_t &index, const uint8_t &nbSubIndex)
    : SubIndex(dataType, objectType, accessType, parameterName)
{
    _index = index;
    _nbSubIndex = nbSubIndex;
}

/**
 * @brief destuctor
 */
Index::~Index()
{
    qDeleteAll(_subIndexes.begin(), _subIndexes.end());
    _subIndexes.clear();
}

/**
 * @brief _index getter
 * @return index number
 */
uint16_t Index::index() const
{
    return _index;
}

/**
 * @brief _index setter
 * @param index number
 */
void Index::setIndex(const uint16_t &index)
{
    _index = index;
}

/**
 * @brief _nbSubIndex setter
 * @return number of sub-indexes
 */
uint8_t Index::nbSubIndex() const
{
    return _nbSubIndex;
}

/**
 * @brief _nbSubIndex setter
 * @param number of sub-indexes
 */
void Index::setNbSubIndex(const uint8_t &nbSubIndex)
{
    _nbSubIndex = nbSubIndex;
}

/**
 * @brief _subIndexes getter
 * @return list of sub-index
 */
QList<SubIndex*> &Index::subIndexes()
{
    return _subIndexes;
}

/**
 * @brief finds a sub-index
 * @param sub-index number
 * @return sub-index
 */
SubIndex* Index::subIndex(const uint8_t &subIndexKey) const
{
     SubIndex *subIndex = _subIndexes.value(subIndexKey);
     return subIndex;
}

/**
 * @brief adds a sub-index
 * @param sub-index
 */
void Index::addSubIndex(SubIndex *subIndex)
{
    _subIndexes.append(subIndex);
}
