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

#ifndef INDEX_H
#define INDEX_H

#include "od_global.h"

#include <QList>
#include <QString>

#include "subindex.h"
#include "datatype.h"

#include <stdint.h>

class OD_EXPORT Index : public SubIndex
{
public:
    Index(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName, const uint16_t &index, const uint8_t &nbSubIndex);
    ~Index();

    uint16_t index() const;
    void setIndex(const uint16_t &index);

    uint8_t nbSubIndex() const;
    void setNbSubIndex(const uint8_t &nbSubIndex);

    QList<SubIndex *> &subIndexes();
    SubIndex* subIndex(const uint8_t &subIndexKey) const;
    void addSubIndex(SubIndex *subIndex);

private:
    uint16_t _index;
    uint8_t _nbSubIndex;
    QList<SubIndex*> _subIndexes;
};

#endif // INDEX_H
