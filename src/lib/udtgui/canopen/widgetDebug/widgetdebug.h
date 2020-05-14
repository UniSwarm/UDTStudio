/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#ifndef WIDGETDEBUG_H
#define WIDGETDEBUG_H

#include "../../udtgui_global.h"

#include "nodeodsubscriber.h"
#include <QWidget>

#include "node.h"
#include <QGroupBox>
#include <QLabel>
#include <QButtonGroup>
#include <QSpinBox>
#include <QToolBar>
#include <QSlider>

class WidgetDebug : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    WidgetDebug(QWidget *parent = nullptr);
    WidgetDebug(Node *node, QWidget *parent = nullptr);
    ~WidgetDebug() override;

    Node *node() const;

public slots:
    void setNode(Node *value);
    void updateData();

private:
    void createWidgets();
    void toggleStart(bool start);
    void setTimer(int ms);
    void readData();
    QTimer _timer;

    Node *_node;

    quint16 cmdControlWord;
    quint16 _controlWordObjectId;
    quint16 _statusWordObjectId;
    quint16 _haltObjectId;
    quint16 _quickStopObjectId;
    quint16 _abortConnectionObjectId;
    quint16 _faultReactionObjectId;

    // VL mode
    quint16 _vlVelocityDemandObjectId;
    quint16 _vlVelocityActualObjectId;

    QToolBar *_toolBar;
    QSpinBox *_logTimerSpinBox;
    QAction *_startStopAction;

    QLabel *_controlWordLabel;
    QLabel *_statusWordRawLabel;
    QLabel *_statusWordLabel;
    QLabel *_voltageEnabledLabel;
    QLabel *_warningLabel;
    QLabel *_remoteLabel;
    QLabel *_targetReachedLabel;
    QLabel *_internalLimitActiveLabel;
    QLabel *_operationModeSpecificLabel;
    QLabel *_manufacturerSpecificLabel;

    QButtonGroup *_stateMachineGroup;
    QButtonGroup *_haltOptionGroup;
    QButtonGroup *_quickStopOptionGroup;
    QButtonGroup *_abortConnectionOptionGroup;
    QButtonGroup *_faultReactionOptionGroup;

    void stateMachineClicked(int id);
    void haltOptionClicked(int id);
    void quickStopOptionClicked(int id);
    void abortConnectionOptionClicked(int id);
    void faultReactionOptionClicked(int id);

    void manageNotificationControlWordObject(SDO::FlagsRequest flags);
    void manageNotificationStatusWordobject();

    void setCheckableStateMachine(int id);
    void controlWordHaltClicked();

    // VL MODE
    QButtonGroup *_vlEnableRampButtonGroup;
    QButtonGroup *_vlUnlockRampButtonGroup;
    QButtonGroup *_vlReferenceRampButtonGroup;
    QButtonGroup *_vlHaltButtonGroup;
    QSpinBox *_vlTargetVelocitySpinBox;
    QSlider *_vlTargetVelocitySlider;
    QLabel *_vlVelocityDemandLabel;
    QLabel *_vlVelocityActualLabel;
    QSpinBox *_vlMinVelocityMinMaxAmountSpinBox;
    QSpinBox *_vlMaxVelocityMinMaxAmountSpinBox;
    QSpinBox *_vlAccelerationDeltaSpeedSpinBox;
    QSpinBox *_vlAccelerationDeltaTimeSpinBox;
    QSpinBox *_vlDecelerationDeltaSpeedDecelerationSpinBox;
    QSpinBox *_vlDecelerationDeltaTimeSpinBox;
    QSpinBox *_vlQuickStopDeltaSpeedSpinBox;
    QSpinBox *_vlQuickStopDeltaTimeSpinBox;
    QSpinBox *_vlSetPointFactorNumeratorSpinBox;
    QSpinBox *_vlSetPointFactorDenominatorSpinBox;
    QSpinBox *_vlDimensionFactorNumeratorSpinBox;
    QSpinBox *_vlDimensionFactorDenominatorSpinBox;

    void vlTargetVelocitySpinboxFinished();
    void vlTargetVelocitySliderChanged();
    void vlMinAmountEditingFinished();
    void vlMaxAmountEditingFinished();
    void vlAccelerationDeltaSpeedEditingFinished();
    void vlAccelerationDeltaTimeEditingFinished();
    void vlDecelerationDeltaSpeedEditingFinished();
    void vlDecelerationDeltaTimeEditingFinished();
    void vlQuickStopDeltaSpeedEditingFinished();
    void vlQuickStopDeltaTimeEditingFinished();
    void vlSetPointFactorNumeratorEditingFinished();
    void vlSetPointFactorDenominatorEditingFinished();
    void vlDimensionFactorNumeratorEditingFinished();
    void vlDimensionFactorDenominatorEditingFinished();

    void vlEnableRampClicked(int id);
    void vlUnlockRampClicked(int id);
    void vlReferenceRampClicked(int id);
    void vlHaltClicked(int id);

    enum ControlWord : quint16
    {
        CW_SwitchOn = 0x01,
        CW_EnableVoltage = 0x02,
        CW_QuickStop = 0x04,
        CW_EnableOperation = 0x08,
        CW_FaultReset = 0x80,
        CW_Halt = 0x100,
        CW_OperationModeSpecific = 0x70,
        CW_ManufacturerSpecific = 0xF800,

        CW_Mask = 0x8F
    };

    enum StatusWord : quint16
    {
        SW_StateNotReadyToSwitchOn = 0x00,
        SW_StateSwitchOnDisabled = 0x40,
        SW_StateReadyToSwitchOn = 0x21,
        SW_StateSwitchedOn = 0x23,
        SW_StateOperationEnabled = 0x27,
        SW_StateQuickStopActive = 0x07,
        SW_StateFaultReactionActive = 0x0F,
        SW_StateFault = 0x08,

        SW_VoltageEnabled = 0x10,
        SW_Warning = 0x80,
        SW_Remote = 0x200,
        SW_TargetReached = 0x400,
        SW_InternalLimitActive = 0x800,
        SW_OperationModeSpecific = 0x3000,
        SW_ManufacturerSpecific = 0xC000
    };

    enum StatusWordStateMask
    {
        Mask1 = 0x4F,
        Mask2 = 0x6f
    };

    enum ControlWordVL : quint16
    {
        CW_VL_EnableRamp = 0x10,
        CW_VL_UnlockRamp = 0x20,
        CW_VL_ReferenceRamp = 0x40
    };

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // WIDGETDEBUG_H
