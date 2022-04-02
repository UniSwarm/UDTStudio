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

#ifndef BUSNODESMODEL_H
#define BUSNODESMODEL_H

#include "../udtgui_global.h"

#include <QAbstractItemModel>

#include "canopen.h"

class UDTGUI_EXPORT BusNodesModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    BusNodesModel(QObject *parent = nullptr);
    BusNodesModel(CanOpen *canOpen, QObject *parent = nullptr);
    ~BusNodesModel() override;

    CanOpen *canOpen() const;
    void setCanOpen(CanOpen *canOpen);

    CanOpenBus *bus(const QModelIndex &index) const;
    Node *node(const QModelIndex &index) const;

    enum Column
    {
        NodeId,
        Name,
        Status,
        ColumnCount
    };

protected:
    void prepareAddBus(quint8 busId);
    void addBus(quint8 busId);
    void removeBus(quint8 busId);
    void updateBus(CanOpenBus *bus, quint8 column);

    void prepareAddNode(CanOpenBus *bus, quint8 nodeId);
    void addNode(CanOpenBus *bus, quint8 nodeId);
    void removeNode(CanOpenBus *bus, quint8 nodeId);
    void updateNode(Node *node, quint8 column);

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    CanOpen *_canOpen;
};

#endif  // BUSNODESMODEL_H
