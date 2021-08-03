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

#include "p401outputwidget.h"

#include "indexdb401.h"
#include "canopen/widget/indexcheckbox.h"

#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QPushButton>
#include <QSlider>

P401OutputWidget::P401OutputWidget(uint8_t channel, QWidget *parent)
    : QWidget(parent)
{
    _channel = channel;
    createWidgets();
}

void P401OutputWidget::readAllObject()
{
    _node->readObject(_analogObjectId);
    _digitalCheckBox->readObject();
}

void P401OutputWidget::setNode(Node *node)
{
    if (!node)
    {
        return;
    }
    _node = node;

    _analogObjectId = IndexDb401::getObjectId(IndexDb401::AO_VALUES_16BITS, _channel + 1);
    _analogObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_analogObjectId);

    _modeObjectId = IndexDb401::getObjectId(IndexDb401::OD_MS_DO_MODE, _channel + 1);
    _modeObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_modeObjectId);

    setNodeInterrest(_node);

    _digitalCheckBox->setNode(_node);
    _digitalCheckBox->setObjId(IndexDb401::getObjectId(IndexDb401::DO_VALUES_8BITS_CHANNELS_0_7));
    _digitalCheckBox->setBitMask(1 << _channel);
}

void P401OutputWidget::analogSliderChanged()
{
    qint16 value = static_cast<qint16>(_analogSlider->value());
    _node->writeObject(_analogObjectId, QVariant(value));
}

void P401OutputWidget::analogSpinboxFinished()
{
    qint16 value = static_cast<qint16>(_analogSpinBox->value());
    _node->writeObject(_analogObjectId, QVariant(value));
}

void P401OutputWidget::setZeroButton()
{
    uint8_t value = 0;
    _node->writeObject(_analogObjectId, value);
}

void P401OutputWidget::createWidgets()
{
    _digitalCheckBox = new IndexCheckBox();
    _digitalCheckBox->setText("On/Off");

    _analogSpinBox = new QDoubleSpinBox();
    _analogSpinBox->setRange(0, 120);
    connect(_analogSpinBox, &QDoubleSpinBox::editingFinished, this, &P401OutputWidget::analogSpinboxFinished);

    QHBoxLayout *sliderLayout = new QHBoxLayout();
    sliderLayout->setContentsMargins(0, 0, 0, 0);
    _analogSlider = new QSlider(Qt::Horizontal);
    _analogSlider->setRange(0, 12);
    connect(_analogSlider, &QSlider::valueChanged, this, &P401OutputWidget::analogSliderChanged);
    sliderLayout->addWidget(_analogSlider);

    _setZeroButton = new QPushButton();
    _setZeroButton->setText("Set to 0");
    connect(_setZeroButton, &QPushButton::clicked, this, &P401OutputWidget::setZeroButton);
    sliderLayout->addWidget(_setZeroButton);

    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(0, 0, 0, 0);
    vBoxLayout->addWidget(_analogSpinBox);
    vBoxLayout->addLayout(sliderLayout);
    vBoxLayout->addWidget(_digitalCheckBox);

    setLayout(vBoxLayout);
}

void P401OutputWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if (objId == _analogObjectId)
    {
        int value = _node->nodeOd()->value(objId).toInt();
        if (!_analogSpinBox->hasFocus())
        {
            _analogSpinBox->blockSignals(true);
            _analogSpinBox->setValue(value);
            _analogSpinBox->blockSignals(false);
        }
        if (!_analogSlider->isSliderDown())
        {
            _analogSlider->blockSignals(true);
            _analogSlider->setValue(value);
            _analogSlider->blockSignals(false);
        }
    }

    if (objId == _modeObjectId)
    {
        uint16_t value = static_cast<uint16_t>(_node->nodeOd()->value(objId).toUInt());
        if (value == 0x0000)
        {
            _analogSlider->setEnabled(false);
            _analogSpinBox->setEnabled(false);
            _setZeroButton->setEnabled(false);
            _digitalCheckBox->setEnabled(false);
        }
        else if ((value == 0x0001) || (value == 0x0001) || (value == 0x0002) || (value == 0x0003))
        {
            _analogSlider->setEnabled(false);
            _analogSpinBox->setEnabled(false);
            _setZeroButton->setEnabled(false);
            _digitalCheckBox->setEnabled(true);
        }
        else
        {
            _analogSlider->setEnabled(true);
            _analogSpinBox->setEnabled(true);
            _setZeroButton->setEnabled(true);
            _digitalCheckBox->setEnabled(false);
        }
    }
}
