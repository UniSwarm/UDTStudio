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

#include "subindex.h"

/**
 * @brief constructor
 * @param sub-index number
 */
SubIndex::SubIndex(const uint8_t subIndex)
{
    _subIndex = subIndex;
    _accessType = NONE;
    _hasNodeId = false;
    _dataType = INVALID;
}

/**
 * @brief copy constructor
 * @param other
 */
SubIndex::SubIndex(const SubIndex &other)
{
    _accessType = other.accessType();
    _subIndex = other.subIndex();
    _name = other.name();
    _value = other.value();

    _lowLimit = other.lowLimit();
    _highLimit = other.highLimit();

    _hasNodeId = other.hasNodeId();
    _dataType = other.dataType();
}

/**
 * @brief destructor
 */
SubIndex::~SubIndex()
{
}

/**
 * @brief access type getter
 * @return 8 bits access type code
 */
SubIndex::AccessType SubIndex::accessType() const
{
    return _accessType;
}

/**
 * @brief access type setter
 * @param 8 bits access type code
 */
void SubIndex::setAccessType(const AccessType accessType)
{
    _accessType = accessType;
}

/**
 * @brief sub-index number getter
 * @return 8 bits sub-index number
 */
uint8_t SubIndex::subIndex() const
{
    return _subIndex;
}

/**
 * @brief sub-index number setter
 * @param 8 bits sub-index number
 */
void SubIndex::setSubIndex(const uint8_t &subIndex)
{
    _subIndex = subIndex;
}

/**
 * @brief name getter
 * @return sub-index name
 */
const QString &SubIndex::name() const
{
    return _name;
}

/**
 * @brief name setter
 * @param new sub-index string name
 */
void SubIndex::setName(const QString &name)
{
    _name = name;
}

/**
 * @brief low limit getter
 * @return low limit value
 */
const QVariant &SubIndex::lowLimit() const
{
    return _lowLimit;
}

/**
 * @brief low limit values
 * @param new low limit value
 */
void SubIndex::setLowLimit(const QVariant &lowLimit)
{
    _lowLimit = lowLimit;
}

bool SubIndex::hasLowLimit() const
{
    return _lowLimit.isValid();
}

/**
 * @brief high limit getter
 * @return high limit value
 */
const QVariant &SubIndex::highLimit() const
{
    return _highLimit;
}

/**
 * @brief high limit setter
 * @param new high limit value
 */
void SubIndex::setHighLimit(const QVariant &highLimit)
{
    _highLimit = highLimit;
}

bool SubIndex::hasHighLimit() const
{
    return _highLimit.isValid();
}

/**
 * @brief _hasNoddId getter
 * @return true if the sub-index value depends of a node-id
 */
bool SubIndex::hasNodeId() const
{
    return _hasNodeId;
}

/**
 * @brief _hasNodeId setter
 * @param new bool
 */
void SubIndex::setHasNodeId(bool hasNodeId)
{
    _hasNodeId = hasNodeId;
}

/**
 * @brief _value getter
 * @return return sub-index value
 */
const QVariant &SubIndex::value() const
{
    return _value;
}

/**
 * @brief _value setter
 * @param new sub-index value
 */
void SubIndex::setValue(const QVariant &value)
{
    _value.setValue(value);
}

/**
 * @brief _value setter
 * @param new sub-index value
 */
void SubIndex::clearValue()
{
    _value.clear();
}

/**
 * @brief _dataType getter
 * @return 16 bits sub-index data type code
 */
SubIndex::DataType SubIndex::dataType() const
{
    return _dataType;
}

/**
 * @brief _dataType setter
 * @param new 16 bits data type code
 */
void SubIndex::setDataType(const DataType dataType)
{
    _dataType = dataType;
}

/**
 * @brief converts an data type code to his corresponding string
 * @param 16 bits data type code
 * @return data type string
 */
QString SubIndex::dataTypeStr(const DataType dataType)
{
    switch (dataType)
    {
    case INVALID:
        return QString("INVALID");
    case BOOLEAN:
        return QString("BOOLEAN");
    case INTEGER8:
        return QString("INT8");
    case INTEGER16:
        return QString("INT16");
    case INTEGER32:
        return QString("INT32");
    case UNSIGNED8:
        return QString("UINT8");
    case UNSIGNED16:
        return QString("UINT16");
    case UNSIGNED32:
        return QString("UINT32");
    case REAL32:
        return QString("REAL32");
    case VISIBLE_STRING:
        return QString("VSTRING");
    case OCTET_STRING:
        return QString("OSTRING");
    case UNICODE_STRING:
        return QString("USTRING");
    case TIME_OF_DAY:
        return QString("DAYTIME");
    case TIME_DIFFERENCE:
        return QString("TIMEDIFF");
    case DDOMAIN:
        return QString("DOMAIN");
    case INTEGER24:
        return QString("INT24");
    case REAL64:
        return QString("REAL64");
    case INTEGER40:
        return QString("INT40");
    case INTEGER48:
        return QString("INT48");
    case INTEGER56:
        return QString("INT56");
    case INTEGER64:
        return QString("INT64");
    case UNSIGNED24:
        return QString("UINT24");
    case UNSIGNED40:
        return QString("UINT40");
    case UNSIGNED48:
        return QString("UINT48");
    case UNSIGNED56:
        return QString("UINT56");
    case UNSIGNED64:
        return QString("UINT64");
    }
    return QString();
}

/**
 * @brief return the length of the sub-index value
 * @return octet sub-index value length
 */
int SubIndex::length() const
{
    switch (_dataType)
    {
    case SubIndex::INVALID:
        return 0;

    case SubIndex::VISIBLE_STRING:
    case SubIndex::OCTET_STRING:
    case SubIndex::UNICODE_STRING:
        break;
    case SubIndex::TIME_OF_DAY:
        break;
    case SubIndex::TIME_DIFFERENCE:
        break;
    case SubIndex::DDOMAIN:
        break;

    case SubIndex::BOOLEAN:
    case SubIndex::UNSIGNED8:
    case SubIndex::INTEGER8:
        return 1;

    case SubIndex::UNSIGNED16:
    case SubIndex::INTEGER16:
        return 2;

    case SubIndex::UNSIGNED24:
    case SubIndex::INTEGER24:
        return 3;

    case SubIndex::UNSIGNED32:
    case SubIndex::INTEGER32:
    case SubIndex::REAL32:
        return 4;

    case SubIndex::UNSIGNED40:
    case SubIndex::INTEGER40:
        return 5;

    case SubIndex::UNSIGNED48:
    case SubIndex::INTEGER48:
        return 6;

    case SubIndex::UNSIGNED56:
    case SubIndex::INTEGER56:
        return 7;

    case SubIndex::UNSIGNED64:
    case SubIndex::INTEGER64:
    case SubIndex::REAL64:
        return 8;
    }
    return 0;
}
