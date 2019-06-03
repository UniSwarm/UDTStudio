#ifndef DATATYPE_H
#define DATATYPE_H

#include "od_global.h"

#include <QString>

#include <stdint.h>

typedef float       float32_t;
typedef double      float64_t;

typedef union
{
    bool        _bool;
    int8_t      _int8;
    int16_t     _int16;
    int32_t     _int32;
    int64_t     _int64;
    uint8_t     _uint8;
    uint16_t    _uint16;
    uint32_t    _uint32;
    uint64_t    _uint64;
    float32_t   _float32;
    float64_t   _float64;
}type;

class OD_EXPORT DataType
{
public:

    DataType();
    DataType(const bool &val);
    DataType(const int8_t &val);
    DataType(const int16_t &val);
    DataType(const int32_t &val);
    DataType(const int64_t &val);
    DataType(const uint8_t &val);
    DataType(const uint16_t &val);
    DataType(const uint32_t &val);
    DataType(const uint64_t &val);
    DataType(const float32_t &val);
    DataType(const float64_t &val);

    bool toBool() const;
    int8_t toInt8() const;
    int16_t toInt16() const;
    int32_t toInt32() const;
    int64_t toInt64() const;
    uint8_t toUInt8() const;
    uint16_t toUInt16() const;
    uint32_t toUInt32() const;
    uint64_t toUInt64() const;
    float32_t toFloat32() const;
    float64_t toFloat64() const;

private:
    type _type;
};

#endif // DATATYPE_H
