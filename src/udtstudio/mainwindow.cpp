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

void delay()
{
    QTime dieTime= QTime::currentTime().addMSecs(10);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    _connectDialog = new CanSettingsDialog(nullptr, this);

    createActions();
    createMenus();

    /*EdsParser parser;

    DeviceDescription *deviceDescription = parser.parse("../../../fw/UMCfw/UMC1BDS32/umc1bds32fr.eds");
    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, 2);

    _odView = new ODTreeView();
    _odView->setDeviceModel(deviceDescription);
    _odView->setEditable(true);
    setCentralWidget(_odView);*/

//    _canOpen = new CanOpen();

//    _bus = _canOpen->addBus(new CanOpenBus(_canOpen, QCanBus::instance()->createDevice("socketcan", "can0")));
//    _bus->setBusName("Bus 1");
//    _bus->exploreBus();
//    _bus->addNode(new Node(_bus));
//    _bus->addNode(new Node(_bus));

    /*for (int id=1; id<=6; id++)
        _bus->_nmt->sendStop(id);

    _bus->_nmt->sendStart(nodeId);
    //_bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6060, 0x00, 2, 1);
    delay();
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6042, 0x00, 0, 2);
    delay();
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6040, 0x00, 0x06, 2);
    delay();
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6040, 0x00, 0x07, 2);
    delay();
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6040, 0x00, 0x0F, 2);
    delay();
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6040, 0x00, 0x7F, 2);
    delay();
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6042, 0x00, 2000, 2);
    delay();
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6048, 0x02, 1, 2);
    delay();
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6049, 0x02, 1, 2);
    delay();
    _bus->_sdos.first()->sendSdoReadReq(nodeId, 0x6040, 0x00);
    _bus->_sync->startSync(10);

    QTime dieTime= QTime::currentTime().addSecs(0);
    while (QTime::currentTime() < dieTime)
    {
        delay();
        //_bus->_sdos.first()->sendSdoReadReq(nodeId, 0x6064, 0x00);
    }
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6042, 0x00, 0, 2);
    delay();*/

    QWidget *widget = new QWidget();
    QLayout *layout = new QHBoxLayout();

    _busNodeTreeView = new BusNodesTreeView();

    layout->addWidget(_busNodeTreeView);
    _busNodeTreeView->expandAll();

    _canFrameListView = new CanFrameListView();

    layout->addWidget(_canFrameListView);
    widget->setLayout(layout);
    setCentralWidget(widget);

    resize(QApplication::screens()[0]->size() / 2);
}

MainWindow::~MainWindow()
{
    // _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6042, 0x00, 0, 2);
    delay();
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
    if (!_canDevice)
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

            _canOpen = new CanOpen();
            _bus = new CanOpenBus(_canDevice);
            _bus->setBusName(settings.interfaceName + "_" + settings.deviceName);
            _canOpen->addBus(_bus);
            _busNodeTreeView->setCanOpen(_canOpen);
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
