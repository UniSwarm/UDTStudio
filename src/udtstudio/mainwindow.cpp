#include "mainwindow.h"

#include "parser/devicemodelparser.h"

#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    DeviceModelParser parser;

    DeviceDescription *deviceDescription = static_cast<DeviceDescription*>(parser.parse("../../../firmware/UMC1BDS32/umc1bds32fr.eds", "eds"));
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
