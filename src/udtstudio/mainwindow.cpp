#include "mainwindow.h"

#include "model/deviceconfiguration.h"
#include "parser/edsparser.h"

#include <QApplication>
#include <QCanBus>
#include <QScreen>

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
    _bus->exploreBus();

    resize(QApplication::screens()[0]->size() / 2);
}

MainWindow::~MainWindow()
{
}
