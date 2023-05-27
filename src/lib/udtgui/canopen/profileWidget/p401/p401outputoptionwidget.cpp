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

#include "p401outputoptionwidget.h"

#include "canopen/indexWidget/indexcombobox.h"

#include "indexdb401.h"

#include <QFormLayout>

P401OutputOptionWidget::P401OutputOptionWidget(uint8_t channel, QWidget *parent)
    : QWidget(parent),
      _channel(channel)
{
    createWidgets();
}

void P401OutputOptionWidget::readAllObject()
{
    _doPwmFrequencyComboBox->readObject();
}

void P401OutputOptionWidget::setNode(Node *node)
{
    if (node == nullptr)
    {
        return;
    }
    _node = node;

    _doPwmFrequencyComboBox->setObjId(IndexDb401::getObjectId(IndexDb401::OD_MS_DO_PWM_FREQUENCY, _channel + 1));
    _doPwmFrequencyComboBox->setNode(_node);
}

void P401OutputOptionWidget::createWidgets()
{
    QFormLayout *formLayout = new QFormLayout();

    _doPwmFrequencyComboBox = new IndexComboBox();
    _doPwmFrequencyComboBox->addItem(tr("50 kHz"), QVariant(static_cast<uint16_t>(0x0000)));
    _doPwmFrequencyComboBox->addItem(tr("32 kHz"), QVariant(static_cast<uint16_t>(0x0001)));
    _doPwmFrequencyComboBox->addItem(tr("25 kHz"), QVariant(static_cast<uint16_t>(0x0002)));
    _doPwmFrequencyComboBox->addItem(tr("10 kHz"), QVariant(static_cast<uint16_t>(0x0003)));
    _doPwmFrequencyComboBox->addItem(tr("5 kHz"), QVariant(static_cast<uint16_t>(0x0004)));
    _doPwmFrequencyComboBox->addItem(tr("2 kHz"), QVariant(static_cast<uint16_t>(0x0005)));
    _doPwmFrequencyComboBox->addItem(tr("1 kHz"), QVariant(static_cast<uint16_t>(0x0006)));
    _doPwmFrequencyComboBox->addItem(tr("500 Hz"), QVariant(static_cast<uint16_t>(0x0007)));
    formLayout->addRow(tr("Pwm &frequency"), _doPwmFrequencyComboBox);

    setLayout(formLayout);
}
