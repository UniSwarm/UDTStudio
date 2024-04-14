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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QSortFilterProxyModel>

#include "canopenbus.h"

#include "can/canFrameListView/canframelistview.h"
#include "canopen/busnodesmanagerview.h"

#include "canopen/datalogger/dataloggerwidget.h"

#include "screen/nodescreenswidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

public slots:
    void exportCfgFile();
    void exportDCF();
    void about();

protected:
    // CanSettingsDialog *_connectDialog;

    void createDocks();
    QDockWidget *_busNodesManagerDock;
    BusNodesManagerView *_busNodesManagerView;
    QDockWidget *_canFrameListDock;
    CanFrameListView *_canFrameListView;
    QDockWidget *_dataLoggerDock;
    DataLoggerWidget *_dataLoggerWidget;

    void createWidgets();
    NodeScreensWidget *_nodeScreens;

    // actions / menu
    void createMenus();

    void writeSettings();
    void readSettings();

    // QObject interface
public:
    bool event(QEvent *event) override;
};

#endif  // MAINWINDOW_H
