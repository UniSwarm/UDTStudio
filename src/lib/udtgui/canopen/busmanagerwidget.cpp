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

#include "busmanagerwidget.h"

#include <QFormLayout>

BusManagerWidget::BusManagerWidget(QWidget *parent)
    : BusManagerWidget(nullptr, parent)
{
}

BusManagerWidget::BusManagerWidget(CanOpenBus *bus, QWidget *parent)
    : QWidget(parent)
    , _bus(nullptr)
{
    createWidgets();
    setBus(bus);
}

CanOpenBus *BusManagerWidget::bus() const
{
    return _bus;
}

void BusManagerWidget::setBus(CanOpenBus *bus)
{
    if (_bus != nullptr)
    {
        disconnect(_bus, nullptr, this, nullptr);
    }

    _bus = bus;
    if (_bus != nullptr)
    {
        connect(_bus, &CanOpenBus::connectedChanged, this, &BusManagerWidget::updateBusData);
        connect(_bus, &CanOpenBus::busNameChanged, this, &BusManagerWidget::updateBusData);
    }

    _groupBox->setEnabled(_bus != nullptr);
    updateBusData();
}

void BusManagerWidget::updateBusData()
{
    if (_bus != nullptr)
    {
        _actionTogleConnect->blockSignals(true);
        _actionTogleConnect->setEnabled(true);
        _actionTogleConnect->setChecked(_bus->isConnected());
        _actionTogleConnect->setText(_bus->isConnected() ? tr("Disconnect") : tr("Connect"));
        _actionTogleConnect->blockSignals(false);

        _actionSyncStart->blockSignals(true);
        _actionSyncStart->setChecked(_bus->sync()->status() == Sync::STARTED);
        _actionSyncStart->blockSignals(false);

        _busNameEdit->setText(_bus->busName());

        _actionExplore->setEnabled(_bus->isConnected());
        _actionSyncOne->setEnabled(_bus->isConnected());
        _actionSyncStart->setEnabled(_bus->isConnected());
    }
    if (_bus == nullptr)
    {
        _actionTogleConnect->setChecked(false);
        _actionTogleConnect->setEnabled(false);
        _actionExplore->setEnabled(false);
        _actionSyncOne->setEnabled(false);
        _actionSyncStart->setEnabled(false);
    }
}

void BusManagerWidget::togleConnect()
{
    if (_bus != nullptr)
    {
        if (_bus->canBusDriver() != nullptr)
        {
            if (_bus->isConnected())
            {
                _bus->canBusDriver()->disconnectDevice();
            }
            else
            {
                _bus->canBusDriver()->connectDevice();
            }
        }
    }
}

void BusManagerWidget::exploreBus()
{
    if (_bus != nullptr)
    {
        _bus->exploreBus();
    }
}

void BusManagerWidget::sendSyncOne()
{
    if (_bus != nullptr)
    {
        _bus->sync()->sendSyncOne();
    }
}

void BusManagerWidget::toggleSync(bool start)
{
    if (_bus != nullptr)
    {
        if (start)
        {
            _bus->sync()->startSync(_syncTimerSpinBox->value());
        }
        else
        {
            _bus->sync()->stopSync();
        }
    }
}

void BusManagerWidget::setSyncTimer(int i)
{
    if (_actionSyncStart->isChecked())
    {
        if (_bus != nullptr)
        {
            _bus->sync()->startSync(i);
        }
    }
}

void BusManagerWidget::setBusName()
{
    if (_bus != nullptr)
    {
        _bus->setBusName(_busNameEdit->text());
    }
}

void BusManagerWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    _groupBox = new QGroupBox(tr("Bus"));
    QFormLayout *layoutGroupBox = new QFormLayout();
    layoutGroupBox->setSpacing(2);
    layoutGroupBox->setContentsMargins(2, 2, 2, 2);

    _toolBar = new QToolBar(tr("Bus commands"));
    _toolBar->setIconSize(QSize(20, 20));

    // connect
    _actionTogleConnect = _toolBar->addAction(tr("Connect"));
    QIcon iconConnect;
    iconConnect.addFile(":/icons/img/icons8-disconnected.png", QSize(), QIcon::Normal, QIcon::Off);
    iconConnect.addFile(":/icons/img/icons8-connected.png", QSize(), QIcon::Normal, QIcon::On);
    _actionTogleConnect->setIcon(iconConnect);
    _actionTogleConnect->setCheckable(true);
    _actionTogleConnect->setStatusTip(tr("Connect/disconnect bus"));
    connect(_actionTogleConnect, &QAction::triggered, this, &BusManagerWidget::togleConnect);

    // explore
    _actionExplore = _toolBar->addAction(tr("Explore"));
    _actionExplore->setIcon(QIcon(":/icons/img/icons8-search-database.png"));
    _actionExplore->setShortcut(QKeySequence("Ctrl+E"));
    _actionExplore->setStatusTip(tr("Explore bus for new nodes"));
    connect(_actionExplore, &QAction::triggered, this, &BusManagerWidget::exploreBus);

    // Sync one
    _actionSyncOne = _toolBar->addAction(tr("Sync one"));
    _actionSyncOne->setIcon(QIcon(":/icons/img/icons8-sync1.png"));
    _actionSyncOne->setStatusTip(tr("Send one sync command"));
    connect(_actionSyncOne, &QAction::triggered, this, &BusManagerWidget::sendSyncOne);

    // Sync timer
    _syncTimerSpinBox = new QSpinBox();
    _syncTimerSpinBox->setRange(1, 5000);
    _syncTimerSpinBox->setValue(100);
    _syncTimerSpinBox->setSuffix(" ms");
    _syncTimerSpinBox->setStatusTip(tr("Sets the interval of sync timer in ms"));
    _toolBar->addWidget(_syncTimerSpinBox);
    connect(_syncTimerSpinBox,
            &QSpinBox::editingFinished,
            this,
            [=]()
            {
                setSyncTimer(_syncTimerSpinBox->value());
            });

    _actionSyncStart = _toolBar->addAction(tr("Start / stop sync"));
    _actionSyncStart->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    _actionSyncStart->setCheckable(true);
    _actionSyncStart->setStatusTip(tr("Start / stop sync timer"));
    connect(_actionSyncStart, &QAction::triggered, this, &BusManagerWidget::toggleSync);

    layoutGroupBox->addRow(_toolBar);
    layoutGroupBox->addItem(new QSpacerItem(0, 2));

    _busNameEdit = new QLineEdit();
    layoutGroupBox->addRow(tr("Name:"), _busNameEdit);
    connect(_busNameEdit, &QLineEdit::returnPressed, this, &BusManagerWidget::setBusName);

    _groupBox->setLayout(layoutGroupBox);
    layout->addWidget(_groupBox);

    setLayout(layout);
}

QAction *BusManagerWidget::actionTogleConnect() const
{
    return _actionTogleConnect;
}

QAction *BusManagerWidget::actionSyncStart() const
{
    return _actionSyncStart;
}

QAction *BusManagerWidget::actionSyncOne() const
{
    return _actionSyncOne;
}

QAction *BusManagerWidget::actionExplore() const
{
    return _actionExplore;
}
