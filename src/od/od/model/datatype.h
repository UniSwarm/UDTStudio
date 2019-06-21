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

#ifndef DATATYPE_H
#define DATATYPE_H

#include "od_global.h"

#include <QString>

#include <stdint.h>

typedef float       float32_t;
typedef double      float64_t;

class OD_EXPORT DataType
{
public:
    // =============== Data type ==================
    enum Type
    {
        BOOLEAN = 0x0001,
        INTEGER8 = 0x0002,
        INTEGER16 = 0x0003,
        INTEGER32 = 0x0004,
        UNSIGNED8 = 0x0005,
        UNSIGNED16 = 0x0006,
        UNSIGNED32 = 0x0007,
        REAL32 = 0x0008,
        VISIBLE_STRING = 0x0009,
        OCTET_STRING = 0x000A,
        UNICODE_STRING = 0x000B,
        TIME_OF_DAY = 0x000C,
        TIME_DIFFERENCE = 0x000D,
        DOMAIN = 0x000F,
        INTEGER24 = 0x0010,
        REAL64 = 0x0011,
        INTEGER40 = 0x0012,
        INTEGER48 = 0x0013,
        INTEGER56 = 0x0014,
        INTEGER64 = 0x0015,
        UNSIGNED24 = 0x0016,
        UNSIGNED40 = 0x0018,
        UNSIGNED48 = 0x0019,
        UNSIGNED56 = 0x001A,
        UNSIGNED64 = 0x001B
    };

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
    DataType(const QString &val);

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
    QString toString() const;

private:
    typedef union
    {
        bool _bool;
        int8_t _int8;
        int16_t _int16;
        int32_t _int32;
        int64_t _int64;
        uint8_t _uint8;
        uint16_t _uint16;
        uint32_t _uint32;
        uint64_t _uint64;
        float32_t _float32;
        float64_t _float64;
    } type;

    type _type;
    QString _string;
};

#endif // DATATYPE_H
