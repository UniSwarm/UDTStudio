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

#include <QWidget>

#include "profile/p402/nodeprofile402.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTimer>
#include <QToolBar>

class Node;
class DataLogger;
class DataLoggerWidget;
class IndexSpinBox;
class IndexLabel;
class IndexComboBox;
class IndexCheckBox;
class AbstractIndexWidget;

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
    void setLogTimer(int ms);

    void statusNodeChanged(Node::Status status);
    void stateChanged();
    void readAllObject();

    void updateSensorParams(int index);
    void updateFilterParams(int index);

protected:
    Node *_node;
    uint8_t _axis;
    ModeSensor _mode;
    NodeProfile402 *_nodeProfile402;

    DataLogger *_dataLogger;
    DataLoggerWidget *_dataLoggerWidget;

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

    // Creation widgets
    void createWidgets();
    QToolBar *createToolBarWidgets();
    QSpinBox *_logTimerSpinBox;

    QGroupBox *createInformationWidgets();
    QLabel *_informationLabel;
    QPushButton *_enableButton;

    QGroupBox *createSensorConfigurationWidgets();
    QGroupBox *_sensorConfigGroupBox;
    IndexComboBox *_sensorSelectComboBox;
    IndexSpinBox *_frequencyDividerSpinBox;
    IndexCheckBox *_configBitCheckBox;
    IndexSpinBox *_sensorParam0SpinBox;
    IndexSpinBox *_sensorParam1SpinBox;
    IndexSpinBox *_sensorParam2SpinBox;
    IndexSpinBox *_sensorParam3SpinBox;
    QList<QLabel *> _sensorParamLabels;

    QGroupBox *createSensorFilterWidgets();
    QGroupBox *_filterGroupBox;
    IndexComboBox *_filterSelectComboBox;
    IndexSpinBox *_filterParam0SpinBox;
    IndexSpinBox *_filterParam1SpinBox;
    IndexSpinBox *_filterParam2SpinBox;
    IndexSpinBox *_filterParam3SpinBox;
    QList<QLabel *> _filterParamLabels;

    QGroupBox *createSensorConditioningWidgets();
    QGroupBox *_conditioningGroupBox;
    IndexSpinBox *_thresholdMinSpinBox;
    IndexSpinBox *_thresholdMaxSpinBox;
    IndexComboBox *_thresholdModeComboBox;
    IndexSpinBox *_preOffsetSpinBox;
    IndexSpinBox *_scaleSpinBox;
    IndexSpinBox *_postOffsetSpinBox;
    IndexSpinBox *_errorMinSpinBox;
    IndexSpinBox *_errorMaxSpinBox;

    QGroupBox *createSensorStatusWidgets();
    QGroupBox *_statusGroupBox;
    IndexLabel *_rawDataValueLabel;
    IndexLabel *_flagLabel;
    IndexLabel *_valueLabel;

    QList<AbstractIndexWidget *> _indexWidgets;

    void goEnableButton();
};

#endif  // MOTIONSENSORWIDGET_H
