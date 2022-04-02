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
#include "nodescreenhome.h"
#include "nodescreenod.h"
#include "nodescreenpdo.h"
#include "nodescreensynchro.h"
#include "nodescreenuio.h"
#include "nodescreenuioled.h"
#include "nodescreenumcmotor.h"

#include <QApplication>
#include <QHBoxLayout>

NodeScreensWidget::NodeScreensWidget(QWidget *parent)
    : QWidget(parent)
{
    _activeNode = nullptr;

    createWidgets();
    setStyleSheet("QTabWidget::tab-bar {left: 5px;}");
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

    if (node != nullptr)
    {
        addNode(node);

        // add all screens from nodeScreens to QTabWidget and set node
        NodeScreens nodeScreens = _nodesMap.value(_activeNode);
        for (NodeScreen *screen : qAsConst(nodeScreens.screens))
        {
            _tabWidget->addTab(screen, screen->icon(), " " + screen->title() + " ");
        }

        _tabWidget->setCurrentIndex(currentIndex);
    }
}

void NodeScreensWidget::setActiveTab(int id)
{
    _tabWidget->setCurrentIndex(id);
}

void NodeScreensWidget::setActiveTab(const QString &name)
{
    for (int tabIndex = 0; tabIndex < _tabWidget->count(); tabIndex++)
    {
        const QString &tabName = _tabWidget->tabText(tabIndex).trimmed();
        if (tabName.compare(name, Qt::CaseInsensitive) == 0)
        {
            _tabWidget->setCurrentIndex(tabIndex);
            return;
        }
    }
}

void NodeScreensWidget::addNode(Node *node)
{
    QMap<Node *, NodeScreens>::const_iterator nodeIt = _nodesMap.constFind(node);
    if (nodeIt != _nodesMap.constEnd())
    {
        return;
    }

    // add generic screens to the NodeScreensStruct
    NodeScreens nodeScreens;
    nodeScreens.node = node;

    NodeScreen *screen;

    screen = new NodeScreenHome();
    screen->setNode(node);
    screen->setScreenWidget(this);
    nodeScreens.screens.append(screen);

    QApplication::processEvents();
    screen = new NodeScreenOD();
    screen->setNode(node);
    screen->setScreenWidget(this);
    nodeScreens.screens.append(screen);

    QApplication::processEvents();
    screen = new NodeScreenPDO();
    screen->setNode(node);
    screen->setScreenWidget(this);
    nodeScreens.screens.append(screen);

    // add specific screens node
    switch (node->profileNumber())
    {
        case 401:  // UIO, P401
        {
            QApplication::processEvents();
            screen = new NodeScreenUio();
            screen->setNode(node);
            screen->setScreenWidget(this);
            nodeScreens.screens.append(screen);
            break;
        }
        case 402:  // UMC, P402
        {
            for (int i = 0; i < node->profilesCount(); i++)
            {
                QApplication::processEvents();
                screen = new NodeScreenUmcMotor();
                screen->setNode(node, i);
                screen->setScreenWidget(this);
                nodeScreens.screens.append(screen);
            }
            if (node->profilesCount() > 1)
            {
                QApplication::processEvents();
                screen = new NodeScreenSynchro();
                screen->setNode(node, 12);
                screen->setScreenWidget(this);
                nodeScreens.screens.append(screen);
            }
            break;
        }
        case 428:  // UIOled, P428
        {
            QApplication::processEvents();
            screen = new NodeScreenUIOLed();
            screen->setNode(node);
            screen->setScreenWidget(this);
            nodeScreens.screens.append(screen);
            break;
        }
    }

    // add NodeScreensStruct to nodeIt
    _nodesMap.insert(node, nodeScreens);
}

void NodeScreensWidget::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);

    _tabWidget = new QTabWidget();
    layout->addWidget(_tabWidget);

    setLayout(layout);
}
