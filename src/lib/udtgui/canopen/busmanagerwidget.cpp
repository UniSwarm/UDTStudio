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
    updateData();
}

void BusManagerWidget::updateData()
{
    if (_bus)
    {
        _busNameEdit->setText(_bus->busName());
        // _busTypeLabel->setText();
        // _busStatusLabel->setText(_bus->);
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
    if (_syncStartAction->isChecked())
    {
        if (_bus)
        {
            _bus->sync()->startSync(i);
        }
    }
}

void BusManagerWidget::createWidgets()
{
    QAction *action;
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    _groupBox = new QGroupBox(tr("Bus"));
    QFormLayout *layoutGroupBox = new QFormLayout();

    _toolBar = new QToolBar(tr("Bus commands"));

    // explore
    action = _toolBar->addAction(tr("Explore"));
    action->setIcon(QIcon(":/icons/img/icons8-search-database.png"));
    action->setShortcut(QKeySequence("Ctrl+E"));
    action->setStatusTip(tr("Explore bus for new nodes"));
    connect(action, &QAction::triggered, this, &BusManagerWidget::exploreBus);

    // Sync one
    action = _toolBar->addAction(tr("Sync one"));
    action->setIcon(QIcon(":/icons/img/icons8-sync1.png"));
    action->setStatusTip(tr("Send one sync command"));
    connect(action, &QAction::triggered, this, &BusManagerWidget::sendSyncOne);

    // Sync timer
    _syncTimerSpinBox = new QSpinBox();
    _syncTimerSpinBox->setRange(10, 5000);
    _syncTimerSpinBox->setValue(1000);
    _syncTimerSpinBox->setSuffix(" ms");
    _syncTimerSpinBox->setStatusTip(tr("Sets the interval of sync timer in ms"));
    _toolBar->addWidget(_syncTimerSpinBox);
    connect(_syncTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ setSyncTimer(i); });

    _syncStartAction = _toolBar->addAction(tr("Start / stop sync"));
    _syncStartAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    _syncStartAction->setCheckable(true);
    _syncStartAction->setStatusTip(tr("Start / stop sync timer"));
    connect(_syncStartAction, &QAction::triggered, this, &BusManagerWidget::toggleSync);

    layoutGroupBox->addRow(_toolBar);

    _busNameEdit = new QLineEdit();
    layoutGroupBox->addRow(tr("Name :"), _busNameEdit);

    _busTypeLabel = new QLabel();
    layoutGroupBox->addRow(tr("Type :"), _busTypeLabel);

    _busStatusLabel = new QLabel();
    layoutGroupBox->addRow(tr("Status :"), _busStatusLabel);

    _groupBox->setLayout(layoutGroupBox);
    layout->addWidget(_groupBox);

    setLayout(layout);
}
