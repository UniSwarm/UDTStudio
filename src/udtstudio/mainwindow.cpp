#include "mainwindow.h"

#include "model/deviceconfiguration.h"
#include "parser/edsparser.h"

#include <QApplication>
#include <QCanBus>
#include <QDateTime>
#include <QScreen>
#include <QLayout>

void delay()
{
    QTime dieTime= QTime::currentTime().addMSecs(10);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

uint8_t nodeId = 2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    /*EdsParser parser;

    DeviceDescription *deviceDescription = parser.parse("../../../fw/UMCfw/UMC1BDS32/umc1bds32fr.eds");
    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, 2);

    _odView = new ODTreeView();
    _odView->setDeviceModel(deviceDescription);
    _odView->setEditable(true);
    setCentralWidget(_odView);*/

    _canOpen = new CanOpen();
    _bus = _canOpen->addBus(new CanOpenBus(_canOpen, QCanBus::instance()->createDevice("socketcan", "can0")));
    _bus->setBusName("Bus 1");
    _bus->exploreBus();
    _bus->addNode(new Node(_bus));
    _bus->addNode(new Node(_bus));

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
    _busNodeTreeView->setCanOpen(_canOpen);
    layout->addWidget(_busNodeTreeView);
    _busNodeTreeView->expandAll();

    _canFrameListView = new CanFrameListView();
    /*QCanBusFrame frame(0x80, QByteArray("ABC123", 6));
    _canFrameListView->appendCanFrame(frame);*/
    layout->addWidget(_canFrameListView);
    connect(_bus, &CanOpenBus::frameAvailable, _canFrameListView, &CanFrameListView::appendCanFrame);

    widget->setLayout(layout);
    setCentralWidget(widget);

    resize(QApplication::screens()[0]->size() / 2);
}

MainWindow::~MainWindow()
{
    // _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6042, 0x00, 0, 2);
    delay();
}
