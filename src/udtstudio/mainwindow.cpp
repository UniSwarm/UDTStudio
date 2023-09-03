/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

#include "mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QDateTime>
#include <QDir>
#include <QDockWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QScreen>
#include <QSettings>
#include <QSplitter>
#include <QStatusBar>

#include "canopen/datalogger/dataloggersingleton.h"

#ifdef Q_OS_UNIX
#    include "busdriver/canbussocketcan.h"
#endif
#include "busdriver/canbustcpudt.h"

#include "db/oddb.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle(tr("UniSwarm UDTStudio"));
    setWindowIcon(QIcon(":/icons/img/udtstudio.ico"));
    statusBar()->setVisible(true);

    createDocks();
    createWidgets();
    createMenus();
    connect(_busNodesManagerView, &BusNodesManagerView::nodeSelected, _nodeScreens, &NodeScreensWidget::setActiveNode);

    CanOpenBus *bus = nullptr;
#ifdef Q_OS_UNIX
    bus = new CanOpenBus(new CanBusSocketCAN("can0"));
    if (bus != nullptr)
    {
        bus->setBusName("Bus can0");
        CanOpen::addBus(bus);
        _canFrameListView->setBus(bus);
    }
    QTimer::singleShot(100, bus, &CanOpenBus::exploreBus);

    bus = new CanOpenBus(new CanBusSocketCAN("can1"));
    if (bus != nullptr)
    {
        bus->setBusName("Bus can1");
        CanOpen::addBus(bus);
    }
    QTimer::singleShot(100, bus, &CanOpenBus::exploreBus);
#endif

    /*bus = new CanOpenBus(new CanBusTcpUDT("192.168.1.80"));
    bus->setBusName("Bus net");
    CanOpen::addBus(bus);*/

    bus = new CanOpenBus(new CanBusDriver(""));
    bus->setBusName("VBus eds");
    CanOpen::addBus(bus);
    int id = 1;
    for (const QString &edsFile : qAsConst(OdDb::edsFiles()))
    {
        Node *node = new Node(id, QFileInfo(edsFile).completeBaseName(), edsFile);
        bus->addNode(node);
        id++;
    }

    resize(QApplication::screens().at(0)->size() * 3 / 4);

    readSettings();
}

MainWindow::~MainWindow()
{
    CanOpen::release();
}

void MainWindow::exportCfgFile()
{
    Node *node = _busNodesManagerView->currentNode();
    if (node == nullptr)
    {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save configuration file"), "", tr("Configuration file (*.conf)"));

    node->nodeOd()->exportConf(fileName);
}

void MainWindow::exportDCF()
{
    Node *node = _busNodesManagerView->currentNode();
    if (node == nullptr)
    {
        return;
    }

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save DCF File"), "", tr("Device File Configuration (*.dcf)"));

    node->nodeOd()->exportDcf(fileName);
}

void MainWindow::createDocks()
{
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::North);

    _busNodesManagerDock = new QDockWidget(tr("Bus nodes"), this);
    _busNodesManagerDock->setObjectName("busNodesManagerDock");
    _busNodesManagerView = new BusNodesManagerView(CanOpen::instance());
    _busNodesManagerDock->setWidget(_busNodesManagerView);
    addDockWidget(Qt::LeftDockWidgetArea, _busNodesManagerDock);

    _canFrameListDock = new QDockWidget(tr("Can frames"), this);
    _canFrameListDock->setObjectName("canFrameListDock");
    _canFrameListView = new CanFrameListView();
    _canFrameListDock->setWidget(_canFrameListView);
    addDockWidget(Qt::LeftDockWidgetArea, _canFrameListDock);
    tabifyDockWidget(_busNodesManagerDock, _canFrameListDock);

    _dataLoggerDock = new QDockWidget(tr("Data logger"), this);
    _dataLoggerDock->setObjectName("dataLoggerDock");
    _dataLoggerWidget = new DataLoggerWidget();
    _dataLoggerWidget->setTitle(tr("Dockable data logger"));
    _dataLoggerWidget->chartView()->setRollingTimeMs(10000);
    _dataLoggerWidget->chartView()->setRollingEnabled(true);
    _dataLoggerDock->setWidget(_dataLoggerWidget);
    addDockWidget(Qt::BottomDockWidgetArea, _dataLoggerDock);

    _busNodesManagerDock->raise();
}

void MainWindow::createWidgets()
{
    _nodeScreens = new NodeScreensWidget();
    setCentralWidget(_nodeScreens);
}

