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

#include "nodescreenod.h"

#include <QHBoxLayout>

NodeScreenOD::NodeScreenOD()
{
    createWidgets();
}

void NodeScreenOD::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);

    _nodeOdWidget = new NodeOdWidget();
    layout->addWidget(_nodeOdWidget);

    setLayout(layout);
}

QString NodeScreenOD::title() const
{
    return QString(tr("OD"));
}

void NodeScreenOD::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(axis)
    _nodeOdWidget->setNode(node);
}
