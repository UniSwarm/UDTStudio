/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#include <QCanBus>
#include <QDebug>
#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QStatusBar>

#include "model/deviceconfiguration.h"
#include "node.h"
#include "parser/edsparser.h"

#include "programdownload.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    _connectDialog = new CanSettingsDialog(nullptr, this);
    statusBar()->setVisible(true);

    CanOpenBus *bus;
    if (QCanBus::instance()->plugins().contains("socketcan"))
    {
        bus = new CanOpenBus(QCanBus::instance()->createDevice("socketcan", "can0"));
    }
    else
    {
        bus = new CanOpenBus(QCanBus::instance()->createDevice("virtualcan", "can0"));
    }
    bus->setBusName("Bus 1");
    CanOpen::addBus(bus);

    createWidget();
    createMenus();

    setWindowTitle(tr("UBL"));
}

void MainWindow::nodeChanged(Node *currentNode)
{
    if (!currentNode)
    {
        statusBar()->showMessage(tr("No node selected"));
        return;
    }
    for (ProgramDownload *program : _programList)
    {
        if (program->nodeId() == currentNode->nodeId())
        {
            refreshInfo();
            return;
        }
    }

    ProgramDownload *program = new ProgramDownload(currentNode);
    _programList.append(program);
    connect(program, &ProgramDownload::downloadFinished, this, &MainWindow::updateProgram);
    connect(program, &ProgramDownload::downloadState, this, &MainWindow::downloadState);
    refreshInfo();
}
void MainWindow::update()
{
    _iNodeListSelected = 0;
    updateProgram();
}

void MainWindow::updateProgram()
{
    if (_iNodeListSelected < _programList.size())
    {
        _programList.at(_iNodeListSelected)->update();
    }
    _iNodeListSelected++;
}

void MainWindow::downloadState(QString state)
{
    _statusLabel->setText(state);
}

void MainWindow::openHexFile()
{
    if (!_busNodesManagerView->currentNode())
    {
        statusBar()->showMessage(tr("No node selected"));
        return;
    }

    QString file = QFileDialog::getOpenFileName(this, "exemple.hex", "ex", "Hex (*.hex)");
    for (ProgramDownload *program : _programList)
    {
        if (program->nodeId() == _busNodesManagerView->currentNode()->nodeId())
        {
            program->openHex(file);
            _fileNameHexDataLabel->setText(program->fileNameHex());
        }
    }
}

void MainWindow::openEdsFile()
{
    if (!_busNodesManagerView->currentNode())
    {
        statusBar()->showMessage(tr("No node selected"));
        return;
    }

    QString file = QFileDialog::getOpenFileName(this, "exemple.hex", "ex", "Hex (*.eds)");
    for (ProgramDownload *program : _programList)
    {
        if (program->nodeId() == _busNodesManagerView->currentNode()->nodeId())
        {
            program->loadEds(file);
            _fileNameEdsDataLabel->setText(program->fileNameEds());
        }
    }
}

void MainWindow::refreshInfo()
{
    Node *currentNode = _busNodesManagerView->currentNode();
    if (!currentNode)
    {
        statusBar()->showMessage(tr("No node selected"));
        return;
    }

    _groupBoxInfo->setTitle("Information Node :" + QString::number(currentNode->nodeId(), 16) + ", Device : 0x" +
                            QString("0x%1").arg(QString::number(currentNode->nodeOd()->value(0x1000, 0).toUInt(), 16).toUpper()));
    _serialNumberLabel->setText(QString("0x%1").arg(QString::number(currentNode->nodeOd()->value(0x1018, 1).toUInt(), 16).toUpper()));
    _vendorIdLabel->setText(QString("0x%1").arg(QString::number(currentNode->nodeOd()->value(0x1018, 2).toUInt(), 16).toUpper()));
    _productCodeLabel->setText(QString("0x%1").arg(QString::number(currentNode->nodeOd()->value(0x1018, 3).toUInt(), 16).toUpper()));
    _revisionNumberLabel->setText(QString("0x%1").arg(QString::number(currentNode->nodeOd()->value(0x1018, 4).toUInt(), 16).toUpper()));
}

void MainWindow::uploadEds()
{
    for (ProgramDownload *program : _programList)
    {
        if (program->nodeId() == _busNodesManagerView->currentNode()->nodeId())
        {
            program->uploadEds();
        }
    }
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
            }

            _bus = new CanOpenBus(_canDevice);
            CanOpen::addBus(_bus);
            _bus->setBusName(settings.interfaceName + ":" + settings.deviceName);
            statusBar()->showMessage(tr("%1 - %2").arg(settings.interfaceName).arg(settings.deviceName));
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
    statusBar()->showMessage(tr("Disconnected"));
}

