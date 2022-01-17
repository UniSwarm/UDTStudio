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

#include "p428channelwidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QStandardItemModel>
#include <QVBoxLayout>

#include "canopen/indexWidget/indexspinbox.h"
#include "canopen/indexWidget/indexcombobox.h"
#include "canopen/indexWidget/indexslider.h"

P428ChannelWidget::P428ChannelWidget(uint8_t channel, QWidget *parent)
    : QWidget{parent}
{
    _channel = channel;

    createWidgets();
}

uint8_t P428ChannelWidget::channel() const
{
    return _channel;
}

void P428ChannelWidget::readAllObject()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->readObject();
    }
}

Node *P428ChannelWidget::node() const
{
    return _node;
}

void P428ChannelWidget::setNode(Node *node)
{
    _node = node;

    for (AbstractIndexWidget *indexWidget : _indexWidgets)
    {
        indexWidget->setNode(_node);
    }
}

void P428ChannelWidget::createWidgets()
{
    QVBoxLayout *vBoxLayout = new QVBoxLayout();
    vBoxLayout->setContentsMargins(2, 2, 2, 2);
    vBoxLayout->setSpacing(0);

    QGroupBox *ledStripGroupBox = new QGroupBox(tr("Led strip %1").arg(_channel + 1));
    QFormLayout *formLayout = new QFormLayout();
    formLayout->setVerticalSpacing(3);
    formLayout->setHorizontalSpacing(3);

    _ledCountSpinBox = new IndexSpinBox();
    _ledCountSpinBox->setObjId(NodeObjectId(0x6001 + 0x100 * _channel, 0));
    formLayout->addRow(tr("LEDs count:"), _ledCountSpinBox);
    _indexWidgets.append(_ledCountSpinBox);

    _effectComboBox = new IndexComboBox();
    _effectComboBox->setObjId(NodeObjectId(0x6020 + 0x100 * _channel, 0));
    _effectComboBox->addItem(tr("Fixed"));
    dynamic_cast<QStandardItemModel *>(_effectComboBox->model())->item(_effectComboBox->count() - 1)->setEnabled(false);
    _effectComboBox->addItem(tr("OFF"), QVariant(static_cast<uint16_t>(0x0000)));
    _effectComboBox->addItem(tr("Fixed color"), QVariant(static_cast<uint16_t>(0x0001)));
    _effectComboBox->insertSeparator(_effectComboBox->count());

    _effectComboBox->addItem(tr("Time based"));
    dynamic_cast<QStandardItemModel *>(_effectComboBox->model())->item(_effectComboBox->count() - 1)->setEnabled(false);
    _effectComboBox->addItem(tr("Flash"), QVariant(static_cast<uint16_t>(0x0101)));
    _effectComboBox->addItem(tr("Breathing"), QVariant(static_cast<uint16_t>(0x0102)));
    _effectComboBox->insertSeparator(_effectComboBox->count());

    _effectComboBox->addItem(tr("Movement"));
    dynamic_cast<QStandardItemModel *>(_effectComboBox->model())->item(_effectComboBox->count() - 1)->setEnabled(false);
    _effectComboBox->addItem(tr("Flow"), QVariant(static_cast<uint16_t>(0x0300)));
    _effectComboBox->addItem(tr("Reverse flow"), QVariant(static_cast<uint16_t>(0x0301)));
    formLayout->addRow(tr("Effect:"), _effectComboBox);
    _indexWidgets.append(_effectComboBox);

    _dimmerSlider = new IndexSlider();
    _dimmerSlider->setObjId(NodeObjectId(0x6025 + 0x100 * _channel, 0));
    _dimmerSlider->setRangeValue(0, 255);
    formLayout->addRow(tr("Dimmer:"), _dimmerSlider);
    _indexWidgets.append(_dimmerSlider);

    _speedSlider = new IndexSlider();
    _speedSlider->setObjId(NodeObjectId(0x6013 + 0x100 * _channel, 0));
    _speedSlider->setRangeValue(0, 255);
    formLayout->addRow(tr("Speed:"), _speedSlider);
    _indexWidgets.append(_speedSlider);

    _rSlider = new IndexSlider();
    _rSlider->setObjId(NodeObjectId(0x6030 + 0x100 * _channel, 1));
    _rSlider->setRangeValue(0, 255);
    formLayout->addRow(tr("R:"), _rSlider);
    _indexWidgets.append(_rSlider);

    _gSlider = new IndexSlider();
    _gSlider->setObjId(NodeObjectId(0x6030 + 0x100 * _channel, 2));
    _gSlider->setRangeValue(0, 255);
    formLayout->addRow(tr("G:"), _gSlider);
    _indexWidgets.append(_gSlider);

    _bSlider = new IndexSlider();
    _bSlider->setObjId(NodeObjectId(0x6030 + 0x100 * _channel, 3));
    _bSlider->setRangeValue(0, 255);
    formLayout->addRow(tr("B:"), _bSlider);
    _indexWidgets.append(_bSlider);

    ledStripGroupBox->setLayout(formLayout);
    vBoxLayout->addWidget(ledStripGroupBox);
    setLayout(vBoxLayout);
}
