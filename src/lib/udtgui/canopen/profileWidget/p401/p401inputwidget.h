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

#ifndef P401INPUTWIDGET_H
#define P401INPUTWIDGET_H

#include "../../../udtgui_global.h"

#include "node.h"
#include "nodeobjectid.h"
#include "nodeodsubscriber.h"

#include <QWidget>

#include <QLabel>
#include <QProgressBar>
#include <QSlider>

class Node;
class IndexCheckBox;
class NodeObjectId;
class IndexLCDNumber;

class P401InputWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P401InputWidget(uint8_t channel, QWidget *parent = nullptr);

    void readAllObject();

    const NodeObjectId &analogObjectId() const;

public slots:
    void setNode(Node *node);

private:
    Node *_node;
    uint8_t _channel;
    IndexLCDNumber *_analogLcdNumber;
    NodeObjectId _analogObjectId;
    NodeObjectId _digitalObjectId;
    QLabel *_digitalLabel;

    QProgressBar *_analogProgressBar;

    // Create widgets
    void createWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif  // P401INPUTWIDGET_H
