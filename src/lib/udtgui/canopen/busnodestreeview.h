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

#ifndef BUSNODESTREEVIEW_H
#define BUSNODESTREEVIEW_H

#include "udtgui_global.h"

#include <QTreeView>

#include "busnodesmodel.h"

#include <QSortFilterProxyModel>
#include <QList>
#include <QAction>

class UDTGUI_EXPORT BusNodesTreeView : public QTreeView
{
    Q_OBJECT
public:
    BusNodesTreeView(QWidget *parent = nullptr);
    BusNodesTreeView(CanOpen *canOpen, QWidget *parent = nullptr);
    ~BusNodesTreeView();

    CanOpen *canOpen() const;
    void setCanOpen(CanOpen *canOpen);

    CanOpenBus *currentBus() const;
    Node *currentNode() const;

    void addBusAction(QAction *action);
    void addNodeAction(QAction *action);

signals:
    void busSelected(CanOpenBus *currentBus);
    void nodeSelected(Node *currentNode);

protected slots:
    void updateSelection();
    void indexDbClick(const QModelIndex &index);

    void addBus(quint8 busId);
    void addNode(CanOpenBus *bus, quint8 nodeId);

protected:
    BusNodesModel *_busNodesModel;
    QSortFilterProxyModel *_sortFilterProxyModel;

    QList<QAction *> _busActions;
    QList<QAction *> _nodeActions;

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // BUSNODESTREEVIEW_H
