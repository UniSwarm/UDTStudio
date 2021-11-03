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

#ifndef NODEODITEM_H
#define NODEODITEM_H

#include "../../udtgui_global.h"

#include "nodeod.h"

class UDTGUI_EXPORT NodeOdItem
{
public:
    NodeOdItem(NodeOd *od, NodeOdItem *parent = nullptr);
    NodeOdItem(NodeIndex *index, NodeOdItem *parent = nullptr);
    NodeOdItem(NodeSubIndex *subIndex, NodeOdItem *parent = nullptr);
    ~NodeOdItem();

    enum Type
    {
        TOD,
        TIndex,
        TSubIndex
    };
    Type type() const;

    NodeOd *od() const;
    NodeIndex *index() const;
    NodeSubIndex *subIndex() const;

    int rowCount() const;
    QVariant data(int column, int role) const;
    bool setData(int column, const QVariant &value, int role, Node *node);
    Qt::ItemFlags flags(int column) const;

    const QList<NodeOdItem *> &children() const;
    NodeOdItem *parent() const;
    NodeOdItem *child(int row) const;
    NodeOdItem *childIndex(quint16 index) const;
    int row() const;

    NodeObjectId objectId() const;
    QString mimeData() const;

protected:
    Type _type;

    NodeOd *_od;
    NodeIndex *_index;
    NodeSubIndex *_subIndex;

    NodeOdItem *_parent;
    QList<NodeOdItem *> _children;
    QMap<quint16, NodeOdItem *> _childrenMap;
    void addChild(quint16 index, NodeOdItem *child);
    void createChildren();

    enum ViewType
    {
        ViewValue,
        ViewHex,
        ViewHybrid,
        EditValue,
        EditHex
    };
    QVariant formatValue(NodeSubIndex *subIndex, ViewType viewType) const;
};

#endif  // NODEODITEM_H
