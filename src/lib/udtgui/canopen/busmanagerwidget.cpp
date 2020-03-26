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

void BusManagerWidget::createWidgets()
{
    QAction *action;
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    _groupBox = new QGroupBox(tr("Bus"));
    QFormLayout *layoutGroupBox = new QFormLayout();

    _toolBar = new QToolBar(tr("Bus commands"));
    action = _toolBar->addAction(tr("Explore"));
    connect(action, &QAction::triggered, this, &BusManagerWidget::exploreBus);
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
