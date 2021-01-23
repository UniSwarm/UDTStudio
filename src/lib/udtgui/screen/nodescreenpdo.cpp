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

#include "nodescreenpdo.h"

#include <QDebug>
#include <QLayout>
#include <QSplitter>

NodeScreenPDO::NodeScreenPDO(QWidget *parent)
    : NodeScreen(parent)
{
    createWidgets();
}

void NodeScreenPDO::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    QSplitter *splitter = new QSplitter();
    layout->addWidget(splitter);

    _nodeOdWidget = new NodeOdWidget();
    _nodeOdWidget->setFilter(NodeOdWidget::FilterPDO);
    splitter->addWidget(_nodeOdWidget);

    _nodePdoMappingWidget = new NodePDOMappingWidget();
    splitter->addWidget(_nodePdoMappingWidget);

    setLayout(layout);
}

QString NodeScreenPDO::title() const
{
    return QString(tr("PDO"));
}

void NodeScreenPDO::setNodeInternal(Node *node, uint8_t axis)
{
    _nodeOdWidget->setNode(node);
    _nodePdoMappingWidget->setNode(node);
}
