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

#include "subindex.h"

/**
 * @brief constructor
 * @param sub-index number
 */
SubIndex::SubIndex(uint8_t subIndex)
    : _index(nullptr),
      _accessType(NONE),
      _subIndex(subIndex),
      _dataType(INVALID),
      _objFlags(0),
      _hasNodeId(false),
      _isHexVal(false)
{
}

/**
 * @brief copy constructor
 * @param other
 */
SubIndex::SubIndex(const SubIndex &other)
    : _index(nullptr),
      _accessType(other.accessType()),
      _subIndex(other.subIndex()),
      _name(other.name()),
      _value(other.value()),
      _dataType(other.dataType()),
      _lowLimit(other.lowLimit()),
      _highLimit(other.highLimit()),
      _objFlags(other.objFlags()),
      _hasNodeId(other.hasNodeId()),
      _isHexVal(other.isHexValue())
{
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
void SubIndex::setAccessType(AccessType accessType)
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
void SubIndex::setSubIndex(uint8_t subIndex)
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

bool SubIndex::isHexValue() const
{
    return _isHexVal;
}

void SubIndex::setHexValue(bool hex)
{
    _isHexVal = hex;
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
void SubIndex::setDataType(DataType dataType)
{
    _dataType = dataType;
}

/**
 * @brief converts an data type code to his corresponding string
 * @param 16 bits data type code
 * @return data type string
 */
QString SubIndex::dataTypeStr(DataType dataType)
{
    switch (dataType)
    {
        case INVALID:
            return QStringLiteral("INVALID");
        case BOOLEAN:
            return QStringLiteral("BOOLEAN");
        case INTEGER8:
            return QStringLiteral("INT8");
        case INTEGER16:
            return QStringLiteral("INT16");
        case INTEGER32:
            return QStringLiteral("INT32");
        case UNSIGNED8:
            return QStringLiteral("UINT8");
        case UNSIGNED16:
            return QStringLiteral("UINT16");
        case UNSIGNED32:
            return QStringLiteral("UINT32");
        case REAL32:
            return QStringLiteral("REAL32");
        case VISIBLE_STRING:
            return QStringLiteral("VSTRING");
        case OCTET_STRING:
            return QStringLiteral("OSTRING");
        case UNICODE_STRING:
            return QStringLiteral("USTRING");
        case TIME_OF_DAY:
            return QStringLiteral("DAYTIME");
        case TIME_DIFFERENCE:
            return QStringLiteral("TIMEDIFF");
        case DDOMAIN:
            return QStringLiteral("DOMAIN");
        case INTEGER24:
            return QStringLiteral("INT24");
        case REAL64:
            return QStringLiteral("REAL64");
        case INTEGER40:
            return QStringLiteral("INT40");
        case INTEGER48:
            return QStringLiteral("INT48");
        case INTEGER56:
            return QStringLiteral("INT56");
        case INTEGER64:
            return QStringLiteral("INT64");
        case UNSIGNED24:
            return QStringLiteral("UINT24");
        case UNSIGNED40:
            return QStringLiteral("UINT40");
        case UNSIGNED48:
            return QStringLiteral("UINT48");
        case UNSIGNED56:
            return QStringLiteral("UINT56");
        case UNSIGNED64:
            return QStringLiteral("UINT64");
    }
    return QStringLiteral();
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

uint32_t SubIndex::objFlags() const
{
    return _objFlags;
}

void SubIndex::setObjFlags(uint32_t objFlags)
{
    _objFlags = objFlags;
}

Index *SubIndex::index() const
{
    return _index;
}
