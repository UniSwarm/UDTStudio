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

#include "nodescreenswidget.h"

#include "indexdb.h"
#include "nodescreenod.h"
#include "nodescreenpdo.h"
#include "nodescreensynchro.h"
#include "nodescreenumcmotor.h"

#include <QHBoxLayout>

NodeScreensWidget::NodeScreensWidget(QWidget *parent)
    : QWidget(parent)
{
    _activeNode = nullptr;
    createWidgets();
}

Node *NodeScreensWidget::activeNode() const
{
    return _activeNode;
}

void NodeScreensWidget::setActiveNode(Node *node)
{
    int currentIndex = _tabWidget->currentIndex();

    // remove all screens from QTabWidget
    while (_tabWidget->count() > 0)
    {
        _tabWidget->removeTab(0);
    }

    _activeNode = node;

    if (node)
    {
        addNode(node);

        // add all screens from nodeScreens to QTabWidget and set node
        NodeScreens nodeScreens = _nodesMap.value(_activeNode);
        for (NodeScreen *screen : nodeScreens.screens)
        {
            _tabWidget->addTab(screen, screen->icon(), " " + screen->title() + " ");
        }

        _tabWidget->setCurrentIndex(currentIndex);
    }
}

void NodeScreensWidget::addNode(Node *node)
{
    QMap<Node *, NodeScreens>::const_iterator nodeIt = _nodesMap.find(node);
    if (nodeIt != _nodesMap.constEnd())
    {
        return;
    }

    // add generic screens to the NodeScreensStruct
    NodeScreens nodeScreens;
    nodeScreens.node = node;

    NodeScreen *screen;

    screen = new NodeScreenOD();
    screen->setNode(node);
    nodeScreens.screens.append(screen);

    screen = new NodeScreenPDO();
    screen->setNode(node);
    nodeScreens.screens.append(screen);

    // add specific screens node
    if ((node->profileNumber()) == 0x192)
    {
        for (uint8_t i = 0; i < node->countProfile(); i++)
        {
            screen = new NodeScreenUmcMotor();
            screen->setNode(node, i);
            nodeScreens.screens.append(screen);
        }
        if (node->countProfile() != 0)
        {
            screen = new NodeScreenSynchro();
            screen->setNode(node, 12);
            nodeScreens.screens.append(screen);
        }
    }

    // add NodeScreensStruct to nodeIt
    _nodesMap.insert(node, nodeScreens);
}

void NodeScreensWidget::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    _tabWidget = new QTabWidget();
    layout->addWidget(_tabWidget);

    setLayout(layout);
}