void MainWindow::createMenus()
{
    QAction *action;
    // ============= file =============
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    action = new QAction(tr("&Stop all"), this);
    action->setIcon(QIcon(":/icons/img/icons8-cancel.png"));
    action->setStatusTip(tr("Stop all nodes in all buses"));
    action->setShortcut(QKeySequence::Cancel);
    fileMenu->addAction(action);
    connect(action, &QAction::triggered, CanOpen::instance(), &CanOpen::stopAll);

    action = new QAction(tr("E&xit"), this);
    action->setIcon(QIcon(":/icons/img/icons8-exit.png"));
    action->setStatusTip(tr("Exits UDTStudio"));
    action->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(action);
    connect(action, &QAction::triggered, this, &MainWindow::close);

    // ============= Bus =============
    QMenu *busMenu = menuBar()->addMenu(tr("&Bus"));
    busMenu->addAction(_busNodesManagerView->busManagerWidget()->actionTogleConnect());
    busMenu->addAction(_busNodesManagerView->busManagerWidget()->actionExplore());
    busMenu->addAction(_busNodesManagerView->busManagerWidget()->actionSyncOne());
    busMenu->addAction(_busNodesManagerView->busManagerWidget()->actionSyncStart());

    // ============= Node =============
    QMenu *nodeMenu = menuBar()->addMenu(tr("&Node"));
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionPreop());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionStart());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionStop());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionReset());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionResetCom());
    nodeMenu->addSeparator();

    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionLoadEds());
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionReLoadEds());
    nodeMenu->addSeparator();

    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionRemoveNode());

    action = new QAction(tr("Export DCF file"), this);
    action->setShortcut(QKeySequence::SaveAs);
    action->setEnabled(false);
    connect(action, &QAction::triggered, this, &MainWindow::exportDCF);
    nodeMenu->addAction(action);
    connect(_busNodesManagerView, &BusNodesManagerView::nodeSelected, action, &QAction::setEnabled);

    action = new QAction(tr("Save conf file"), this);
    action->setEnabled(false);
    connect(action, &QAction::triggered, this, &MainWindow::exportCfgFile);
    nodeMenu->addAction(action);
    connect(_busNodesManagerView, &BusNodesManagerView::nodeSelected, action, &QAction::setEnabled);

    nodeMenu->addSeparator();
    nodeMenu->addAction(_busNodesManagerView->nodeManagerWidget()->actionUpdateFirmware());

    // ============= Loggers =============
    menuBar()->addMenu(DataLoggerSingleton::loggersMenu());

    // ============= View =============
    QMenu *viewMenu = menuBar()->addMenu(tr("&View"));

    action = _busNodesManagerDock->toggleViewAction();
    action->setStatusTip(tr("View/hide node manager dock"));
    viewMenu->addAction(action);

    action = _canFrameListDock->toggleViewAction();
    action->setStatusTip(tr("View/hide CAN frame viewer"));
    viewMenu->addAction(action);

    action = _dataLoggerDock->toggleViewAction();
    action->setStatusTip(tr("View/hide data logger"));
    viewMenu->addAction(action);

    // ============= Help =============
    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));

    QAction *aboutAction = new QAction(tr("&About"), this);
    aboutAction->setIcon(QIcon(":/icons/img/icons8-about.png"));
    connect(aboutAction, &QAction::triggered, this, &MainWindow::about);
    helpMenu->addAction(aboutAction);

    QAction *aboutQtAction = new QAction(tr("About &Qt"), this);
    aboutQtAction->setIcon(QIcon(":/icons/img/icons8-system-information.png"));
    connect(aboutQtAction, &QAction::triggered, qApp, &QApplication::aboutQt);
    helpMenu->addAction(aboutQtAction);
}

void MainWindow::writeSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    // MainWindow position/size/maximized
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.endGroup();

    settings.beginGroup("BusNodeManager");
    _busNodesManagerView->saveState(settings);
    settings.endGroup();
}

void MainWindow::readSettings()
{
    QSettings settings(QApplication::organizationName(), QApplication::applicationName());

    // MainWindow position/size/maximized
    settings.beginGroup("MainWindow");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    settings.endGroup();

    settings.beginGroup("BusNodeManager");
    _busNodesManagerView->restoreState(settings);
    settings.endGroup();
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::Close)
    {
        writeSettings();
        QApplication::quit();
    }
    return QMainWindow::event(event);
}

void MainWindow::about()
{
    QMessageBox::about(this,
                       "UDTStudio v0",
                       QString("Copyright (C) 2019-2021 UniSwarm (<a href=\"https://uniswarm.eu\">uniswarm.eu</a>)<br>\
<br>\
This sofware is part of uDevkit distribution. To check for new version, please visit <a href=\"https://github.com/UniSwarm/UDTStudio\">github.com/UniSwarm/UDTStudio</a><br>\
<br>\
Written by <a href=\"https://github.com/sebcaux\">Sébastien CAUX (sebcaux)</a><br>\
<br>\
UDTStudio is a free software: you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation, either version 3 of the License, or \
(at your option) any later version.<br>\
<br>\
This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the \
GNU General Public License for more details.<br>\
<br>\
You should have received a copy of the GNU General Public License \
along with this program. If not, see <a href=\"http://www.gnu.org/licenses/\">www.gnu.org/licenses</a><br>\
<br>\
Build date: ") + __DATE__ + QString(" time: ")
                           + __TIME__ + QString("<br>\
<br>\
UDTStudio use others open libraries :<br>\
- QDarkStyleSheet, a nice dark theme for Qt (dark theme) <a href=\"https://github.com/ColinDuquesnoy/QDarkStyleSheet\">github.com/ColinDuquesnoy/QDarkStyleSheet</a> [MIT]<br>\
- icons8, online icons library (menu and tool buttons) <a href=\"https://icons8.com\">icons8.com</a> [Free for Open]<br>\
"));
}
