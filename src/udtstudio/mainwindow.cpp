#include "mainwindow.h"

#include "model/deviceconfiguration.h"
#include "parser/edsparser.h"

#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
     EdsParser parser;

    DeviceDescription *deviceDescription = parser.parse("../../../firmware/UMC1BDS32/umc1bds32fr.eds");
    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, 2);

    _odView = new ODTreeView();
    _odView->setDeviceModel(deviceConfiguration);
    _odView->setEditable(true);
    setCentralWidget(_odView);

    resize(QApplication::screens()[0]->size() / 2);
}

MainWindow::~MainWindow()
{
}
