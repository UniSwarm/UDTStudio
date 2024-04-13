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

#include "nodewidget.h"

#include "node.h"

NodeWidget::NodeWidget(QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
}

Node *NodeWidget::node() const
{
    return _node;
}

void NodeWidget::setNode(Node *node)
{
    _node = node;

    if (node != nullptr)
    {
        connect(node, &Node::statusChanged, this, &NodeWidget::updateObjects);
    }
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->setNode(node);
    }
}

void NodeWidget::readAll()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->readObject();
    }
}

void NodeWidget::readDynamics()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_dynamicIndexWidgets))
    {
        indexWidget->readObject();
    }
}

void NodeWidget::addIndexWidget(AbstractIndexWidget *indexWidget)
{
    _indexWidgets.append(indexWidget);
}

void NodeWidget::adddynamicIndexWidget(AbstractIndexWidget *indexWidget)
{
    _indexWidgets.append(indexWidget);
    _dynamicIndexWidgets.append(indexWidget);
}

void NodeWidget::updateObjects()
{
    if (_node == nullptr || _indexWidgets.isEmpty())
    {
        return;
    }

    NodeSubIndex *subIndex = _node->nodeOd()->subIndex(_indexWidgets.first()->objId());
    if (subIndex != nullptr)
    {
        if (subIndex->lastModification().isNull() && (_node->status() == Node::Status::PREOP || _node->status() == Node::Status::STARTED) && isVisible())
        {
            QTimer::singleShot(100, this, &NodeWidget::readAll);
        }
    }
}

void NodeWidget::showEvent(QShowEvent *event)
{
    Q_UNUSED(event);
    updateObjects();
}
