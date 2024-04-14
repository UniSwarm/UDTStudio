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

#include "p401widget.h"

#include "canopen/datalogger/dataloggerwidget.h"
#include "node.h"
#include "p401channelwidget.h"
#include "p401inputwidget.h"

#include <QFormLayout>
#include <QFrame>
#include <QLabel>
#include <QLayout>
#include <QScrollArea>

P401Widget::P401Widget(uint8_t channelCount, QWidget *parent)
    : QWidget(parent)
{
    _channelCount = channelCount;
    createWidgets();

    connect(&_readTimer, &QTimer::timeout, this, &P401Widget::readInputObject);
}

Node *P401Widget::node() const
{
    return _node;
}

void P401Widget::readAllObject()
{
    for (P401ChannelWidget *p401ChannelWidget : qAsConst(_p401ChannelWidgets))
    {
        p401ChannelWidget->readAllObject();
    }
}

void P401Widget::readInputObject()
{
    for (P401ChannelWidget *p401ChannelWidget : qAsConst(_p401ChannelWidgets))
    {
        p401ChannelWidget->readInputObject();
    }
}

void P401Widget::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);

    for (P401ChannelWidget *p401ChannelWidget : qAsConst(_p401ChannelWidgets))
    {
        dataLogger->addData(p401ChannelWidget->inputWidget()->analogObjectId());
    }

    _dataLoggerWidget->show();
}

void P401Widget::start(int msec)
{
    _readTimer.start(msec);
}

void P401Widget::stop()
{
    _readTimer.stop();
}

void P401Widget::setNode(Node *node)
{
    if (node == nullptr)
    {
        return;
    }
    _node = node;

    for (P401ChannelWidget *p401ChannelWidget : qAsConst(_p401ChannelWidgets))
    {
        p401ChannelWidget->setNode(_node);
    }
}

void P401Widget::setSettings(bool checked)
{
    emit settings(checked);
}

void P401Widget::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    QLabel *laneLabel = new QLabel(tr("Channel"));
    QLabel *inputLabel = new QLabel(tr("Input"));
    QLabel *outputLabel = new QLabel(tr("Output"));

    QHBoxLayout *commun = new QHBoxLayout();
    commun->setContentsMargins(0, 0, 0, 0);
    commun->addWidget(laneLabel);
    commun->addWidget(inputLabel);
    commun->addWidget(outputLabel);

    layout->addItem(commun);

    QWidget *p401Widget = new QWidget(this);
    QVBoxLayout *channelLayout = new QVBoxLayout(p401Widget);
    channelLayout->setContentsMargins(0, 0, 0, 0);

    for (uint8_t i = 0; i < _channelCount; i++)
    {
        QFrame *frame = new QFrame();
        frame->setFrameStyle(QFrame::HLine);
        frame->setFrameShadow(QFrame::Sunken);
        channelLayout->addWidget(frame);
        _p401ChannelWidgets.append(new P401ChannelWidget(i, this));
        channelLayout->addWidget(_p401ChannelWidgets.at(i));
        connect(this, &P401Widget::settings, _p401ChannelWidgets.at(i), &P401ChannelWidget::setSettings);
    }

    QScrollArea *channelScrollArea = new QScrollArea();
    channelScrollArea->setWidget(p401Widget);
    channelScrollArea->setWidgetResizable(true);

    layout->addWidget(channelScrollArea);
    setLayout(layout);
}
