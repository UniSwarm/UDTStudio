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

#include "p401inputoptionwidget.h"

#include "indexdb401.h"
#include "canopen/widget/indexspinbox.h"

#include <QFormLayout>
#include <QLabel>
P401InputOptionWidget::P401InputOptionWidget(uint8_t channel, QWidget *parent)
    : QWidget(parent)
    , _channel(channel)
{
    createWidgets();
}

void P401InputOptionWidget::readAllObject()
{
    _diSchmittTriggersHigh->readObject();
    _diSchmittTriggersHigh->readObject();
}

void P401InputOptionWidget::setNode(Node *node)
{
    if (!node)
    {
        return;
    }
    _node = node;

    _diSchmittTriggersHigh->setObjId(IndexDb401::getObjectId(IndexDb401::OD_MS_DI_SCHMITT_TRIGGERS_HIGH, _channel + 1));
    _diSchmittTriggersHigh->setNode(_node);

    _diSchmittTriggersLow->setObjId(IndexDb401::getObjectId(IndexDb401::OD_MS_DI_SCHMITT_TRIGGERS_LOW, _channel + 1));
    _diSchmittTriggersLow->setNode(_node);
}

void P401InputOptionWidget::createWidgets()
{
    QFormLayout *formLayout = new QFormLayout();

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);

    _diSchmittTriggersLow = new IndexSpinBox();
    _diSchmittTriggersLow->setScale(12.0 / std::numeric_limits<qint16>::max());
    hLayout->addWidget(_diSchmittTriggersLow);
    QLabel *label = new QLabel(tr(":"));
    label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    hLayout->addWidget(label);

    _diSchmittTriggersHigh = new IndexSpinBox();
    _diSchmittTriggersHigh->setScale(12.0 / std::numeric_limits<qint16>::max());
    hLayout->addWidget(_diSchmittTriggersHigh);
    label = new QLabel(tr("&Schmitt Triggers:"));
    label->setToolTip(tr("Min, Max"));
    label->setBuddy(_diSchmittTriggersHigh);
    formLayout->addRow(label, hLayout);

    setLayout(formLayout);
}
