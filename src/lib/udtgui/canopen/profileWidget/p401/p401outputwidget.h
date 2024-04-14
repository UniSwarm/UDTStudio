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

#ifndef P401OUTPUTWIDGET_H
#define P401OUTPUTWIDGET_H

#include "../../../udtgui_global.h"

#include "node.h"
#include "nodeobjectid.h"
#include "nodeodsubscriber.h"

#include <QWidget>

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QStackedWidget>

class Node;
class NodeObjectId;

class P401OutputWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P401OutputWidget(uint8_t channel, QWidget *parent = nullptr);

    void readAllObject();

public slots:
    void setNode(Node *node);

private slots:
    void analogSliderChanged();
    void analogSpinboxFinished();
    void digitalPushButtonClicked(bool clicked);

private:
    Node *_node;
    uint8_t _channel;

    NodeObjectId _analogObjectId;
    NodeObjectId _digitalObjectId;
    NodeObjectId _modeObjectId;

    QPushButton *_digitalPushButton;
    QWidget *_digitalWidget;

    QSpinBox *_analogSpinBox;
    QSlider *_analogSlider;
    QCheckBox *_typeCheckBox;
    QPushButton *_setZeroButton;
    QWidget *_analogWidget;

    QLabel *_outputLabel;

    QStackedWidget *_stackedWidget;

    void setZeroButton();
    void typeCheckBoxClicked(bool checked);

    // Create widgets
    void createWidgets();
    QWidget *analogWidgets();
    QWidget *digitalWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif  // P401OUTPUTWIDGET_H
