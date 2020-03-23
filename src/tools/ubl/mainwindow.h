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
#include <QLabel>
#include <QTableView>
#include <QListView>
#include <QStringListModel>

#include "cansettingsdialog.h"
#include "canopen.h"
#include "hexfile.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

  signals:

  public slots:

  private:
    void createActions();
    void createMenus();
    void createWidget();
    void openFile();
    void exploreBus();
    void addEds();
    void update();
    void refreshOInfo();
    void refreshListNode();

    CanSettingsDialog *_connectDialog = nullptr;
    CanOpenBus *_bus;
    QCanBusDevice *_canDevice = nullptr;
    HexFile *hexFile;
    QString fileNameHex;
    QString fileNameEds;

    QWidget *widget;
    QMenu *_fileMenu;
    QMenu *_connectMenu;
    QAction *_quitAction;

    QAction *_connectAction;
    void connectDevice();

    QAction *_disconnectAction;
    void disconnectDevice();

    QAction *_canSettingsAction;

    QLabel *_deviceDataLabel;
    QLabel *_manuDeviceNameDataLabel;
    QLabel *_manufacturerHardwareVersionDataLabel;
    QLabel *_manufacturerSoftwareVersionDataLabel;

    QLabel *_fileNameDataLabel;
    QPushButton *_updatePushButton;
    QPushButton *_refreshPushButton;

    QListView *_tableView;
    QStringListModel *model;

};

#endif // MAINWINDOW_H
