/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include <QFormLayout>
#include <QLayout>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

P401OutputWidget::P401OutputWidget(uint8_t channel, QWidget *parent)
    : QWidget(parent)
{
    _channel = channel;
    createWidgets();

    _stackedWidget->setCurrentWidget(_outputLabel);
    _digitalWidget->setEnabled(false);
}

void P401OutputWidget::readAllObject()
{
    _node->readObject(_analogObjectId);
    _node->readObject(_digitalObjectId);
}

void P401OutputWidget::setNode(Node *node)
{
    if (node == nullptr)
    {
        return;
    }
    _node = node;

    _analogObjectId = IndexDb401::getObjectId(IndexDb401::AO_VALUES_16BITS, _channel + 1);
    _analogObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_analogObjectId);

    _digitalObjectId = IndexDb401::getObjectId(IndexDb401::DO_VALUES_8BITS_CHANNELS_0_7);
    _digitalObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_digitalObjectId);

    _modeObjectId = IndexDb401::getObjectId(IndexDb401::OD_MS_DO_MODE, _channel + 1);
    _modeObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_modeObjectId);

    setNodeInterrest(_node);
}

void P401OutputWidget::analogSliderChanged()
{
    qint16 value = static_cast<qint16>(_analogSlider->value());
    if (_typeCheckBox->isChecked())
    {
        if (value != 0)
        {
            value = (std::numeric_limits<qint16>::max() / 100) * value;
        }
        _node->writeObject(_analogObjectId, QVariant(value));
    }
    else
    {
        _node->writeObject(_analogObjectId, QVariant(value));
    }
}

void P401OutputWidget::analogSpinboxFinished()
{
    qint16 value = static_cast<qint16>(_analogSpinBox->value());
    if (_typeCheckBox->isChecked())
    {
        if (value != 0)
        {
            value = (std::numeric_limits<qint16>::max() / 100) * value;
        }
        _node->writeObject(_analogObjectId, QVariant(value));
    }
    else
    {
        _node->writeObject(_analogObjectId, QVariant(value));
    }
}

void P401OutputWidget::digitalPushButtonClicked(bool clicked)
{
    uint8_t valueDigital = static_cast<uint8_t>(_node->nodeOd()->value(_digitalObjectId).toUInt());
    uint8_t value = valueDigital & ~(1 << _channel);
    value |= (static_cast<int>(clicked) << _channel);
    _node->writeObject(_digitalObjectId, QVariant(value));

    if (clicked)
    {
        _digitalPushButton->setText(tr("On"));
    }
    else
    {
        _digitalPushButton->setText(tr("Off"));
    }
}

void P401OutputWidget::setZeroButton()
{
    uint8_t value = 0;
    _node->writeObject(_analogObjectId, value);
}

void P401OutputWidget::typeCheckBoxClicked(bool checked)
{
    int value = _node->nodeOd()->value(_analogObjectId).toInt();
    if (checked)
    {
        if (value != 0)
        {
            value = (value * 100) / std::numeric_limits<qint16>::max();
        }
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
            _analogSlider->setRange(0, 100);
            _analogSlider->blockSignals(false);
        }
        _analogSpinBox->setRange(0, 100);
    }
    else
    {
        _analogSpinBox->setRange(0, std::numeric_limits<qint16>::max());
        _analogSlider->setRange(0, std::numeric_limits<qint16>::max());
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
}

void P401OutputWidget::createWidgets()
{
    _analogWidget = analogWidgets();
    _digitalWidget = digitalWidgets();

    _stackedWidget = new QStackedWidget();
    _stackedWidget->addWidget(_analogWidget);
    _stackedWidget->addWidget(_digitalWidget);

    _outputLabel = new QLabel(tr("Output disabled"));
    _outputLabel->setDisabled(true);
    _stackedWidget->addWidget(_outputLabel);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(_stackedWidget);
    setLayout(layout);
}

