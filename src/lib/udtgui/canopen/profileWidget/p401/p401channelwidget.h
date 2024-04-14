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

#ifndef P401CHANNELWIDGET_H
#define P401CHANNELWIDGET_H

#include "../../../udtgui_global.h"

#include "nodeobjectid.h"
#include "nodeodsubscriber.h"

#include <QLabel>
#include <QStackedWidget>
#include <QWidget>

class Node;

class IndexComboBox;
class P401InputWidget;
class P401OutputWidget;
class P401InputOptionWidget;
class P401OutputOptionWidget;

class P401ChannelWidget : public QWidget
{
    Q_OBJECT

public:
    P401ChannelWidget(uint8_t channel, QWidget *parent = nullptr);

    uint8_t channel() const;

    void readAllObject();
    void readInputObject();

    P401InputWidget *inputWidget() const;

public slots:
    void setNode(Node *node);
    void setSettings(bool checked);

private:
    uint8_t _channel;

    // Children widgets
    void createWidgets();
    IndexComboBox *_modeCombobox;

    P401InputOptionWidget *_inputOptionWidget;
    P401InputWidget *_inputWidget;

    P401OutputOptionWidget *_outputOptionWidget;
    P401OutputWidget *_outputWidget;

    QStackedWidget *_inputStackedWidget;
    QStackedWidget *_outputStackedWidget;
};

#endif  // P401CHANNELWIDGET_H
