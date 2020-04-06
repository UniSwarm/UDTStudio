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

#include "mainwindow.h"

#include "model/deviceconfiguration.h"
#include "parser/edsparser.h"

#include <QApplication>
#include <QCanBus>
#include <QDateTime>
#include <QScreen>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QAction>
#include <QDockWidget>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowIcon(QIcon(":/icons/img/UNIdevkit.ico"));
    _connectDialog = new CanSettingsDialog(nullptr, this);

    _canOpen = new CanOpen();

    createDocks();
    createWidgets();
    connect(_busNodesManagerView, &BusNodesManagerView::nodeSelected, _nodeOdWidget, &NodeOdWidget::setNode);

    CanOpenBus *bus;
    if (QCanBus::instance()->plugins().contains("socketcan"))
    {
        bus = new CanOpenBus(QCanBus::instance()->createDevice("socketcan", "can0"));
    }
    else
    {
        bus = new CanOpenBus(QCanBus::instance()->createDevice("virtualcan", "can0"));
    }
    bus->setBusName("Bus 1");
    _canOpen->addBus(bus);
    connect(bus, &CanOpenBus::frameAvailable, _canFrameListView, &CanFrameListView::appendCanFrame);

    createActions();
    createMenus();
    resize(QApplication::screens()[0]->size() / 2);
}

MainWindow::~MainWindow()
{
    delete _canOpen;
    delete _connectDialog;
}

void MainWindow::createDocks()
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);

    _busNodesManagerDock = new QDockWidget(tr("Bus nodes manager"), this);
    _busNodesManagerDock->setObjectName("fileProjectDock");
    _busNodesManagerView = new BusNodesManagerView(_canOpen);
    _busNodesManagerDock->setWidget(_busNodesManagerView);
    addDockWidget(Qt::LeftDockWidgetArea, _busNodesManagerDock);
}

void MainWindow::createWidgets()
{
    QWidget *widget = new QWidget();
    QLayout *layout = new QHBoxLayout();

    _nodeOdWidget = new NodeOdWidget();
    layout->addWidget(_nodeOdWidget);

    _canFrameListView = new CanFrameListView();
    layout->addWidget(_canFrameListView);

    widget->setLayout(layout);
    setCentralWidget(widget);
}

void MainWindow::createActions()
{
    _quitAction = new QAction(tr("&Quit"), this);
    connect(_quitAction, &QAction::triggered, this, &MainWindow::close);

    _connectAction = new QAction(tr("&Connect"), this);
    connect(_connectAction, &QAction::triggered, this, &MainWindow::connectDevice);

    _disconnectAction = new QAction(tr("&Disconnect"), this);
    connect(_disconnectAction, &QAction::triggered, this, &MainWindow::disconnectDevice);

    _canSettingsAction = new QAction(tr("&Can Settings"), this);
    connect(_canSettingsAction, &QAction::triggered, _connectDialog, &CanSettingsDialog::show);
    connect(_connectDialog, &QDialog::accepted, this, &MainWindow::connectDevice);

    _connectAction->setEnabled(true);
    _disconnectAction->setEnabled(false);
}

void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(_quitAction);
    _fileMenu->addSeparator();

    _connectMenu = menuBar()->addMenu(tr("&Connection"));
    _connectMenu->addAction(_connectAction);
    _connectMenu->addAction(_disconnectAction);
    _connectMenu->addAction(_canSettingsAction);
    _connectMenu->addSeparator();

    _connectMenu = menuBar()->addMenu(tr("&Bus"));
    _connectMenu->addSeparator();
}

void MainWindow::connectDevice()
{
    QString errorString;
    const CanSettingsDialog::Settings settings = _connectDialog->settings();

    _canDevice = _connectDialog->device();
    if (!_canDevice)
    {
        _connectDialog->show();
    }
    else
    {
        if (_canDevice->state() == QCanBusDevice::UnconnectedState)
        {
            if (!_canDevice->connectDevice())
            {
                statusBar()->showMessage(tr("Connection error: %1").arg(_canDevice->errorString()));
            }
            else
            {
                const QVariant bitRate = _canDevice->configurationParameter(QCanBusDevice::BitRateKey);
                if (bitRate.isValid())
                {
                    statusBar()->showMessage(tr("%1 - %2 at %3 kBit/s").arg(settings.interfaceName).arg(settings.deviceName).arg(bitRate.toInt() / 1000));
                }
                else
                {
                    statusBar()->showMessage(tr("%1 - %2").arg(settings.interfaceName).arg(settings.deviceName));
                }

                _connectAction->setEnabled(false);
                _disconnectAction->setEnabled(true);
            }

            CanOpenBus *bus = new CanOpenBus(_canDevice);
            bus->setBusName(settings.interfaceName + ":" + settings.deviceName);
            statusBar()->showMessage(tr("%1 - %2").arg(settings.interfaceName).arg(settings.deviceName));
            _canOpen->addBus(bus);
            connect(bus, &CanOpenBus::frameAvailable, _canFrameListView, &CanFrameListView::appendCanFrame);
            //connect(bus, &CanOpenBus::nodeAdded, _busNodeTreeView, &BusNodesTreeView::refresh);
        }
    }
}

void MainWindow::disconnectDevice()
{
    if (!_canDevice)
    {
        return;
    }

    _canDevice->disconnectDevice();
    _connectAction->setEnabled(true);
    _disconnectAction->setEnabled(false);
    statusBar()->showMessage(tr("Disconnected"));
}
