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

#ifndef NODESCREEN_H
#define NODESCREEN_H

#include "udtgui_global.h"

#include "utils/nodewidget.h"

#include <node.h>

class NodeScreensWidget;

class UDTGUI_EXPORT NodeScreen : public NodeWidget
{
    Q_OBJECT
public:
    NodeScreen(QWidget *parent = nullptr);

    virtual QString title() const = 0;
    virtual QIcon icon() const;

    NodeScreensWidget *screenWidget() const;
    void setScreenWidget(NodeScreensWidget *screenWidget);

public slots:
    void setNode(Node *node, uint8_t axis = 0);

protected:
    virtual void setNodeInternal(Node *node, uint8_t axis = 0) = 0;

    NodeScreensWidget *_screenWidget;
};

#endif  // NODESCREEN_H
