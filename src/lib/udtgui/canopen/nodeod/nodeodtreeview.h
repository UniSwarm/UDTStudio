/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#ifndef NODEODTREEVIEW_H
#define NODEODTREEVIEW_H

#include "../../udtgui_global.h"

#include <QTreeView>

#include "nodeodfilterproxymodel.h"
#include "nodeoditemdelegate.h"
#include "nodeoditemmodel.h"

#include <QAction>

class UDTGUI_EXPORT NodeOdTreeView : public QTreeView
{
    Q_OBJECT
public:
    NodeOdTreeView(QWidget *parent = nullptr);
    ~NodeOdTreeView() override;

    Node *node() const;
    void setNode(Node *node);

    QList<NodeObjectId> selectedNodeObjectIds() const;
    void selectNodeObjectId(const NodeObjectId &objId);

public slots:
    void setFilter(const QString &filterText);
    void readSelected();
    void readAll();
    void copy();

protected slots:
    void updateSelect(const QItemSelection &selected, const QItemSelection &deselected);

protected:
    NodeOdItemModel *_odModel;
    NodeOdFilterProxyModel *_odModelSorter;
    NodeOdItemDelegate *_delegate;

    void createActions();
    QAction *_readAction;
    QAction *_readAllAction;
    QAction *_copyAction;
    QAction *_expandAllAction;
    QAction *_collapseAllAction;

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif  // NODEODTREEVIEW_H
