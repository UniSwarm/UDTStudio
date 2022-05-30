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

#include <QApplication>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QSettings>

#include "utils/headerview.h"

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

    createHeader();
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
    if (_busNodesModel->canOpen() != nullptr)
    {
        disconnect(_busNodesModel->canOpen(), nullptr, this, nullptr);
    }

    _busNodesModel->setCanOpen(canOpen);

    if (canOpen != nullptr)
    {
        connect(canOpen, &CanOpen::busAdded, this, &BusNodesTreeView::addBus);
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

void BusNodesTreeView::addBusAction(QAction *action)
{
    _busActions.append(action);
}

void BusNodesTreeView::addNodeAction(QAction *action)
{
    _nodeActions.append(action);
}

void BusNodesTreeView::saveState(QSettings &settings)
{
    settings.setValue("header", header()->saveState());
}

void BusNodesTreeView::restoreState(QSettings &settings)
{
    header()->restoreState(settings.value("header").toByteArray());
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
    if (bus != nullptr)
    {
        if (!bus->isConnected())
        {
            bus->canBusDriver()->connectDevice();
        }
        if (bus->isConnected() && bus->nodes().isEmpty())
        {
            bus->exploreBus();
        }
    }
}

void BusNodesTreeView::addBus(quint8 busId)
{
    CanOpenBus *bus = CanOpen::bus(busId);
    if (bus == nullptr)
    {
        return;
    }
    connect(bus,
            &CanOpenBus::nodeAdded,
            this,
            [=](int nodeId)
            {
                addNode(bus, nodeId);
            });
}

void BusNodesTreeView::addNode(CanOpenBus *bus, quint8 nodeId)
{
    Q_UNUSED(nodeId);
    if (bus->nodes().count() == 1)  // first node added to this bus
    {
        for (int row = 0; row < _sortFilterProxyModel->rowCount(); row++)
        {
            QModelIndex index = _sortFilterProxyModel->index(row, BusNodesModel::NodeId);
            if (_sortFilterProxyModel->data(index) == QVariant(bus->busId()))
            {
                expand(index);
            }
        }
    }
}

void BusNodesTreeView::createHeader()
{
    HeaderView *headerView = new HeaderView(Qt::Horizontal, this);
    setHeader(headerView);
    headerView->addMandatorySection(BusNodesModel::NodeId);
    headerView->addMandatorySection(BusNodesModel::Name);
    headerView->setStretchLastSection(true);
#if QT_VERSION >= 0x050B00
    int w0 = QFontMetrics(font()).horizontalAdvance("0");
#else
    int w0 = QFontMetrics(font()).width("0");
#endif
    headerView->resizeSection(BusNodesModel::NodeId, 12 * w0);
    headerView->resizeSection(BusNodesModel::Name, 14 * w0);
    headerView->resizeSection(BusNodesModel::Status, 8 * w0);

    setSortingEnabled(true);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    sortByColumn(0, Qt::AscendingOrder);
}

void BusNodesTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    QList<CanOpenBus *> selectedBuses;
    QList<Node *> selectedNodes;

    QModelIndexList selectedRows = selectionModel()->selectedRows();
    for (QModelIndex row : qAsConst(selectedRows))
    {
        const QModelIndex &curentIndex = _sortFilterProxyModel->mapToSource(row);

        Node *node = _busNodesModel->node(curentIndex);
        if (node != nullptr)
        {
            selectedNodes.append(node);
            continue;
        }

        CanOpenBus *bus = _busNodesModel->bus(curentIndex);
        if (bus != nullptr)
        {
            selectedBuses.append(bus);
            continue;
        }
    }

    QMenu menu;
    if (!selectedBuses.isEmpty())
    {
        menu.addActions(_busActions);
        if (!selectedNodes.isEmpty())
        {
            menu.addSeparator();
        }
    }
    if (!selectedNodes.isEmpty())
    {
        menu.addActions(_nodeActions);
    }
    menu.exec(event->globalPos());
}
