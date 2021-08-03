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

#ifndef P401INPUTWIDGET_H
#define P401INPUTWIDGET_H

#include "../../../udtgui_global.h"

#include "node.h"
#include "nodeobjectid.h"
#include "nodeodsubscriber.h"

#include <QWidget>

#include <QLCDNumber>
#include <QProgressBar>
#include <QSlider>

class Node;
class IndexCheckBox;
class NodeObjectId;

class P401InputWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P401InputWidget(uint8_t channel, QWidget *parent = nullptr);

    void readAllObject();

public slots:
    void setNode(Node *node, uint8_t _channel = 0);

private:
    Node *_node;
    uint8_t _channel;

    NodeObjectId _analogObjectId;
    IndexCheckBox *_digitalCheckBox;

    void update();

    QLCDNumber *_analogLcd;
    QProgressBar *_analogProgressBar;

    // Create widgets
    void createWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P401INPUTWIDGET_H
