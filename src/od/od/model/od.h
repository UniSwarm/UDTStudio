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

    Index *index(const uint16_t &index) const;
    void addIndex(Index *index);
    int indexCount() const;

    QMap<uint16_t, Index *> &indexes();

private:
    QMap<uint16_t, Index*> _indexes;
};

#endif // OD_H
