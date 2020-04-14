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
#include <QSplitter>

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    setWindowIcon(QIcon(":/icons/img/UNIdevkit.ico"));
    _connectDialog = new CanSettingsDialog(nullptr, this);
    statusBar()->setVisible(true);

    createDocks();
    createWidgets();
    connect(_busNodesManagerView, &BusNodesManagerView::nodeSelected, _nodeOdWidget, &NodeOdWidget::setNode);
    connect(_busNodesManagerView, &BusNodesManagerView::nodeSelected, _nodePdoMappingWidget, &NodePDOMappingWidget::setNode);

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
    CanOpen::addBus(bus);
    connect(bus, &CanOpenBus::frameAvailable, _canFrameListView, &CanFrameListView::appendCanFrame);

    createMenus();
    resize(QApplication::screens()[0]->size() * 3 / 4);
}

MainWindow::~MainWindow()
{
    CanOpen::release();
    delete _connectDialog;
}

void MainWindow::createDocks()
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);

    _busNodesManagerDock = new QDockWidget(tr("Bus nodes manager"), this);
    _busNodesManagerDock->setObjectName("busNodesManagerDock");
    _busNodesManagerView = new BusNodesManagerView(CanOpen::instance());
    _busNodesManagerDock->setWidget(_busNodesManagerView);
    addDockWidget(Qt::LeftDockWidgetArea, _busNodesManagerDock);

    _canFrameListDock = new QDockWidget(tr("Can frames list view"), this);
    _canFrameListDock->setObjectName("canFrameListDock");
    _canFrameListView = new CanFrameListView();
    _canFrameListDock->setWidget(_canFrameListView);
    addDockWidget(Qt::LeftDockWidgetArea, _canFrameListDock);
    tabifyDockWidget(_busNodesManagerDock, _canFrameListDock);

    _dataLoggerDock = new QDockWidget(tr("Data logger"), this);
    _dataLoggerDock->setObjectName("dataLoggerDock");
    _dataLoggerWidget = new DataLoggerWidget();
    _dataLoggerDock->setWidget(_dataLoggerWidget);
    addDockWidget(Qt::BottomDockWidgetArea, _dataLoggerDock);

    _busNodesManagerDock->raise();
}

void MainWindow::createWidgets()
{
    QSplitter *splitter = new QSplitter();

    _nodeOdWidget = new NodeOdWidget();
    splitter->addWidget(_nodeOdWidget);

    _nodePdoMappingWidget = new NodePDOMappingWidget();
    splitter->addWidget(_nodePdoMappingWidget);

    setCentralWidget(splitter);
}

void MainWindow::createMenus()
{
    // ============= file =============
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *exitAction = new QAction(tr("E&xit"), this);
    exitAction->setStatusTip(tr("Exits UDTStudio"));
    exitAction->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(exitAction);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    // ============= Bus =============
    QMenu *busMenu = menuBar()->addMenu(tr("&Bus"));

    // ============= View =============
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));
    viewMenu->addAction(_busNodesManagerDock->toggleViewAction());
    viewMenu->addAction(_canFrameListDock->toggleViewAction());
    viewMenu->addAction(_dataLoggerDock->toggleViewAction());
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
            CanOpen::addBus(bus);
            connect(bus, &CanOpenBus::frameAvailable, _canFrameListView, &CanFrameListView::appendCanFrame);
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

bool MainWindow::event(QEvent *event)
{
    if (event->type()==QEvent::Close)
    {
        QApplication::quit();
    }
    return QMainWindow::event(event);
}
