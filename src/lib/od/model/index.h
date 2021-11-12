/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
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

#ifndef INDEX_H
#define INDEX_H

#include "od_global.h"

#include <QMap>
#include <QString>

#include "subindex.h"

#include <stdint.h>

class OD_EXPORT Index
{
public:
    Index(const uint16_t &index);
    Index(const Index &other);
    ~Index();

    uint16_t index() const;
    void setIndex(const uint16_t &index);

    uint8_t maxSubIndex() const;
    void setMaxSubIndex(const uint8_t &maxSubIndex);

    const QMap<uint8_t, SubIndex *> &subIndexes() const;

    SubIndex *subIndex(uint8_t subIndex) const;
    SubIndex *subIndex(QString nameSubIndex) const;
    void addSubIndex(SubIndex *subIndex);
    int subIndexesCount();
    bool subIndexExist(uint8_t subIndex);
    bool subIndexExist(QString nameSubIndex);

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
    uint8_t objectType() const;
    void setObjectType(const uint8_t &objectType);
    static QString objectTypeStr(const uint8_t &objectType);

    const QString &name() const;
    void setName(const QString &name);

private:
    uint16_t _index;
    uint8_t _maxSubIndex;
    uint8_t _objectType;
    QString _name;
    QMap<uint8_t, SubIndex *> _subIndexes;
};

#endif  // INDEX_H
