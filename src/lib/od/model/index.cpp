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

#include "index.h"

/**
 * @brief constructor
 * @param 16 bits index number
 */
Index::Index(const uint16_t &index)
{
    _index = index;
    _maxSubIndex = 0;
    _objectType = VAR;
}

/**
 * @brief copy constructor
 * @param other
 */
Index::Index(const Index &other)
{
    _index = other.index();
    _maxSubIndex = other.maxSubIndex();
    _objectType = other.objectType();
    _name = other.name();

    for (SubIndex *subIndex : other._subIndexes)
    {
        _subIndexes.insert(subIndex->subIndex(), new SubIndex(*subIndex));
    }
}

/**
 * @brief destuctor
 */
Index::~Index()
{
    qDeleteAll(_subIndexes);
}

/**
 * @brief index number getter
 * @return 16 bits index number
 */
uint16_t Index::index() const
{
    return _index;
}

/**
 * @brief index number setter
 * @param 16 bits index number
 */
void Index::setIndex(const uint16_t &index)
{
    _index = index;
}

/**
 * @brief max sub-index getter
 * @return max sub-index
 */
uint8_t Index::maxSubIndex() const
{
    return _maxSubIndex;
}

void Index::setMaxSubIndex(const uint8_t &maxSubIndex)
{
    _maxSubIndex = maxSubIndex;
}

/**
 * @brief max sub-index setter
 * @param max sub-index
 */
const QMap<uint8_t, SubIndex *> &Index::subIndexes() const
{
    return _subIndexes;
}

/**
 * @brief return the subIndex with the number subIndex
 * @param subi-index number
 * @return a sub-index
 */
SubIndex *Index::subIndex(uint8_t subIndex) const
{
    if (_subIndexes.contains(subIndex))
    {
        return _subIndexes.value(subIndex);
    }

    return nullptr;
}

/**
 * @brief inserts a new sub-index, if the sub-index already exists,
 * replaces it by the new sub-index
 * @param subIndex
 */
void Index::addSubIndex(SubIndex *subIndex)
{
    _subIndexes.insert(subIndex->subIndex(), subIndex);
}

/**
 * @brief returns the number of sub-indexes.
 * @return nummber of sub-indexes
 */
int Index::subIndexesCount()
{
    return _subIndexes.count();
}

/**
 * @brief returns true if the map contains a sub-index with the number subIndex;
 *  otherwise returns false
 * @param subIndex
 * @return true if the map contains a sub-index with the number subIndex,
 * otherwise returns false
 */
bool Index::subIndexExist(uint8_t subIndex)
{
    return _subIndexes.contains(subIndex);
}

/**
 * @brief object type getter
 * @return 8bits object type code
 */
uint8_t Index::objectType() const
{
    return _objectType;
}

/**
 * @brief object type setter
 * @param 8 bits objects type code
 */
void Index::setObjectType(const uint8_t &objectType)
{
    _objectType = objectType;
}

/**
 * @brief converts an object type code to his corresponding string
 * @param 8 bits object type code
 * @return object type string
 */
QString Index::objectTypeStr(const uint8_t &objectType)
{
    switch (objectType)
    {
    case OBJECT_NULL:
        return QString("NULL");
    case OBJECT_DOMAIN:
        return QString("DOMAIN");
    case DEFTYPE:
        return QString("DEFTYPE");
    case DEFSTRUCT:
        return QString("DEFSTRUCT");
    case VAR:
        return QString("VAR");
    case ARRAY:
        return QString("ARRAY");
    case RECORD:
        return QString("RECORD");
    }
    return QString();
}

/**
 * @brief name getter
 * @return the index parameter name
 */
const QString &Index::name() const
{
    return _name;
}

/**
 * @brief name setter
 * @param new index name string
 */
void Index::setName(const QString &name)
{
    _name = name;
}
