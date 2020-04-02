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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QTreeView>
#include <QSortFilterProxyModel>

#include "od/odtreeview.h"
#include "canopen/nodeod/nodeodtreeview.h"
#include "canopenbus.h"

#include "can/canFrameListView/canframelistview.h"
#include "canopen/busnodesmanagerview.h"
#include "can/canSettingsDialog/cansettingsdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    CanSettingsDialog *_connectDialog;

    CanOpen *_canOpen;
    CanOpenBus *_bus;
    QCanBusDevice *_canDevice;

    void createDocks();
    QDockWidget *_busNodesManagerDock;
    BusNodesManagerView *_busNodesManagerView;

    void createWidgets();
    CanFrameListView *_canFrameListView;
    ODTreeView *_odView;
    NodeOdTreeView *_nodeOdTreeView;

    void createActions();
    void createMenus();
    QWidget *widget;
    QMenu *_fileMenu;
    QMenu *_connectMenu;
    QAction *_quitAction;

    QAction *_connectAction;
    void connectDevice();

    QAction *_disconnectAction;
    void disconnectDevice();

    QAction *_canSettingsAction;
};

#endif // MAINWINDOW_H
