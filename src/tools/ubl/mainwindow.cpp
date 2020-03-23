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

#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include <QFormLayout>
#include <QFileDialog>
#include <QListView>
#include <QMessageBox>
#include <QDebug>

#include "node.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    _connectDialog = new CanSettingsDialog(nullptr, this);

    widget = new QWidget;
    setCentralWidget(widget);

    createActions();
    createMenus();
    createWidget();

    setWindowTitle(tr("UBL"));
    setMinimumSize(200, 200);
    resize(480, 320);
}


void MainWindow::createActions()
{
    _quitAction = new QAction(tr("&Quit"), this);
    connect(_quitAction, &QAction::triggered, this, &MainWindow::close);

    _connectAction = new QAction(tr("&Connect"), this);
    connect(_connectAction, &QAction::triggered, this, &MainWindow::connectDevice);

    _disconnectAction = new QAction(tr("&Disconnect"), this);
    connect(_disconnectAction, &QAction::triggered, this, &MainWindow::disconnectDevice);

    _canSettingsAction = new QAction(tr("&Can Settings"), this);
    connect(_canSettingsAction, &QAction::triggered, _connectDialog, &CanSettingsDialog::show);
    connect(_connectDialog, &QDialog::accept, this, &MainWindow::connectDevice);

    _connectAction->setEnabled(true);
    _disconnectAction->setEnabled(false);
}

void MainWindow::exploreBus()
{
    if (!_canDevice)
    {
        statusBar()->showMessage(tr("No interface"));
        return;
    }
    _bus->exploreBus();
}

void MainWindow::refreshListNode()
{
    if (!_canDevice)
    {
        statusBar()->showMessage(tr("No interface"));
        return;
    }

    QStringList List;
    for (int i = 0; i < _bus->nodes().size(); ++i)
    {
        List.append(QString::number(_bus->nodes().at(i)->nodeId()));
    }
    model->setStringList(List);
    _tableView->setModel(model);
}

void MainWindow::update()
{
    if (!_canDevice)
    {
        statusBar()->showMessage(tr("No interface"));
        return;
    }

    //        // Savce file
    //        QFile file("fileBeforeSend.hex");
    //        if (!file.open(QFile::WriteOnly))
    //        {
    //            //      ...
    //            //            return false;
    //        }
    //        else
    //        {
    //            file.write(hexFile->prog());
    //            file.close();
    if (!hexFile)
    {
        openFile();
    }
    else
    {
        QItemSelectionModel *selection = _tableView->selectionModel();
        QModelIndexList listeSelections = selection->selectedIndexes();
        QStringList elementsSelectionnes;

        for (int i = 0; i < listeSelections.size(); i++)
        {
            qDebug() << " listeSelections.at(i).data(:" << listeSelections.at(i).data(Qt::DisplayRole).toInt();

            int index = listeSelections.at(i).data(Qt::DisplayRole).toInt();
            if (_bus->existNode(static_cast<uint8_t>(index)) == true)
            {
                _bus->node(static_cast<uint8_t>(index))->updateFirmware(hexFile->prog());
            }
        }
    }

    //        }
}

void MainWindow::openFile()
{
    fileNameHex = QFileDialog::getOpenFileName(this, "exemple.hex", "ex", "Hex (*.hex)");
    hexFile = new HexFile(fileNameHex);

    QFileInfo fileInfo(fileNameHex);
    _fileNameDataLabel->setText(fileInfo.fileName());
    hexFile->read();
    _updatePushButton->setEnabled(true);
}

void MainWindow::refreshOInfo()
{
    QItemSelectionModel *selection = _tableView->selectionModel();
    QModelIndexList listeSelections = selection->selectedIndexes();
    QStringList elementsSelectionnes;

    int index = listeSelections.at(0).data(Qt::DisplayRole).toInt();
    _deviceDataLabel->setText(_bus->node(static_cast<uint8_t>(index))->device());
    _manuDeviceNameDataLabel->setText(_bus->node(static_cast<uint8_t>(index))->manuDeviceName());
    _manufacturerHardwareVersionDataLabel->setText(_bus->node(static_cast<uint8_t>(index))->manufacturerHardwareVersion());
    _manufacturerSoftwareVersionDataLabel->setText(_bus->node(static_cast<uint8_t>(index))->manufacturerSoftwareVersion());
}

