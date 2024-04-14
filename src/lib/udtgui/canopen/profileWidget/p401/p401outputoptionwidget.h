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

#ifndef P401OUTPUTOPTIONWIDGET_H
#define P401OUTPUTOPTIONWIDGET_H

#include <QWidget>

class IndexComboBox;
class Node;

class P401OutputOptionWidget : public QWidget
{
    Q_OBJECT
public:
    P401OutputOptionWidget(uint8_t channel, QWidget *parent = nullptr);
    void readAllObject();

public slots:
    void setNode(Node *node);

private:
    uint8_t _channel;
    Node *_node;

    IndexComboBox *_doPwmFrequencyComboBox;

    // Create widgets
    void createWidgets();
};

#endif  // P401OUTPUTOPTIONWIDGET_H