void MainWindow::createMenus()
{
    // ============= file =============
    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));

    QAction *exitAction = new QAction(tr("E&xit"), this);
    exitAction->setStatusTip(tr("Exits UDTStudio"));
    exitAction->setShortcut(QKeySequence::Quit);
    fileMenu->addAction(exitAction);
    connect(exitAction, SIGNAL(triggered()), this, SLOT(close()));

    QAction *_connectAction = new QAction(tr("&Connect"), this);
    connect(_connectAction, &QAction::triggered, this, &MainWindow::connectDevice);

    QAction *_disconnectAction = new QAction(tr("&Disconnect"), this);
    connect(_disconnectAction, &QAction::triggered, this, &MainWindow::disconnectDevice);

    QAction *_canSettingsAction = new QAction(tr("&Can Settings"), this);
    connect(_canSettingsAction, &QAction::triggered, _connectDialog, &CanSettingsDialog::show);

    QMenu *menu;
    menu = menuBar()->addMenu(tr("&Connection"));
    menu->addAction(_connectAction);
    menu->addAction(_disconnectAction);
    menu->addAction(_canSettingsAction);
    menu->addSeparator();

    connect(_connectDialog, &QDialog::accepted, this, &MainWindow::connectDevice);
    connect(_busNodesManagerView, &BusNodesTreeView::nodeSelected, this, &MainWindow::nodeChanged);
}

void MainWindow::createWidget()
{
    _busNodesManagerView = new BusNodesTreeView(CanOpen::instance());

    QLayout *infoLayout = new QVBoxLayout();
    // Information

    _serialNumberLabel = new QLabel();
    _vendorIdLabel = new QLabel();
    _productCodeLabel = new QLabel();
    _revisionNumberLabel = new QLabel();

    QFormLayout *formLayout = new QFormLayout;

    formLayout->addRow(tr("Vendor ID :"), _vendorIdLabel);
    formLayout->addRow(tr("Product Code :"), _productCodeLabel);
    formLayout->addRow(tr("Revison number :"), _revisionNumberLabel);
    formLayout->addRow(tr("Serial Number :"), _serialNumberLabel);
    formLayout->setSpacing(5);
    _uploadEdsPushButton = new QPushButton(tr("Upload EDS"), this);
    QHBoxLayout *infoButtonLayout = new QHBoxLayout();
    infoButtonLayout->addWidget(_uploadEdsPushButton);

    infoLayout->addItem(formLayout);
    infoLayout->addItem(infoButtonLayout);

    _groupBoxInfo = new QGroupBox("Information");
    _groupBoxInfo->setLayout(infoLayout);

    // File Eds
    QHBoxLayout *fileNameEdsLayout = new QHBoxLayout();
    QLabel *fileNameLabel = new QLabel(tr("EDS :"));
    _fileNameEdsDataLabel = new QLabel("...");
    QPushButton *openFileEdsPushButton = new QPushButton(tr("..."), this);
    fileNameEdsLayout->addWidget(fileNameLabel);
    fileNameEdsLayout->addWidget(_fileNameEdsDataLabel);
    fileNameEdsLayout->addWidget(openFileEdsPushButton);
    fileNameEdsLayout->setSpacing(5);

    // Hex
    QHBoxLayout *fileNameHexLayout = new QHBoxLayout();
    QLabel *fileNameHexLabel = new QLabel(tr("Hex :"));
    _fileNameHexDataLabel = new QLabel("...");
    QPushButton *openFileHexPushButton = new QPushButton(tr("..."), this);
    fileNameHexLayout->addWidget(fileNameHexLabel);
    fileNameHexLayout->addWidget(_fileNameHexDataLabel);
    fileNameHexLayout->addWidget(openFileHexPushButton);
    fileNameHexLayout->setSpacing(5);

    QGroupBox *fileBox = new QGroupBox("File");
    QVBoxLayout *fileNameLayout = new QVBoxLayout();
    fileNameLayout->addItem(fileNameEdsLayout);
    fileNameLayout->addItem(fileNameHexLayout);
    fileBox->setLayout(fileNameLayout);

    _statusLabel = new QLabel();
    _statusLabel->setText("Status :");

    _updatePushButton = new QPushButton(tr("Update"));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(fileBox);
    layout->addWidget(_groupBoxInfo);

    layout->addWidget(_statusLabel);
    layout->setSpacing(10);
    layout->addWidget(_updatePushButton);

    QLayout *hLayout = new QHBoxLayout();
    hLayout->addWidget(_busNodesManagerView);
    hLayout->addItem(layout);

    QWidget *widget = new QWidget;
    widget->setLayout(hLayout);

    setCentralWidget(widget);
    connect(_updatePushButton, &QPushButton::clicked, this, &MainWindow::update);
    connect(openFileHexPushButton, &QPushButton::clicked, this, &MainWindow::openHexFile);
    connect(openFileEdsPushButton, &QPushButton::clicked, this, &MainWindow::openEdsFile);
    connect(_uploadEdsPushButton, &QPushButton::clicked, this, &MainWindow::uploadEds);
}
