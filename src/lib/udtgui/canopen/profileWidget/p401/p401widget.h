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

#ifndef P401WIDGET_H
#define P401WIDGET_H

#include "../../../udtgui_global.h"

#include <QTimer>
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

    void start(int msec);
    void stop();

public slots:
    void setNode(Node *node);
    void setSettings(bool checked);
    void readAllObject();
    void readInputObject();
    void dataLogger();

signals:
    void settings(bool checked);

private:
    quint8 _channelCount;
    Node *_node;

    QTimer _readTimer;

    QList<P401ChannelWidget *> _p401ChannelWidgets;

    // Create widgets
    void createWidgets();
    QToolBar *toolBarWidgets();
};

#endif  // P401WIDGET_H
