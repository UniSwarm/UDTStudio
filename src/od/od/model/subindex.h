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

#include "datastorage.h"

class OD_EXPORT SubIndex
{
public:
    SubIndex(const uint8_t &subIndex);
    ~SubIndex();

    // ============== Access type =================
     enum Access
     {
         READ = 0x01,
         WRITE = 0x02,
         TPDO = 0x04,
         RPDO = 0x08
     };

    uint8_t accessType() const;
    void setAccessType(const uint8_t &accessType);

    uint8_t subIndex() const;
    void setSubIndex(const uint8_t &subIndex);

    QString name() const;
    void setName(const QString &name);

    const DataStorage &data() const;
    DataStorage &data();
    void setData(const DataStorage &data);

protected:
    uint8_t _accessType;
    uint8_t _subIndex;
    QString _name;
    DataStorage _data;
};

#endif // SUBINDEX_H
