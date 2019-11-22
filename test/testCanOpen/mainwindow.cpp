#include "mainwindow.h"

#include <QApplication>
#include <QCanBus>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    _button = new QPushButton("Explore");
    setCentralWidget(_button);
    connect(_button, &QPushButton::clicked, this, &MainWindow::pushButton_clicked);

    if (QCanBus::instance()->plugins().contains("socketcan"))
        _bus = new CanOpenBus(QCanBus::instance()->createDevice("socketcan", "can0"));
    else
        _bus = new CanOpenBus(QCanBus::instance()->createDevice("virtualcan", "can0"));
}

MainWindow::~MainWindow()
{
}

void MainWindow::pushButton_clicked()
{
    _bus->exploreBus();
    qDebug() << "explore";
}
