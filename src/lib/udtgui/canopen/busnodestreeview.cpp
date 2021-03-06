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

#include "busnodestreeview.h"

#include <QDebug>
#include <QHeaderView>

BusNodesTreeView::BusNodesTreeView(QWidget *parent)
    : BusNodesTreeView(nullptr, parent)
{
}

BusNodesTreeView::BusNodesTreeView(CanOpen *canOpen, QWidget *parent)
    : QTreeView(parent)
{
    _busNodesModel = new BusNodesModel(this);

    _sortFilterProxyModel = new QSortFilterProxyModel(this);
    _sortFilterProxyModel->setSourceModel(_busNodesModel);
    setModel(_sortFilterProxyModel);

    setCanOpen(canOpen);
    setAnimated(true);
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &BusNodesTreeView::updateSelection);
    connect(this, &QAbstractItemView::doubleClicked, this, &BusNodesTreeView::indexDbClick);

    int w0 = QFontMetrics(font()).horizontalAdvance("0");
    header()->resizeSection(BusNodesModel::NodeId, 12 * w0);
    header()->resizeSection(BusNodesModel::Name, 14 * w0);
    header()->resizeSection(BusNodesModel::Status, 8 * w0);

    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    sortByColumn(0, Qt::AscendingOrder);
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
    CanOpen *oldCanOpen = _busNodesModel->canOpen();
    if (canOpen != oldCanOpen)
    {
        if (oldCanOpen)
        {
            disconnect(oldCanOpen, &CanOpen::busChanged, this, &BusNodesTreeView::refresh);
        }
    }
    _busNodesModel->setCanOpen(canOpen);
    if (canOpen)
    {
        connect(canOpen, &CanOpen::busChanged, this, &BusNodesTreeView::refresh);
    }
}

CanOpenBus *BusNodesTreeView::currentBus() const
{
    return _busNodesModel->bus(_sortFilterProxyModel->mapToSource(selectionModel()->currentIndex()));
}

Node *BusNodesTreeView::currentNode() const
{
    return _busNodesModel->node(_sortFilterProxyModel->mapToSource(selectionModel()->currentIndex()));
}

void BusNodesTreeView::refresh()
{
    // TODO add a real insert node/bus system
    _busNodesModel->setCanOpen(_busNodesModel->canOpen());
    if (_busNodesModel->canOpen())
    {
        for (CanOpenBus *bus : _busNodesModel->canOpen()->buses())
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

void BusNodesTreeView::indexDbClick(const QModelIndex &index)
{
    QModelIndex indexBusNodeModel = _sortFilterProxyModel->mapToSource(index);

    CanOpenBus *bus = _busNodesModel->bus(indexBusNodeModel);
    if (bus)
    {
        if (bus->isConnected() && bus->nodes().isEmpty())
        {
            bus->exploreBus();
        }
    }
}
