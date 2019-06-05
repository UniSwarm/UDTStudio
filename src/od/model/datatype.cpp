#include "datatype.h"

/**
 * @brief boolean constructor
 * @param value
 */
DataType::DataType(const bool &val)
{
    _type._bool = val;
}

/**
 * @brief int8_t constructor
 * @param value
 */
DataType::DataType(const int8_t &val)
{
    _type._int8 = val;
}

/**
 * @brief int16_t constructor
 * @param value
 */
DataType::DataType(const int16_t &val)
{
    _type._int16 = val;
}

/**
 * @brief int32_t constructor
 * @param value
 */
DataType::DataType(const int32_t &val)
{
    _type._int32 = val;
}

/**
 * @brief int64_t constructor
 * @param value
 */
DataType::DataType(const int64_t &val)
{
    _type._int64 = val;
}

/**
 * @brief uint8_t constructor
 * @param value
 */
DataType::DataType(const uint8_t &val)
{
    _type._uint8 = val;
}

/**
 *
 * @brief uint16_t constructor
 * @param value
 */
DataType::DataType(const uint16_t &val)
{
    _type._uint16 = val;
}

/**
 * @brief uint32_t constructor
 * @param value
 */
DataType::DataType(const uint32_t &val)
{
    _type._uint32 = val;
}

/**
 * @brief uint64_t constructor
 * @param value
 */
DataType::DataType(const uint64_t &val)
{
    _type._uint64 = val;
}

/**
 * @brief float32_t constructor
 * @param value
 */
DataType::DataType(const float32_t &val)
{
    _type._float32 = val;
}

/**
 * @brief float64_t constructor
 * @param value
 */
DataType::DataType(const float64_t &val)
{
    _type._float64 = val;
}

/**
 * @brief vstring_t constructor
 * @param value
 */
DataType::DataType(const vstring_t &val)
{
    _type._vstring = val;
}

/**
 * @brief returns value to specific format
 * @return a boolean
 */
bool DataType::toBool() const
{
    return _type._bool;
}

/**
 * @brief returns value to specific format
 * @return an int8_t
 */
int8_t DataType::toInt8() const
{
    return _type._int8;
}

/**
 * @brief returns value to specific format
 * @return an int16_t
 */
int16_t DataType::toInt16() const
{
    return _type._int16;
}

/**
 * @brief returns value to specific format
 * @return an int32_t
 */
int32_t DataType::toInt32() const
{
    return _type._int32;
}

/**
 * @brief returns value to specific format
 * @return an int64_t
 */
int64_t DataType::toInt64() const
{
    return _type._int64;
}

/**
 * @brief returns value to specific format
 * @return an uint8_t
 */
uint8_t DataType::toUInt8() const
{
    return _type._uint8;
}

/**
 * @brief returns value to specific format
 * @return an uint16_t
 */
uint16_t DataType::toUInt16() const
{
    return _type._uint16;
}

/**
 * @brief returns value to specific format
 * @return an uint32_t
 */
uint32_t DataType::toUInt32() const
{
    return _type._uint32;
}

/**
 * @brief returns value to specific format
 * @return an uint64_t
 */
uint64_t DataType::toUInt64() const
{
    return _type._uint64;
}

/**
 * @brief returns value to specific format
 * @return a float
 */
float32_t DataType::toFloat32() const
{
    return _type._float32;
}

/**
 * @brief returns value to specific format
 * @return a double
 */
float64_t DataType::toFloat64() const
{
    return _type._float64;
}

/**
 * @brief returns value to specific format
 * @return a visible string
 */
QString DataType::toVString() const
{
    return QString(_type._vstring);
}

