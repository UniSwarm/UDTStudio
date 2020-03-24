/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#ifndef BUSNODESMODEL_H
#define BUSNODESMODEL_H

#include "../udtgui_global.h"

#include <QAbstractItemModel>

#include "canopen.h"

class UDTGUI_EXPORT BusNodesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    BusNodesModel(QObject *parent = nullptr, CanOpen *canOpen = nullptr);
    ~BusNodesModel();

    CanOpen *canOpen() const;
    void setCanOpen(CanOpen *canOpen);

    enum Column {
        NodeId,
        Name,
        ColumnCount
    };

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    CanOpen *_canOpen;
};

#endif // BUSNODESMODEL_H