QWidget *P401OutputWidget::analogWidgets()
{
    QWidget *widget = new QWidget();

    QGridLayout *gridLayout = new QGridLayout(widget);
    gridLayout->setContentsMargins(0, 0, 0, 0);

    _analogSpinBox = new QSpinBox();
    _analogSpinBox->setRange(0, 100);
    connect(_analogSpinBox, &QSpinBox::editingFinished, this, &P401OutputWidget::analogSpinboxFinished);

    _typeCheckBox = new QCheckBox();
    _typeCheckBox->setText(tr("Percent"));
    connect(_typeCheckBox, &QCheckBox::clicked, this, &P401OutputWidget::typeCheckBoxClicked);

    _analogSlider = new QSlider(Qt::Horizontal);
    _analogSlider->setRange(0, 100);
    connect(_analogSlider, &QSlider::valueChanged, this, &P401OutputWidget::analogSliderChanged);

    _setZeroButton = new QPushButton();
    _setZeroButton->setText(tr("Set to 0"));
    connect(_setZeroButton, &QPushButton::clicked, this, &P401OutputWidget::setZeroButton);

    gridLayout->addWidget(_analogSpinBox, 0, 0);
    gridLayout->addWidget(_typeCheckBox, 0, 1);
    gridLayout->addWidget(_analogSlider, 1, 0);
    gridLayout->addWidget(_setZeroButton, 1, 1);

    return widget;
}

QWidget *P401OutputWidget::digitalWidgets()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *digitalLayout = new QVBoxLayout(widget);

    _digitalPushButton = new QPushButton(tr("Off"));
    _digitalPushButton->setCheckable(true);
    _digitalPushButton->setStyleSheet(QStringLiteral("QPushButton:checked { background-color : #148CD2; }"));
    QIcon icon;
    icon.addFile(QStringLiteral(":/icons/img/icons8-stop.png"), QSize(), QIcon::Normal, QIcon::Off);
    icon.addFile(QStringLiteral(":/icons/img/icons8-play.png"), QSize(), QIcon::Normal, QIcon::On);
    _digitalPushButton->setIcon(icon);

    connect(_digitalPushButton, &QPushButton::clicked, this, &P401OutputWidget::digitalPushButtonClicked);

    digitalLayout->addWidget(_digitalPushButton);

    return widget;
}

void P401OutputWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if ((flags & NodeOd::FlagsRequest::Error) != 0)
    {
        return;
    }

    if (objId == _analogObjectId)
    {
        int value = _node->nodeOd()->value(objId).toInt();
        if (!_analogSpinBox->hasFocus())
        {
            if (_typeCheckBox->isChecked())
            {
                if (value != 0)
                {
                    value = (value * 100) / std::numeric_limits<qint16>::max();
                }
            }
            _analogSpinBox->blockSignals(true);
            _analogSpinBox->setValue(value);
            _analogSpinBox->blockSignals(false);
        }
        if (!_analogSlider->isSliderDown())
        {
            if (_typeCheckBox->isChecked())
            {
                if (value != 0)
                {
                    value = (value * 100) / std::numeric_limits<qint16>::max();
                }
            }

            _analogSlider->blockSignals(true);
            _analogSlider->setValue(value);
            _analogSlider->blockSignals(false);
        }
    }
    if (objId == _digitalObjectId)
    {
        uint8_t valueDigital = static_cast<uint8_t>(_node->nodeOd()->value(_digitalObjectId).toUInt());
        bool act = ((valueDigital >> _channel) & 1) != 0;
        _digitalPushButton->setEnabled(true);
        _digitalPushButton->setChecked(act);
    }

    if (objId == _modeObjectId)
    {
        uint16_t value = static_cast<uint16_t>(_node->nodeOd()->value(objId).toUInt());
        if (value == 0x0000)
        {
            _stackedWidget->setCurrentWidget(_outputLabel);
            _digitalWidget->setEnabled(false);
        }
        else if ((value == 0x0001) || (value == 0x0002) || (value == 0x0003))
        {
            _digitalWidget->setEnabled(true);
            _stackedWidget->setCurrentWidget(_digitalWidget);
        }
        else
        {
            _stackedWidget->setCurrentWidget(_analogWidget);
        }
    }
}
