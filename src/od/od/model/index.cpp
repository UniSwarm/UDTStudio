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
Index::Index(const uint16_t &index)
{
    _index = index;
    _maxSubIndex = 0;
}

/**
 * @brief destuctor
 */
Index::~Index()
{
    qDeleteAll(_subIndexes.begin(), _subIndexes.end());
    _subIndexes.clear();
}

uint16_t Index::index() const
{
    return _index;
}

void Index::setIndex(const uint16_t &index)
{
    _index = index;
}

uint8_t Index::maxSubIndex() const
{
    return _maxSubIndex;
}

void Index::setMaxSubIndex(const uint8_t &maxSubIndex)
{
    _maxSubIndex = maxSubIndex;
}

QMap<uint8_t, SubIndex *> &Index::subIndexes()
{
    return _subIndexes;
}

SubIndex *Index::subIndex(uint8_t subIndex)
{
    if (_subIndexes.contains(subIndex))
        return _subIndexes.value(subIndex);

   return nullptr;
}

void Index::addSubIndex(SubIndex *subIndex)
{
    _subIndexes.insert(subIndex->subIndex(), subIndex);
}

int Index::subIndexesCount()
{
    return _subIndexes.count();
}

uint8_t Index::objectType() const
{
    return _objectType;
}

void Index::setObjectType(const uint8_t &objectType)
{
    _objectType = objectType;
}

QString Index::name() const
{
    return _name;
}

void Index::setName(const QString &name)
{
    _name = name;
}