void MainWindow::addEds()
{
    if (!_canDevice)
    {
        statusBar()->showMessage(tr("No interface"));
        return;
    }

    QItemSelectionModel *selection = _tableView->selectionModel();
    if (selection == nullptr)
    {
        return;
    }

    QModelIndexList listeSelections = selection->selectedIndexes();
    if (listeSelections.size() == 0)
    {
        QMessageBox::warning(this, tr("UBL"), tr("Please select a Node"), QMessageBox::Cancel);
        return;
    }
    else
    {
        fileNameEds = QFileDialog::getOpenFileName(this, "exemple.hex", "ex", "Hex (*.eds)");
        QStringList elementsSelectionnes;

        for (int i = 0; i < listeSelections.size(); i++)
        {
            qDebug() << " listeSelections.at(i).data(:" << listeSelections.at(i).data(Qt::DisplayRole).toInt();

            int index = listeSelections.at(i).data(Qt::DisplayRole).toInt();
            if (_bus->existNode(static_cast<uint8_t>(index)) == true)
            {
                _bus->node(static_cast<uint8_t>(index))->addEds(fileNameEds);
            }
        }
        _refreshPushButton->setEnabled(true);
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

                _connectAction->setEnabled(false);
                _disconnectAction->setEnabled(true);
            }

            _bus = new CanOpenBus(_canDevice);
            connect(_bus, &CanOpenBus::nodeAdded, this, &MainWindow::refreshListNode);
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
    _connectAction->setEnabled(true);
    _disconnectAction->setEnabled(false);
    statusBar()->showMessage(tr("Disconnected"));
}


void MainWindow::createMenus()
{
    _fileMenu = menuBar()->addMenu(tr("&File"));
    _fileMenu->addAction(_quitAction);
    _fileMenu->addSeparator();

    _connectMenu = menuBar()->addMenu(tr("&Connection"));
    _connectMenu->addAction(_connectAction);
    _connectMenu->addAction(_disconnectAction);
    _connectMenu->addAction(_canSettingsAction);
    _connectMenu->addSeparator();
}

void MainWindow::createWidget()
{
    QLayout *infoLayout = new QVBoxLayout();
    // Information

    _deviceDataLabel = new QLabel();
    _manuDeviceNameDataLabel = new QLabel();
    _manufacturerHardwareVersionDataLabel = new QLabel();
    _manufacturerSoftwareVersionDataLabel = new QLabel();

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow(tr("&Device :"), _deviceDataLabel);
    formLayout->addRow(tr("D&evice Name :"), _manuDeviceNameDataLabel);
    formLayout->addRow(tr("&Hardware Version :"), _manufacturerHardwareVersionDataLabel);
    formLayout->addRow(tr("&Software Version :"), _manufacturerSoftwareVersionDataLabel);

    QPushButton *addEdsPushButton = new QPushButton(tr("Add Eds"), this);
    _refreshPushButton = new QPushButton(tr("Refresh"), this);
    QHBoxLayout *infoButtonLayout = new QHBoxLayout();
    infoButtonLayout->addWidget(addEdsPushButton);
    infoButtonLayout->addWidget(_refreshPushButton);

    infoLayout->addItem(formLayout);
    infoLayout->addItem(infoButtonLayout);

    QGroupBox *groupBox = new QGroupBox("Information");
    groupBox->setLayout(infoLayout);

    // File Hex
    QHBoxLayout *fileNameLayout = new QHBoxLayout();
    QLabel *fileNameLabel = new QLabel(tr("File Name :"));
    _fileNameDataLabel = new QLabel("");
    fileNameLayout->addWidget(fileNameLabel);
    fileNameLayout->addWidget(_fileNameDataLabel);

    QPushButton *openFileHexPushButton = new QPushButton(tr("Open File"), this);
    QLayout *fileLayout = new QVBoxLayout();
    fileLayout->addItem(fileNameLayout);
    fileLayout->addWidget(openFileHexPushButton);

    QGroupBox *fileBox = new QGroupBox("File");
    fileBox->setLayout(fileLayout);

    _updatePushButton = new QPushButton(tr("Update"));

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(groupBox);
    layout->addWidget(fileBox);
    layout->setSpacing(10);
    layout->addWidget(_updatePushButton);

    _tableView = new QListView();
    QPushButton *exploreBusPushButton = new QPushButton(tr("Explore Bus"), this);
    QLayout *busLayout = new QVBoxLayout();
    busLayout->addWidget(_tableView);
    busLayout->addWidget(exploreBusPushButton);

    QLayout *hLayout = new QHBoxLayout();
    hLayout->addItem(busLayout);
    hLayout->addItem(layout);

    widget->setLayout(hLayout);

    _updatePushButton->setEnabled(false);
    _refreshPushButton->setEnabled(false);

    connect(exploreBusPushButton, &QPushButton::clicked, this, &MainWindow::exploreBus);
    connect(_updatePushButton, &QPushButton::clicked, this, &MainWindow::update);
    connect(openFileHexPushButton, &QPushButton::clicked, this, &MainWindow::openFile);
    connect(addEdsPushButton, &QPushButton::clicked, this, &MainWindow::addEds);
    connect(_refreshPushButton, &QPushButton::clicked, this, &MainWindow::refreshOInfo);

    model = new QStringListModel(this);
}
