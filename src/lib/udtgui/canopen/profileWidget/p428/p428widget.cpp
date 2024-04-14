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

#include "p428widget.h"

#include <QScrollArea>
#include <QVBoxLayout>

#include "p428channelwidget.h"

P428Widget::P428Widget(uint8_t channelCount, QWidget *parent)
    : QWidget{parent}
{
    _node = nullptr;
    _channelCount = channelCount;

    createWidgets();
}

Node *P428Widget::node() const
{
    return _node;
}

void P428Widget::setNode(Node *node)
{
    _node = node;
    for (P428ChannelWidget *channelWidget : _channelWidgets)
    {
        channelWidget->setNode(node);
    }
}

void P428Widget::readAllObject()
{
    for (P428ChannelWidget *channelWidget : qAsConst(_channelWidgets))
    {
        channelWidget->readAllObject();
    }
}

void P428Widget::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget *channelWidget = new QWidget(this);
    QGridLayout *channelLayout = new QGridLayout();
    channelLayout->setContentsMargins(0, 0, 0, 0);
    for (uint channel = 0; channel < _channelCount; channel++)
    {
        P428ChannelWidget *channelWidget = new P428ChannelWidget(channel);
        channelLayout->addWidget(channelWidget, channel / 5, channel % 5);
        _channelWidgets.append(channelWidget);
    }
    channelWidget->setLayout(channelLayout);

    QScrollArea *channelScrollArea = new QScrollArea();
    channelScrollArea->setWidget(channelWidget);
    channelScrollArea->setWidgetResizable(true);

    layout->addWidget(channelScrollArea);
    setLayout(layout);
}
