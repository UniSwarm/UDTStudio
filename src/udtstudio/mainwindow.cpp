#include "mainwindow.h"

#include "parser/cdfparser.h"

#include <QApplication>
#include <QScreen>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    CdfParser parser;
    OD *od = parser.parse("../../../firmware/UMC1BDS32/umc1bds32fr.eds");

    _odView = new ODTreeView();
    _odView->setOd(od);
    _odView->setEditable(true);
    setCentralWidget(_odView);

    resize(QApplication::screens()[0]->size() / 2);
}

MainWindow::~MainWindow()
{
}
