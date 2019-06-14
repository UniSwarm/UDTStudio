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

#ifndef OD_H
#define OD_H

#include "od_global.h"

#include <QMap>

#include "index.h"

#include <stdint.h>

class OD_EXPORT OD
{
public:
    OD();
    ~OD();

    Index* index(const uint16_t &indexKey) const;
    void addIndex(Index *index);
    int indexCount() const;

    QMap<uint16_t, Index *> &indexes();

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

    // =========== Object type ====================
    enum Object
    {
        OBJECT_NULL = 0x00,
        OBJECT_DOMAIN = 0x02,
        DEFTYPE = 0x05,
        DEFSTRUCT = 0x06,
        VAR = 0x07,
        ARRAY = 0x08,
        RECORD = 0x09
    };

    // ============== Access type =================
     enum Access
     {
         READ_WRITE = 0x01,
         WRITE_ONLY	= 0x02,
         READ_ONLY = 0x03,
         CONST = 0x04
     };

private:
    QMap<uint16_t, Index*> _indexes;
};

#endif // OD_H
