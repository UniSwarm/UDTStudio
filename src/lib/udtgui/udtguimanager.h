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

#ifndef UDTGUIMANAGER_H
#define UDTGUIMANAGER_H

#include "udtgui_global.h"

#include "nodeobjectid.h"

#include <QMap>

class QWidget;

class Node;
class NodeOdItemModel;
class NodeOdTreeView;

class UDTGUI_EXPORT UdtGuiManager
{
    Q_DISABLE_COPY(UdtGuiManager)

public:
    // NodeOd
    static NodeOdItemModel *nodeOdItemModel(Node *node);
    static NodeOdTreeView *nodeOdTreeView(Node *node);
    static void locateInOdTreeView(const NodeObjectId &objId);
    static void setNodeOdTreeView(Node *node, NodeOdTreeView *nodeOdTreeView);

    static void showWidgetRecursive(QWidget *widget);

protected:
    static UdtGuiManager *instance();
    UdtGuiManager();

    static UdtGuiManager *_instance;
    QMap <Node *, NodeOdItemModel *> _nodeOdItemModels;
    QMap <Node *, NodeOdTreeView *> _nodeOdTreeViews;
};

#endif // UDTGUIMANAGER_H
