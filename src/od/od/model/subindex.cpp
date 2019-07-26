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
 * @param data type
 * @param object type
 * @param access type
 * @param parameter name
 */
SubIndex::SubIndex(const uint8_t &subIndex)
{
    _subIndex = subIndex;
    _flagLimit = 0;
    _accessType = 0;
    _hasNodeId = false;
    _value = QVariant(0);
    _dataType = 0;
}

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

SubIndex::~SubIndex()
{

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

QVariant SubIndex::lowLimit() const
{
    return _lowLimit;
}

void SubIndex::setLowLimit(const QVariant &lowLimit)
{
    _lowLimit = lowLimit;
}

QVariant SubIndex::highLimit() const
{
    return _highLimit;
}

void SubIndex::setHighLimit(const QVariant &highLimit)
{
    _highLimit = highLimit;
}

uint8_t SubIndex::flagLimit() const
{
    return _flagLimit;
}

void SubIndex::setFlagLimit(const uint8_t &flagLimit)
{
    _flagLimit = flagLimit;
}

bool SubIndex::hasNodeId() const
{
    return _hasNodeId;
}

void SubIndex::setHasNodeId(bool hasNodeId)
{
    _hasNodeId = hasNodeId;
}

const QVariant &SubIndex::value() const
{
    return _value;
}

void SubIndex::setValue(const QVariant &value)
{
    _value.setValue(value);
}

uint16_t SubIndex::dataType() const
{
    return _dataType;
}

void SubIndex::setDataType(const uint16_t &dataType)
{
    _dataType = dataType;
}

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
    case DOMAIN:
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
