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

#ifndef P402MODEWIDGET_H
#define P402MODEWIDGET_H

#include "../../../udtgui_global.h"

#include <QWidget>
#include "nodeodsubscriber.h"

class UDTGUI_EXPORT P402ModeWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P402ModeWidget(QWidget *parent);

    Node *node() const;
    uint8_t axis() const;

    virtual void readRealTimeObjects();
    virtual void readAllObjects() = 0;
    virtual void reset();
    virtual void stop();

public slots:
    virtual void setNode(Node *value, uint8_t axis = 0) = 0;

protected:
    Node *_node;
    uint8_t _axis;
};

#endif // P402MODEWIDGET_H
