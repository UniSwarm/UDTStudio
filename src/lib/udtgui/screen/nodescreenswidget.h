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

#ifndef NODESCREENS_H
#define NODESCREENS_H

#include "udtgui_global.h"

#include <QWidget>

#include "nodescreen.h"

class QStackedWidget;
class QTabWidget;

class UDTGUI_EXPORT NodeScreensWidget : public QWidget
{
    Q_OBJECT
public:
    NodeScreensWidget(QWidget *parent = nullptr);

    Node *activeNode() const;

public slots:
    void setActiveNode(Node *node);
    void setActiveTab(int id);
    void setActiveTab(const QString &name);

protected:
    Node *_activeNode;
    void addNode(Node *node);

    void createWidgets();
    QStackedWidget *_stackedWidget;

    struct NodeScreens
    {
        Node *node;
        QTabWidget *tabWidget;
        QList<NodeScreen *> screens;
    };
    void addScreen(NodeScreens *nodeScreens, NodeScreen *screen);

    QMap<Node *, NodeScreens> _nodesMap;
};

#endif  // NODESCREENS_H
