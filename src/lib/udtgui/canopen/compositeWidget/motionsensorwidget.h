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

#ifndef MOTIONSENSORWIDGET_H
#define MOTIONSENSORWIDGET_H

#include "../../udtgui_global.h"

#include "profile/p402/nodeprofile402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTimer>
#include <QToolBar>
#include <QWidget>

class Node;
class DataLogger;
class DataLoggerChartsWidget;
class IndexSpinBox;
class IndexLabel;

class UDTGUI_EXPORT MotionSensorWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MotionSensorWidget(QWidget *parent = nullptr);

    Node *node() const;
    QString title() const;

    enum ModeSensor
    {
        MODE_SENSOR_NONE,
        MODE_SENSOR_VELOCITY,
        MODE_SENSOR_POSITION,
        MODE_SENSOR_TORQUE,
    };

public slots:
    void setNode(Node *node, uint8_t axis = 0);
    void setMode(MotionSensorWidget::ModeSensor mode);

protected slots:
    void toggleStartLogger(bool start);
    void setLogTimer(int ms);

protected:
    Node *_node;
    uint8_t _axis;
    ModeSensor _mode;

    DataLogger *_dataLogger;
    DataLoggerChartsWidget *_dataLoggerChartsWidget;

    QToolBar *_toolBar;
    QSpinBox *_logTimerSpinBox;
    QAction *_startStopAction;

    QGroupBox *sensorConfigGroupBox;

    IndexSpinBox *_sensorSelectSpinBox;
    IndexSpinBox *_thresholdMinSpinBox;
    IndexSpinBox *_thresholdMaxSpinBox;
    IndexSpinBox *_thresholdModeSpinBox;
    IndexSpinBox *_preOffsetSpinBox;
    IndexSpinBox *_scaleSpinBox;
    IndexSpinBox *_postOffsetSpinBox;
    IndexSpinBox *_errorMinSpinBox;
    IndexSpinBox *_errorMaxSpinBox;
    IndexSpinBox *_frequencyDividerSpinBox;

    IndexLabel *_rawDataValueLabel;
    IndexLabel *_flagLabel;
    IndexLabel *_valueLabel;

    IndexSpinBox *_filterSelect;
    IndexSpinBox *_param0;
    IndexSpinBox *_param1;
    IndexSpinBox *_param2;
    IndexSpinBox *_param3;

    NodeProfile402 *_nodeProfile402;

    enum State
    {
        NONE,
        LAUCH_DATALOGGER,
        LAUCH_FIRST_TARGET,
        LAUCH_SECOND_TARGET,
        STOP_DATALOGGER,
    };

    State _state;

    void setIMode();
    void createWidgets();
    void statusNodeChanged(Node::Status status);
};

#endif // MOTIONSENSORWIDGET_H
