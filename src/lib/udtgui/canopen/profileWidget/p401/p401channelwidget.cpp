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

#include "p401channelwidget.h"

#include "indexdb401.h"
#include "p401inputwidget.h"
#include "p401outputwidget.h"
#include "p401optionwidget.h"
#include "canopen/widget/indexcombobox.h"

#include <QComboBox>
#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLayout>

P401ChannelWidget::P401ChannelWidget(uint8_t channel, QWidget *parent)
    : QWidget(parent)
{
    _channel = channel;
    createWidgets();
    _stackedWidget->setCurrentWidget(_inputWidget);
}

uint8_t P401ChannelWidget::channel() const
{
    return _channel;
}

void P401ChannelWidget::readAllObject()
{
    _modeCombobox->readObject();

    if (_stackedWidget->currentWidget() == _inputWidget)
    {
        _inputWidget->readAllObject();
    }
    else
    {
        _optionWidget->readAllObject();
    }

    _outputWidget->readAllObject();
}

void P401ChannelWidget::setNode(Node *node)
{
    _modeCombobox->setObjId(IndexDb401::getObjectId(IndexDb401::OD_MS_DO_MODE, _channel + 1));

    _modeCombobox->setNode(node);
    _inputWidget->setNode(node);
    _outputWidget->setNode(node);
    _optionWidget->setNode(node);
}

void P401ChannelWidget::displayOption401()
{
    if (_stackedWidget->currentWidget() == _inputWidget)
    {
        _stackedWidget->setCurrentWidget(_optionWidget);
    }
    else
    {
        _stackedWidget->setCurrentWidget(_inputWidget);
    }
}

void P401ChannelWidget::createWidgets()
{
    QVBoxLayout *channelLayout = new QVBoxLayout();
    channelLayout->setContentsMargins(0, 0, 0, 0);

    QLabel *channelLabel = new QLabel(tr("Channel ") + QString("%1").arg(QString::number(_channel, 10)));
    channelLayout->addWidget(channelLabel);

    _modeCombobox = new IndexComboBox();
    _modeCombobox->addItem(tr("No output"), QVariant(static_cast<uint16_t>(0x0000)));
    _modeCombobox->addItem(tr("Open-drain"), QVariant(static_cast<uint16_t>(0x0001)));
    _modeCombobox->addItem(tr("Open-source"), QVariant(static_cast<uint16_t>(0x0002)));
    _modeCombobox->addItem(tr("Push-Pull"), QVariant(static_cast<uint16_t>(0x0003)));
    _modeCombobox->addItem(tr("PWM Open-drain"), QVariant(static_cast<uint16_t>(0x0011)));
    _modeCombobox->addItem(tr("PWM Open-source"), QVariant(static_cast<uint16_t>(0x0012)));
    _modeCombobox->addItem(tr("PWM Push-Pull"), QVariant(static_cast<uint16_t>(0x0013)));
    channelLayout->addWidget(_modeCombobox);

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::VLine);
    frame->setFrameShadow(QFrame::Sunken);

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    _optionWidget= new P401OptionWidget(_channel, this);
    _inputWidget = new P401InputWidget(_channel, this);
    _outputWidget = new P401OutputWidget(_channel, this);

    layout->addItem(channelLayout);
    layout->addWidget(frame);

    // Stacked Widget
    _stackedWidget = new QStackedWidget;
    _stackedWidget->addWidget(_inputWidget);
    _stackedWidget->addWidget(_optionWidget);

    layout->addWidget(_stackedWidget);

    frame = new QFrame();
    frame->setFrameStyle(QFrame::VLine);
    frame->setFrameShadow(QFrame::Sunken);
    layout->addWidget(frame);

    layout->addWidget(_outputWidget);

    setLayout(layout);
}
