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

#include "nodescreens.h"

#include "nodescreenod.h"
#include "nodescreenpdo.h"

#include <QHBoxLayout>

NodeScreens::NodeScreens(QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
    createWidgets();
}

Node *NodeScreens::node() const
{
    return _node;
}

void NodeScreens::addScreen(NodeScreen *screen)
{
    _tabWidget->addTab(screen, " " + screen->title() + " ");
    _screens.append(screen);
}

void NodeScreens::setNode(Node *node)
{
    _node = node;
    for (NodeScreen *screen : _screens)
    {
        screen->setNode(node);
    }
}

void NodeScreens::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    _tabWidget = new QTabWidget();
    layout->addWidget(_tabWidget);

    NodeScreen *screen;

    screen = new NodeScreenOD();
    addScreen(screen);

    screen = new NodeScreenPDO();
    addScreen(screen);

    setLayout(layout);
}
