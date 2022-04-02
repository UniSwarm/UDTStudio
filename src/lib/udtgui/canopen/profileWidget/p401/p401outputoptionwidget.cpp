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

#include "canopen/indexWidget/indexspinbox.h"
#include "indexdb401.h"

#include <QFormLayout>

P401OutputOptionWidget::P401OutputOptionWidget(uint8_t channel, QWidget *parent)
    : QWidget(parent)
    , _channel(channel)
{
    createWidgets();
}

void P401OutputOptionWidget::readAllObject()
{
    _doPwmFrequency->readObject();
}

void P401OutputOptionWidget::setNode(Node *node)
{
    if (node == nullptr)
    {
        return;
    }
    _node = node;

    _doPwmFrequency->setObjId(IndexDb401::getObjectId(IndexDb401::OD_MS_DO_PWM_FREQUENCY, _channel + 1));
    _doPwmFrequency->setNode(_node);
}

void P401OutputOptionWidget::createWidgets()
{
    QFormLayout *formLayout = new QFormLayout();

    _doPwmFrequency = new IndexSpinBox();
    formLayout->addRow(tr("Pwm Frequency"), _doPwmFrequency);

    setLayout(formLayout);
}
