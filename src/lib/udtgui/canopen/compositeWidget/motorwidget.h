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

#ifndef MOTORCONFIG_H
#define MOTORCONFIG_H

#include "../../udtgui_global.h"

#include "node.h"

#include <QGroupBox>
#include <QSpinBox>
#include <QToolBar>
#include <QWidget>

class Node;
class DataLogger;
class DataLoggerChartsWidget;
class IndexSpinBox;
class IndexLabel;
class IndexComboBox;

class UDTGUI_EXPORT MotorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MotorWidget(QWidget *parent = nullptr);

    Node *node() const;
    QString title() const;

public slots:
    void setNode(Node *node);

protected:
    Node *_node;

    IndexComboBox *_motorTypeComboBox;
    IndexSpinBox *_peakCurrent;
    IndexSpinBox *_polePair;
    IndexSpinBox *_maxVelocity;
    IndexSpinBox *_velocityConstant;
    IndexSpinBox *_currentConstant;

    IndexLabel *_coderLabel;
    IndexLabel *_timeCoderLabel;
    IndexLabel *_phaseLabel;
    IndexLabel *_bridgePwmLabel;

    void statusNodeChanged(Node::Status status);
    void readAllObject();

    QToolBar *toolBarWidgets();
    QGroupBox *motorConfigWidgets();
    QGroupBox *motorStatusWidgets();
    void createWidgets();

};

#endif // MOTORCONFIG_H
