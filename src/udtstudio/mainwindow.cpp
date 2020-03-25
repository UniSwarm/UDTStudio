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

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    _connectDialog = new CanSettingsDialog(nullptr, this);

    /*_canOpen = new CanOpen();
    if (QCanBus::instance()->plugins().contains("socketcan"))
    {
        _bus = new CanOpenBus(QCanBus::instance()->createDevice("socketcan", "can0"));
    }
    else
    {
        _bus = new CanOpenBus(QCanBus::instance()->createDevice("virtualcan", "can0"));
    }
    _bus->setBusName("Bus 1");
    _canOpen->addBus(_bus);*/

    _canOpen = new CanOpen();

    QWidget *widget = new QWidget();
    QLayout *layout = new QHBoxLayout();

    _busNodeTreeView = new BusNodesTreeView();
    _busNodeTreeView->setCanOpen(_canOpen);

    layout->addWidget(_busNodeTreeView);
    _busNodeTreeView->expandAll();

    _canFrameListView = new CanFrameListView();

    layout->addWidget(_canFrameListView);
    widget->setLayout(layout);
    setCentralWidget(widget);

    createActions();
    createMenus();
    resize(QApplication::screens()[0]->size() / 2);
}

MainWindow::~MainWindow()
{
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

    _exploreBusAction = new QAction(tr("&Explore"), this);
    connect(_exploreBusAction, &QAction::triggered, this, &MainWindow::exploreBus);
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
    _connectMenu->addAction(_exploreBusAction);
    _connectMenu->addSeparator();
}

void MainWindow::exploreBus()
{
    CanOpenBus *bus = _busNodeTreeView->currentBus();
    if (!bus)
    {
        statusBar()->showMessage(tr("No interface"));
        return;
    }
    _bus->exploreBus();
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

            _bus = new CanOpenBus(_canDevice);
            _bus->setBusName(settings.interfaceName + "_" + settings.deviceName);
            _canOpen->addBus(_bus);
            connect(_bus, &CanOpenBus::nodeAdded, _busNodeTreeView, &BusNodesTreeView::refresh);
            //connect(_bus, &CanOpenBus::nodeAdded, this, &MainWindow::refreshListNode);
            statusBar()->showMessage(tr("%1 - %2").arg(settings.interfaceName).arg(settings.deviceName));
            connect(_bus, &CanOpenBus::frameAvailable, _canFrameListView, &CanFrameListView::appendCanFrame);
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
