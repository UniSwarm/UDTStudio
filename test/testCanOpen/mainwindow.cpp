#include "mainwindow.h"

#include <QPushButton>
#include <QLayout>

#include <QApplication>
#include <QDebug>
#include <QDataStream>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    toggle = false;
    _value = 402;

    QPushButton *button;
    QWidget *widget = new QWidget();
    QLayout *layout = new QVBoxLayout();

    button = new QPushButton("explore");
    layout->addWidget(button);
    connect(button, &QPushButton::clicked, this, &MainWindow::explore);

    button = new QPushButton("sendState");
    layout->addWidget(button);
    connect(button, &QPushButton::clicked, this, &MainWindow::sendState);

    button = new QPushButton("replySdo1000");
    layout->addWidget(button);
    connect(button, &QPushButton::clicked, this, &MainWindow::replySdo1000);

    _canView = new CanFrameListView();
    layout->addWidget(_canView);

    widget->setLayout(layout);
    setCentralWidget(widget);

    _canOpen = new CanOpen();
    CanOpenBus *bus;
    if (QCanBus::instance()->plugins().contains("socketcan"))
    {
        bus = new CanOpenBus(QCanBus::instance()->createDevice("socketcan", "can0"));
    }
    else
    {
        bus = new CanOpenBus(QCanBus::instance()->createDevice("virtualcan", "can0"));
    }
    _canDev = bus->canDevice();
    bus->setBusName("Bus 1");
    _canOpen->addBus(bus);
    connect(bus, &CanOpenBus::frameAvailable, _canView, &CanFrameListView::appendCanFrame);
}

MainWindow::~MainWindow()
{
}

void MainWindow::explore()
{
    _canDev->writeFrame(QCanBusFrame(0x702, QByteArray()));
    //_canDev->writeFrame(QCanBusFrame(0x708, QByteArray()));
    //_canDev->writeFrame(QCanBusFrame(0x703, QByteArray()));
}

void MainWindow::sendState()
{
    char c = 4 + toggle * 0x80;
    toggle = !toggle;
    QByteArray data;
    data.append(c);
    _canDev->writeFrame(QCanBusFrame(0x702, data));
}

void MainWindow::replySdo1000()
{
    unsigned char command = 0x43u;
    quint16 index = 0x1000;
    quint8 subIndex = 0x00;
    quint32 value = _value;
    _value += 11;
    QByteArray data;
    QDataStream stream(&data, QIODevice::WriteOnly);
    stream.setByteOrder(QDataStream::LittleEndian);
    stream << command << index << subIndex << value;
    _canDev->writeFrame(QCanBusFrame(0x582, data));
}
