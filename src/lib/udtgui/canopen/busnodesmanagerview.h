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

#ifndef BUSNODESMANAGERVIEW_H
#define BUSNODESMANAGERVIEW_H

#include "../udtgui_global.h"

#include <QWidget>

#include "busnodestreeview.h"
#include "busmanagerwidget.h"
#include "nodemanagerwidget.h"

class UDTGUI_EXPORT BusNodesManagerView : public QWidget
{
    Q_OBJECT
public:
    BusNodesManagerView(QWidget *parent = nullptr);
    BusNodesManagerView(CanOpen *canOpen, QWidget *parent = nullptr);

    CanOpen *canOpen() const;
    void setCanOpen(CanOpen *canOpen);

signals:
    void busSelected(CanOpenBus *currentBus);
    void nodeSelected(Node *currentNode);

public slots:

protected:
    void createWidgets();
    BusNodesTreeView *_busNodeTreeView;
    BusManagerWidget *_busManagerWidget;
    NodeManagerWidget *_nodeManagerWidget;

    CanOpen *_canOpen;
};

#endif // BUSNODESMANAGERVIEW_H
