#include "mainwindow.h"

#include "model/deviceconfiguration.h"
#include "parser/edsparser.h"

#include <QApplication>
#include <QCanBus>
#include <QDateTime>
#include <QScreen>

void delay()
{
    QTime dieTime= QTime::currentTime().addMSecs(10);
    while (QTime::currentTime() < dieTime)
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
     EdsParser parser;

    DeviceDescription *deviceDescription = parser.parse("../../../firmware/UMC1BDS32/umc1bds32fr.eds");
    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, 2);

    _odView = new ODTreeView();
    _odView->setDeviceModel(deviceDescription);
    _odView->setEditable(true);
    setCentralWidget(_odView);

    _bus = new CanOpenBus(QCanBus::instance()->createDevice("socketcan", "can0"));
    //_bus->exploreBus();

    for (int id=1; id<=6; id++)
        _bus->_nmt->sendStop(id);

    uint8_t nodeId = 2;
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
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6042, 0x00, 50, 2);
    delay();
    _bus->_sdos.first()->sendSdoReadReq(nodeId, 0x6042, 0x00);
    delay();
    _bus->_sdos.first()->sendSdoReadReq(nodeId, 0x6040, 0x00);
    _bus->_sync->startSync(10);

    QTime dieTime= QTime::currentTime().addSecs(3);
    while (QTime::currentTime() < dieTime)
    {
        delay();
        //_bus->_sdos.first()->sendSdoReadReq(nodeId, 0x6064, 0x00);
    }
    _bus->_sdos.first()->sendSdoWriteReq(nodeId, 0x6042, 0x00, 0, 2);
    delay();

    resize(QApplication::screens()[0]->size() / 2);
}

MainWindow::~MainWindow()
{
}
