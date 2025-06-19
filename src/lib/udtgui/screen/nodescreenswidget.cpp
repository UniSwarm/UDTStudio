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

#include "nodescreenswidget.h"

#include "nodescreenerror.h"
#include "nodescreenhome.h"
#include "nodescreennmt.h"
#include "nodescreenod.h"
#include "nodescreenpdo.h"

#include "nodescreenadamp.h"
#include "nodescreensynchro.h"
#include "nodescreenuio.h"
#include "nodescreenuioled.h"
#include "nodescreenumcmotor.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QStackedWidget>
#include <QTabWidget>

NodeScreensWidget::NodeScreensWidget(QWidget *parent)
    : QWidget(parent)
{
    _activeNode = nullptr;

    createWidgets();
    setStyleSheet(QStringLiteral("QTabWidget::tab-bar {left: 5px;}"));
}

Node *NodeScreensWidget::activeNode() const
{
    return _activeNode;
}

void NodeScreensWidget::setActiveNode(Node *node)
{
    int currentIndex = 0;
    QTabWidget *tabWidget = dynamic_cast<QTabWidget *>(_stackedWidget->currentWidget());
    if (tabWidget != nullptr)
    {
        currentIndex = tabWidget->currentIndex();
    }

    _activeNode = node;

    if (node != nullptr)
    {
        addNode(node);

        NodeScreens nodeScreens = _nodesMap.value(_activeNode);
        _stackedWidget->setCurrentWidget(nodeScreens.tabWidget);
        QTabWidget *tabWidget = dynamic_cast<QTabWidget *>(_stackedWidget->currentWidget());
        tabWidget->setCurrentIndex(currentIndex);
    }
}

void NodeScreensWidget::setActiveTab(int id)
{
    QTabWidget *tabWidget = dynamic_cast<QTabWidget *>(_stackedWidget->currentWidget());
    if (tabWidget == nullptr)
    {
        return;
    }
    tabWidget->setCurrentIndex(id);
}

void NodeScreensWidget::setActiveTab(const QString &name)
{
    QTabWidget *tabWidget = dynamic_cast<QTabWidget *>(_stackedWidget->currentWidget());
    if (tabWidget == nullptr)
    {
        return;
    }

    for (int tabIndex = 0; tabIndex < tabWidget->count(); tabIndex++)
    {
        const QString &tabName = tabWidget->tabText(tabIndex).trimmed();
        if (tabName.compare(name, Qt::CaseInsensitive) == 0)
        {
            tabWidget->setCurrentIndex(tabIndex);
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

    QTabWidget *tabWidget = new QTabWidget(_stackedWidget);
    nodeScreens.tabWidget = tabWidget;

    NodeScreen *screen;

    screen = new NodeScreenHome(tabWidget);
    screen->setNode(node);
    addScreen(&nodeScreens, screen);

    screen = new NodeScreenOD(tabWidget);
    screen->setNode(node);
    addScreen(&nodeScreens, screen);

    screen = new NodeScreenPDO(tabWidget);
    screen->setNode(node);
    addScreen(&nodeScreens, screen);

    screen = new NodeScreenNMT(tabWidget);
    screen->setNode(node);
    addScreen(&nodeScreens, screen);

    screen = new NodeScreenError(tabWidget);
    screen->setNode(node);
    addScreen(&nodeScreens, screen);

    // add specific screens node
    switch (node->profileNumber())
    {
        case 401:  // UIO, P401
        {
            QApplication::processEvents();

            screen = new NodeScreenUio(tabWidget);
            screen->setNode(node);
            addScreen(&nodeScreens, screen);
            break;
        }
        case 402:  // UMC, P402
        {
            for (int i = 0; i < node->profilesCount(); i++)
            {
                QApplication::processEvents();

                screen = new NodeScreenUmcMotor(tabWidget);
                screen->setNode(node, i);
                addScreen(&nodeScreens, screen);
            }
            if (node->profilesCount() > 1)
            {
                screen = new NodeScreenSynchro(tabWidget);
                screen->setNode(node, 12);
                addScreen(&nodeScreens, screen);
            }
            break;
        }
        case 428:  // UIOled, P428
        {
            QApplication::processEvents();

            screen = new NodeScreenUIOLed(tabWidget);
            screen->setNode(node);
            addScreen(&nodeScreens, screen);
            break;
        }
        case 199:  // ADAMP
        {
            QApplication::processEvents();

            screen = new NodeScreenAdamp(tabWidget);
            screen->setNode(node);
            addScreen(&nodeScreens, screen);
            break;
        }
    }
    _stackedWidget->addWidget(tabWidget);

    // add NodeScreensStruct to nodeIt
    _nodesMap.insert(node, nodeScreens);
}

void NodeScreensWidget::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);

    _stackedWidget = new QStackedWidget();
    layout->addWidget(_stackedWidget);

    setLayout(layout);
}

void NodeScreensWidget::addScreen(NodeScreens *nodeScreens, NodeScreen *screen)
{
    screen->setScreenWidget(this);
    nodeScreens->screens.append(screen);
    nodeScreens->tabWidget->addTab(screen, screen->icon(), " " + screen->title() + " ");
}
