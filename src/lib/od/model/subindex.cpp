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

/**
 * @brief constructor
 * @param sub-index number
 */
SubIndex::SubIndex(const uint8_t subIndex)
{
    _subIndex = subIndex;
    _flagLimit = 0;
    _accessType = 0;
    _hasNodeId = false;
    _value = QVariant(0);
    _dataType = 0;
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

    _flagLimit = other.flagLimit();
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
uint8_t SubIndex::accessType() const
{
    return _accessType;
}

/**
 * @brief access type setter
 * @param 8 bits access type code
 */
void SubIndex::setAccessType(const uint8_t &accessType)
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

/**
 * @brief flag limit getter
 * @return 0x1 if sub-index has low limit, 0x2 if has high limit and 0x3 if has both. Else 0.
 */
uint8_t SubIndex::flagLimit() const
{
    return _flagLimit;
}

/**
 * @brief flag limit setter
 * @param new flag limit code
 */
void SubIndex::setFlagLimit(const uint8_t &flagLimit)
{
    _flagLimit = flagLimit;
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
SubIndex::Type SubIndex::dataType() const
{
    switch (_dataType)
    {
    case BOOLEAN:
        return BOOLEAN;
    case INTEGER8:
        return INTEGER8;
    case INTEGER16:
        return INTEGER16;
    case INTEGER32:
        return INTEGER32;
    case UNSIGNED8:
        return UNSIGNED8;
    case UNSIGNED16:
        return UNSIGNED16;
    case UNSIGNED32:
        return UNSIGNED32;
    case REAL32:
        return REAL32;
    case VISIBLE_STRING:
        return VISIBLE_STRING;
    case OCTET_STRING:
        return OCTET_STRING;
    case UNICODE_STRING:
        return UNICODE_STRING;
    case TIME_OF_DAY:
        return TIME_OF_DAY;
    case TIME_DIFFERENCE:
        return TIME_DIFFERENCE;
    case DDOMAIN:
        return DDOMAIN;
    case INTEGER24:
        return INTEGER24;
    case REAL64:
        return REAL64;
    case INTEGER40:
        return INTEGER40;
    case INTEGER48:
        return INTEGER48;
    case INTEGER56:
        return INTEGER56;
    case INTEGER64:
        return INTEGER64;
    case UNSIGNED24:
        return UNSIGNED24;
    case UNSIGNED40:
        return UNSIGNED40;
    case UNSIGNED48:
        return UNSIGNED48;
    case UNSIGNED56:
        return UNSIGNED56;
    case UNSIGNED64:
        return UNSIGNED64;
    }
}

/**
 * @brief _dataType setter
 * @param new 16 bits data type code
 */
void SubIndex::setDataType(const uint16_t &dataType)
{
    _dataType = dataType;
}

/**
 * @brief converts an data type code to his corresponding string
 * @param 16 bits data type code
 * @return data type string
 */
QString SubIndex::dataTypeStr(const uint16_t &dataType)
{
    switch (dataType)
    {
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
    case BOOLEAN:
    case UNSIGNED8:
    case INTEGER8:
        return 1;

    case UNSIGNED16:
    case INTEGER16:
        return 2;

    case UNSIGNED32:
    case INTEGER32:
    case REAL32:
        return 4;

    case UNSIGNED64:
    case INTEGER64:
    case REAL64:
        return 8;
    }
    return 0;
}
