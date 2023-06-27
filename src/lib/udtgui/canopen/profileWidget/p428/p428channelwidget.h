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

#ifndef P428CHANNELWIDGET_H
#define P428CHANNELWIDGET_H

#include "../../../udtgui_global.h"

#include <QWidget>

class Node;

class AbstractIndexWidget;
class IndexComboBox;
class IndexSlider;
class IndexSpinBox;

class P428ChannelWidget : public QWidget
{
    Q_OBJECT
public:
    explicit P428ChannelWidget(uint8_t channel, QWidget *parent = nullptr);

    uint8_t channel() const;

    Node *node() const;
    void setNode(Node *node);

public slots:
    void readAllObject();

private:
    uint8_t _channel;
    Node *_node;

    // Children widgets
    void createWidgets();
    IndexSpinBox *_ledCountSpinBox;
    IndexComboBox *_effectComboBox;
    IndexSlider *_dimmerSlider;
    IndexSlider *_speedSlider;
    IndexSlider *_valueSlider;
    IndexSlider *_rSlider;
    IndexSlider *_gSlider;
    IndexSlider *_bSlider;

    QList<AbstractIndexWidget *> _indexWidgets;
};

#endif  // P428CHANNELWIDGET_H
