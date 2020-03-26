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

#include "busnodestreeview.h"

#include <QDebug>

BusNodesTreeView::BusNodesTreeView(QWidget *parent)
    : BusNodesTreeView(nullptr, parent)
{
}

BusNodesTreeView::BusNodesTreeView(CanOpen *canOpen, QWidget *parent)
    : QTreeView(parent)
{
    _busNodesModel = new BusNodesModel();
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setCanOpen(canOpen);
    setModel(_busNodesModel);
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &BusNodesTreeView::updateSelection);
}

BusNodesTreeView::~BusNodesTreeView()
{
}

CanOpen *BusNodesTreeView::canOpen() const
{
    return _busNodesModel->canOpen();
}

void BusNodesTreeView::setCanOpen(CanOpen *canOpen)
{
    _busNodesModel->setCanOpen(canOpen);
    if (canOpen)
    {
        connect(canOpen, &CanOpen::busChanged, this, &BusNodesTreeView::refresh);
    }
}

CanOpenBus *BusNodesTreeView::currentBus() const
{
    return _busNodesModel->bus(selectionModel()->currentIndex());
}

Node *BusNodesTreeView::currentNode() const
{
    return _busNodesModel->node(selectionModel()->currentIndex());
}

void BusNodesTreeView::refresh()
{
    // TODO add a real insert node/bus system
    _busNodesModel->setCanOpen(_busNodesModel->canOpen());
    if (_busNodesModel->canOpen())
    {
        foreach (CanOpenBus *bus, _busNodesModel->canOpen()->buses())
        {
            connect(bus, &CanOpenBus::nodeAdded, this, &BusNodesTreeView::refresh);
        }
    }
    expandAll();
}

void BusNodesTreeView::updateSelection()
{
    emit busSelected(currentBus());
    emit nodeSelected(currentNode());
}
