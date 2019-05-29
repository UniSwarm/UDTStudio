#include "datatype.h"

/**
 * @brief DataType::DataType
 */
DataType::DataType()
{

}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const bool &val)
{
    _type._bool = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const int8_t &val)
{
    _type._int8 = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const int16_t &val)
{
    _type._int16 = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const int32_t &val)
{
    _type._int32 = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const int64_t &val)
{
    _type._int64 = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const uint8_t &val)
{
    _type._uint8 = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const uint16_t &val)
{
    _type._uint16 = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const uint32_t &val)
{
    _type._uint32 = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const uint64_t &val)
{
    _type._uint64 = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const float32_t &val)
{
    _type._float32 = val;
}
/**
 * @brief DataType::DataType
 * @param val
 */
DataType::DataType(const float64_t &val)
{
    _type._float64 = val;
}
/**
 * @brief DataType::toBool
 * @return
 */
bool DataType::toBool() const
{
    return _type._bool;
}
/**
 * @brief DataType::toInt8
 * @return
 */
int8_t DataType::toInt8() const
{
    return _type._int8;
}
/**
 * @brief DataType::toInt16
 * @return
 */
int16_t DataType::toInt16() const
{
    return _type._int16;
}
/**
 * @brief DataType::toInt32
 * @return
 */
int32_t DataType::toInt32() const
{
    return _type._int32;
}
/**
 * @brief DataType::toInt64
 * @return
 */
int64_t DataType::toInt64() const
{
    return _type._int64;
}

uint8_t DataType::toUInt8() const
{
    return _type._uint8;
}
/**
 * @brief DataType::toUInt16
 * @return
 */
uint16_t DataType::toUInt16() const
{
    return _type._uint16;
}
/**
 * @brief DataType::toUInt32
 * @return
 */
uint32_t DataType::toUInt32() const
{
    return _type._uint32;
}
/**
 * @brief DataType::toUInt64
 * @return
 */
uint64_t DataType::toUInt64() const
{
    return _type._uint64;
}
/**
 * @brief DataType::toFloat32
 * @return
 */
float32_t DataType::toFloat32() const
{
    return _type._float32;
}
/**
 * @brief DataType::toFloat64
 * @return
 */
float64_t DataType::toFloat64() const
{
    return _type._float64;
}

