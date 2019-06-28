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

#ifndef ODITEM_H
#define ODITEM_H

#include "udtgui_global.h"

#include "model/od.h"

class UDTGUI_EXPORT ODItem
{
public:
    ODItem(OD *od, ODItem *parent=nullptr);
    ODItem(Index *index, ODItem *parent=nullptr);
    ODItem(SubIndex *subIndex, ODItem *parent=nullptr);
    ~ODItem();

    enum Type {
        TOD,
        TIndex,
        TSubIndex
    };
    Type type() const;

    OD *od() const;
    Index *index() const;
    SubIndex *subIndex() const;

    int rowCount() const;
    QVariant data(int column, int role) const;
    bool setData(int column, const QVariant &value, int role);

    ODItem *parent() const;
    ODItem *child(int id) const;
    int row() const;

protected:
    Type _type;

    OD *_od;
    Index *_index;
    SubIndex *_subIndex;

    ODItem *_parent;
    QList<ODItem *> _children;
    void createChildren();
};

#endif // ODITEM_H
