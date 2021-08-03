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

#ifndef P401WIDGET_H
#define P401WIDGET_H

#include "../../../udtgui_global.h"

#include <QToolBar>
#include <QWidget>

class Node;
class P401ChannelWidget;

class UDTGUI_EXPORT P401Widget : public QWidget
{
    Q_OBJECT
public:
    P401Widget(uint8_t channelCount, QWidget *parent = nullptr);

    Node *node() const;

    void readAllObject();

public slots:
    void setNode(Node *node);

private:
    quint8 _channelCount;
    Node *_node;

    QList<P401ChannelWidget *> _p401ChannelWidgets;

    // Create widgets
    void createWidgets();
    QToolBar *toolBarWidgets();
};

#endif // P401WIDGET_H
