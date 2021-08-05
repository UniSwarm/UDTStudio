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
#include "p401inputoptionwidget.h"
#include "p401outputoptionwidget.h"
#include "canopen/indexWidget/indexcombobox.h"

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
    _inputStackedWidget->setCurrentWidget(_inputWidget);
    _outputStackedWidget->setCurrentWidget(_outputWidget);
}

uint8_t P401ChannelWidget::channel() const
{
    return _channel;
}

void P401ChannelWidget::readAllObject()
{
    _modeCombobox->readObject();

    if (_inputStackedWidget->currentWidget() == _inputWidget)
    {
        _inputWidget->readAllObject();
        _outputWidget->readAllObject();
    }
    else
    {
        _inputOptionWidget->readAllObject();
        _outputOptionWidget->readAllObject();
    }
}

void P401ChannelWidget::readInputObject()
{
    _modeCombobox->readObject();

    if (_inputStackedWidget->currentWidget() == _inputWidget)
    {
        _inputWidget->readAllObject();
    }
    else
    {
        _inputOptionWidget->readAllObject();
    }
}

void P401ChannelWidget::setNode(Node *node)
{
    _modeCombobox->setObjId(IndexDb401::getObjectId(IndexDb401::OD_MS_DO_MODE, _channel + 1));
    _modeCombobox->setNode(node);

    _inputWidget->setNode(node);    
    _inputOptionWidget->setNode(node);

    _outputWidget->setNode(node);
    _outputOptionWidget->setNode(node);
}

void P401ChannelWidget::setSettings(bool checked)
{
    if (checked)
    {
        _inputStackedWidget->setCurrentWidget(_inputOptionWidget);
        _outputStackedWidget->setCurrentWidget(_outputOptionWidget);
    }
    else
    {
        _inputStackedWidget->setCurrentWidget(_inputWidget);
        _outputStackedWidget->setCurrentWidget(_outputWidget);
    }
}

P401InputWidget *P401ChannelWidget::inputWidget() const
{
    return _inputWidget;
}

void P401ChannelWidget::createWidgets()
{
    QVBoxLayout *channelLayout = new QVBoxLayout();
    channelLayout->setContentsMargins(0, 0, 0, 0);

    channelLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QLabel *channelLabel = new QLabel(QString("%1").arg(QString::number(_channel, 10)), this);
    channelLabel->setStyleSheet("font: bold 14px;font-size: 20px;");
    channelLabel->setGeometry(0, 0, 100, 25);

    _modeCombobox = new IndexComboBox();
    _modeCombobox->addItem(tr("Output disabled"), QVariant(static_cast<uint16_t>(0x0000)));
    _modeCombobox->addItem(tr("Open-drain"), QVariant(static_cast<uint16_t>(0x0001)));
    _modeCombobox->addItem(tr("Open-source"), QVariant(static_cast<uint16_t>(0x0002)));
    _modeCombobox->addItem(tr("Push-Pull"), QVariant(static_cast<uint16_t>(0x0003)));
    _modeCombobox->addItem(tr("PWM Open-drain"), QVariant(static_cast<uint16_t>(0x0011)));
    _modeCombobox->addItem(tr("PWM Open-source"), QVariant(static_cast<uint16_t>(0x0012)));
    _modeCombobox->addItem(tr("PWM Push-Pull"), QVariant(static_cast<uint16_t>(0x0013)));
    channelLayout->addWidget(_modeCombobox);

    channelLayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::Expanding));

    QHBoxLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addItem(channelLayout);

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::VLine);
    frame->setFrameShadow(QFrame::Sunken);
    layout->addWidget(frame);

    // Input Stacked Widget
    _inputOptionWidget= new P401InputOptionWidget(_channel, this);
    _inputWidget = new P401InputWidget(_channel, this);

    _inputStackedWidget = new QStackedWidget;
    _inputStackedWidget->addWidget(_inputWidget);
    _inputStackedWidget->addWidget(_inputOptionWidget);
    layout->addWidget(_inputStackedWidget);

    frame = new QFrame();
    frame->setFrameStyle(QFrame::VLine);
    frame->setFrameShadow(QFrame::Sunken);
    layout->addWidget(frame);

    // Output Stacked Widget
    _outputWidget = new P401OutputWidget(_channel, this);
    _outputOptionWidget = new P401OutputOptionWidget(_channel, this);

    _outputStackedWidget = new QStackedWidget;
    _outputStackedWidget->addWidget(_outputWidget);
    _outputStackedWidget->addWidget(_outputOptionWidget);
    layout->addWidget(_outputStackedWidget);

    setLayout(layout);
}
