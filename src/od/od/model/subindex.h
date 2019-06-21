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

#ifndef SUBINDEX_H
#define SUBINDEX_H

#include "od_global.h"

#include<QList>
#include<QString>

#include "datatype.h"

class OD_EXPORT SubIndex
{
public:
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

    SubIndex(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName);
    ~SubIndex();

    uint16_t dataType() const;
    void setDataType(const uint16_t &dataType);

    uint8_t objectType() const;
    void setObjectType(const uint8_t &objectType);

    uint8_t accessType() const;
    void setAccessType(const uint8_t &accessType);

    QList<DataType*> &datas();
    DataType* data(const uint16_t &dataKey) const;
    void addData(DataType *data);

    QString parameterName() const;
    void setParameterName(const QString &parameterName);

    uint8_t length() const;

    uint8_t subNumber() const;
    void setSubNumber(const uint8_t &subNumber);

protected:
    uint16_t _dataType;
    uint8_t _objectType;
    uint8_t _accessType;
    uint8_t _subNumber;
    QString _parameterName;
    QList<DataType*> _datas;
};

#endif // SUBINDEX_H
