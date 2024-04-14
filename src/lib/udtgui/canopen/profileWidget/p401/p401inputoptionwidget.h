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

#ifndef P401INPUTOPTIONWIDGET_H
#define P401INPUTOPTIONWIDGET_H

#include <QWidget>

class IndexSpinBox;
class Node;

class P401InputOptionWidget : public QWidget
{
    Q_OBJECT
public:
    P401InputOptionWidget(uint8_t channel, QWidget *parent = nullptr);

    void readAllObject();

public slots:
    void setNode(Node *node);

private:
    uint8_t _channel;
    Node *_node;

    IndexSpinBox *_diSchmittTriggersHigh;
    IndexSpinBox *_diSchmittTriggersLow;

    // Create widgets
    void createWidgets();
};

#endif  // P401INPUTOPTIONWIDGET_H
