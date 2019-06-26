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

#include "datastorage.h"

DataStorage::DataStorage(const uint16_t dataType, const QVariant &value)
{
    _dataType = dataType;
    _value = value;
}

DataStorage::DataStorage(const DataStorage &other)
{
    _dataType = other._dataType;
    _value = other._value;
}

bool DataStorage::toBool() const
{
    return _value.toBool();
}

/**
 * @brief returns value to specific format
 * @return an int8_t
 */
int8_t DataStorage::toInt8() const
{
    return (int8_t)_value.toInt();
}

/**
 * @brief returns value to specific format
 * @return an int16_t
 */
int16_t DataStorage::toInt16() const
{
    return (int16_t)_value.toInt();
}

/**
 * @brief returns value to specific format
 * @return an int32_t
 */
int32_t DataStorage::toInt32() const
{
    return _value.toInt();
}

/**
 * @brief returns value to specific format
 * @return an int64_t
 */
int64_t DataStorage::toInt64() const
{
    return _value.toInt();
}

/**
 * @brief returns value to specific format
 * @return an uint8_t
 */
uint8_t DataStorage::toUInt8() const
{
    return (uint8_t)_value.toUInt();
}

/**
 * @brief returns value to specific format
 * @return an uint16_t
 */
uint16_t DataStorage::toUInt16() const
{
    return (uint16_t)_value.toUInt();
}

/**
 * @brief returns value to specific format
 * @return an uint32_t
 */
uint32_t DataStorage::toUInt32() const
{
    return _value.toUInt();
}

/**
 * @brief returns value to specific format
 * @return an uint64_t
 */
uint64_t DataStorage::toUInt64() const
{
    return _value.toUInt();
}

/**
 * @brief returns value to specific format
 * @return a float
 */
float32_t DataStorage::toFloat32() const
{
    return _value.toFloat();
}

/**
 * @brief returns value to specific format
 * @return a double
 */
float64_t DataStorage::toFloat64() const
{
    return _value.toDouble();
}

/**
 * @brief returns value to specific format
 * @return a string
 */
QString DataStorage::toString() const
{
    return _value.toString();
}

const QVariant &DataStorage::value() const
{
    return _value;
}

void DataStorage::setValue(const QVariant &value)
{
    _value.setValue(value);
}

uint16_t DataStorage::dataType() const
{
    return _dataType;
}

void DataStorage::setDataType(const uint16_t &dataType)
{
    _dataType = dataType;
}
