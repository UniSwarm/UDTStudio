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
    : QWidget(parent), _bus(nullptr)
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
    _bus = bus;
    _groupBox->setEnabled(_bus);
    _actionExplore->setEnabled(_bus);
    _actionSyncOne->setEnabled(_bus);
    _actionSyncStart->setEnabled(_bus);
    updateData();
}

void BusManagerWidget::updateData()
{
    if (_bus)
    {
        _busNameEdit->setText(_bus->busName());
    }
}

void BusManagerWidget::exploreBus()
{
    if (_bus)
    {
        _bus->exploreBus();
    }
}

void BusManagerWidget::sendSyncOne()
{
    if (_bus)
    {
        _bus->sync()->sendSyncOne();
    }
}

void BusManagerWidget::toggleSync(bool start)
{
    if (_bus)
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
        if (_bus)
        {
            _bus->sync()->startSync(i);
        }
    }
}

void BusManagerWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    _groupBox = new QGroupBox(tr("Bus"));
    QFormLayout *layoutGroupBox = new QFormLayout();
    layoutGroupBox->setSpacing(2);
    layoutGroupBox->setContentsMargins(5, 5, 5, 5);

    _toolBar = new QToolBar(tr("Bus commands"));
    _toolBar->setIconSize(QSize(20, 20));

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
    _syncTimerSpinBox->setValue(1000);
    _syncTimerSpinBox->setSuffix(" ms");
    _syncTimerSpinBox->setStatusTip(tr("Sets the interval of sync timer in ms"));
    _toolBar->addWidget(_syncTimerSpinBox);
    connect(_syncTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ setSyncTimer(i); });

    _actionSyncStart = _toolBar->addAction(tr("Start / stop sync"));
    _actionSyncStart->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    _actionSyncStart->setCheckable(true);
    _actionSyncStart->setStatusTip(tr("Start / stop sync timer"));
    connect(_actionSyncStart, &QAction::triggered, this, &BusManagerWidget::toggleSync);

    layoutGroupBox->addRow(_toolBar);

    _busNameEdit = new QLineEdit();
    layoutGroupBox->addRow(tr("Name :"), _busNameEdit);

    _groupBox->setLayout(layoutGroupBox);
    layout->addWidget(_groupBox);

    setLayout(layout);
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
