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

#include "busnodesmanagerview.h"

#include <QLayout>

BusNodesManagerView::BusNodesManagerView(QWidget *parent)
    : BusNodesManagerView(nullptr, parent)
{
}

BusNodesManagerView::BusNodesManagerView(CanOpen *canOpen, QWidget *parent)
    : QWidget(parent),
      _canOpen(nullptr)
{
    createWidgets();
    setCanOpen(canOpen);

    connect(_busNodeTreeView, &BusNodesTreeView::busSelected, this, &BusNodesManagerView::busSelected);
    connect(_busNodeTreeView, &BusNodesTreeView::nodeSelected, this, &BusNodesManagerView::nodeSelected);
    connect(_busNodeTreeView, &BusNodesTreeView::busSelected, _busManagerWidget, &BusManagerWidget::setBus);
    connect(_busNodeTreeView, &BusNodesTreeView::nodeSelected, _nodeManagerWidget, &NodeManagerWidget::setNode);
}

CanOpen *BusNodesManagerView::canOpen() const
{
    return _canOpen;
}

void BusNodesManagerView::setCanOpen(CanOpen *canOpen)
{
    _canOpen = canOpen;

    _busNodeTreeView->setCanOpen(_canOpen);
    _busNodeTreeView->expandAll();
}

CanOpenBus *BusNodesManagerView::currentBus() const
{
    return _busNodeTreeView->currentBus();
}

Node *BusNodesManagerView::currentNode() const
{
    return _busNodeTreeView->currentNode();
}

void BusNodesManagerView::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setSpacing(0);
    layout->setContentsMargins(2, 2, 2, 2);

    _busNodeTreeView = new BusNodesTreeView();
    layout->addWidget(_busNodeTreeView);

    _busManagerWidget = new BusManagerWidget();
    _busNodeTreeView->addBusAction(_busManagerWidget->actionExplore());
    _busNodeTreeView->addBusAction(_busManagerWidget->actionSyncOne());
    _busNodeTreeView->addBusAction(_busManagerWidget->actionSyncStart());
    _busNodeTreeView->addBusAction(_busManagerWidget->actionTogleConnect());
    layout->addWidget(_busManagerWidget);

    _nodeManagerWidget = new NodeManagerWidget();
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionRemoveNode());
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionUpdateFirmware());
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionLoadEds());
    _busNodeTreeView->addNodeAction(_nodeManagerWidget->actionReLoadEds());
    layout->addWidget(_nodeManagerWidget);

    setLayout(layout);
}

NodeManagerWidget *BusNodesManagerView::nodeManagerWidget() const
{
    return _nodeManagerWidget;
}

void BusNodesManagerView::saveState(QSettings &settings)
{
    _busNodeTreeView->saveState(settings);
}

void BusNodesManagerView::restoreState(QSettings &settings)
{
    _busNodeTreeView->restoreState(settings);
}

BusManagerWidget *BusNodesManagerView::busManagerWidget() const
{
    return _busManagerWidget;
}

BusNodesTreeView *BusNodesManagerView::busNodeTreeView() const
{
    return _busNodeTreeView;
}
