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

#include "p401inputwidget.h"

#include "indexdb401.h"
#include "canopen/widget/indexcheckbox.h"

#include <QFormLayout>
#include <QLabel>

P401InputWidget::P401InputWidget(uint8_t channel, QWidget *parent)
    : QWidget(parent)
{
    _channel = channel;
    createWidgets();
}

void P401InputWidget::readAllObject()
{
    _node->readObject(_analogObjectId);
    _node->readObject(_digitalObjectId);
}

void P401InputWidget::setNode(Node *node, uint8_t _channel)
{
    if (!node)
    {
        return;
    }
    _node = node;

    _analogObjectId = IndexDb401::getObjectId(IndexDb401::AI_VALUES_16BITS, _channel + 1);
    _analogObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_analogObjectId);

    _digitalObjectId = (IndexDb401::getObjectId(IndexDb401::DI_VALUES_8BITS_CHANNELS_0_7));
    _digitalObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_digitalObjectId);
    setNodeInterrest(_node);
}

void P401InputWidget::update()
{
    int value = _node->nodeOd()->value(_analogObjectId).toInt();
    _analogLcd->display(value);
    _analogProgressBar->setValue(value);

    uint8_t valueDigital = static_cast<uint8_t>(_node->nodeOd()->value(_digitalObjectId).toUInt());
    bool act = valueDigital >> _channel;

    if (act)
    {
        _digitalLabel->setText("On");
        _digitalLabel->setStyleSheet("background-color: #1464A0;border: 1px solid #32414B;color: #F0F0F0;border-radius: 4px;padding: 0px; marging: 2px");
    }
    else
    {
        _digitalLabel->setText("Off");
        _digitalLabel->setStyleSheet("background-color: #19232D;border: 1px solid #32414B;color: #F0F0F0;border-radius: 4px;padding: 0px; marging: 2px");
    }
}

void P401InputWidget::createWidgets()
{
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 0);

    _analogLcd = new QLCDNumber();
    _analogLcd->display(QString::number(0, 10, 2));
    hlayout->addWidget(_analogLcd);

    QLabel *volt = new QLabel("V");
    hlayout->addWidget(volt);

    _digitalLabel = new QLabel();
    _digitalLabel->setStyleSheet("background-color: #19232D;border: 1px solid #32414B;color: #F0F0F0;border-radius: 4px;padding: 0px; marging: 2px");
    _digitalLabel->setSizePolicy( QSizePolicy::Fixed, QSizePolicy::Fixed);

    hlayout->addWidget(_digitalLabel);

    _analogProgressBar = new QProgressBar();
    _analogProgressBar->setRange(0, 12);
    _analogProgressBar->setFormat("%v");
    hlayout->addWidget(_analogProgressBar);

    setLayout(hlayout);
}

void P401InputWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if (objId == _analogObjectId)
    {
        update();
    }
}
