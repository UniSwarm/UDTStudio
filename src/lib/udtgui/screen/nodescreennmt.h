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

#ifndef NODESCREENNMT_H
#define NODESCREENNMT_H

#include "udtgui_global.h"

#include "nodescreen.h"

class AbstractIndexWidget;

class UDTGUI_EXPORT NodeScreenNMT : public NodeScreen
{
    Q_OBJECT
public:
    NodeScreenNMT(QWidget *parent = nullptr);

protected:
    void createWidgets();
    QWidget *createProducerHeartBeatWidget();
    QWidget *createConsumerHeartBeatWidget();

    // NodeScreen interface
public:
    QString title() const override;
    void setNodeInternal(Node *node, uint8_t axis = 0) override;
};

#endif  // NODESCREENNMT_H
