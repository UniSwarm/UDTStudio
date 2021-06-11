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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QMainWindow>

#include "can/canSettingsDialog/cansettingsdialog.h"
#include "canopen.h"
#include "canopen/busnodesmanagerview.h"
#include "canopen/nodeod/nodeodwidget.h"
#include "canopenbus.h"
#include "parser/hexparser.h"

#include "model/deviceconfiguration.h"
#include "parser/edsparser.h"
#include "programdownload.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

signals:

public slots:

private:
    void createMenus();
    void createWidget();

    void openHexFile();
    void openEdsFile();

    void update();
    void updateProgram();
    void downloadState(QString state);

    void refreshInfo();
    void uploadEds();

    void nodeChanged(Node *currentNode);

    CanSettingsDialog *_connectDialog = nullptr;

    CanOpenBus *_bus;
    QCanBusDevice *_canDevice = nullptr;

    BusNodesTreeView *_busNodesManagerView;

    void connectDevice();
    void disconnectDevice();

    QLabel *_serialNumberLabel;
    QLabel *_vendorIdLabel;
    QLabel *_productCodeLabel;
    QLabel *_revisionNumberLabel;
    QGroupBox *_groupBoxInfo;
    QLabel *_fileNameEdsDataLabel;
    QLabel *_fileNameHexDataLabel;

    QPushButton *_updatePushButton;
    QPushButton *_uploadEdsPushButton;

    QLabel *_statusLabel;

    quint8 _iNodeListSelected;
    QList<ProgramDownload *> _programList;
};

#endif // MAINWINDOW_H
