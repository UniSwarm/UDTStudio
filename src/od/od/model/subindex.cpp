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

#include "subindex.h"
#include "od.h"

/**
 * @brief constructor
 * @param data type
 * @param object type
 * @param access type
 * @param parameter name
 */
SubIndex::SubIndex(const uint8_t &subIndex)
    : _data(0)
{
    _subIndex = subIndex;
}

uint8_t SubIndex::accessType() const
{
    return _accessType;
}

void SubIndex::setAccessType(const uint8_t &accessType)
{
    _accessType = accessType;
}

uint8_t SubIndex::subIndex() const
{
    return _subIndex;
}

void SubIndex::setSubIndex(const uint8_t &subIndex)
{
    _subIndex = subIndex;
}

QString SubIndex::name() const
{
    return _name;
}

void SubIndex::setName(const QString &name)
{
    _name = name;
}

DataStorage SubIndex::data() const
{
    return _data;
}

void SubIndex::setData(const DataStorage &data)
{
    _data = data;
}
