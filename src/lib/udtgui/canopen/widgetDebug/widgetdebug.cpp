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

#include "widgetdebug.h"

#include "services/services.h"
#include <QButtonGroup>
#include <QCheckBox>
#include <QDebug>
#include <QFormLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QDir>
#include "canopen/datalogger/dataloggerwidget.h"

WidgetDebug::WidgetDebug(QWidget *parent)
    : QWidget(parent)
{
}

WidgetDebug::WidgetDebug(Node *node, QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
    if ((node->nodeOd()->value(0x1000, 0x0).toUInt() & 0xFFFF) != 0x192)
    {
        return;
    }

    _controlWordObjectId = 0x6040;
    _statusWordObjectId = 0x6041;
    _haltObjectId = 0x605D;
    _quickStopObjectId = 0x605A;
    _abortConnectionObjectId = 0x6007;
    _faultReactionObjectId = 0x605E;

    createWidgets();
    setCheckableStateMachine(2);
    registerObjId({_controlWordObjectId, 0x00});
    registerObjId({_statusWordObjectId, 0x00});
    registerObjId({_haltObjectId, 0x00});
    registerObjId({_quickStopObjectId, 0x00});
    registerObjId({_abortConnectionObjectId, 0x00});
    registerObjId({_faultReactionObjectId, 0x00});

    // VL_MODE
    _vlVelocityDemandObjectId = 0x6043;
    _vlVelocityActualObjectId = 0x6044;
    _vlTargetVelocityObjectId = 0x6042;
    registerObjId({_vlVelocityDemandObjectId, 0x00});
    registerObjId({_vlVelocityActualObjectId, 0x00});

    setNode(node);
}
WidgetDebug::~WidgetDebug()
{
    unRegisterFullOd();
}
void WidgetDebug::setNode(Node *node)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &WidgetDebug::updateData);
        }
    }

    setNodeInterrest(node);
    _node = node;

    if (_node)
    {
        this->setWindowTitle("402 : " + _node->name() +  ", Status :" + _node->statusStr());

        connect(_node, &Node::statusChanged, this, &WidgetDebug::updateData);
        _node->readObject(_statusWordObjectId, 0x0);
        _node->readObject(_haltObjectId, 0x0);
        _node->readObject(_quickStopObjectId, 0x0);
        _node->readObject(_abortConnectionObjectId, 0x0);
        _node->readObject(_faultReactionObjectId, 0x0);

        cmdControlWord = ControlWordVL::CW_VL_EnableRamp | ControlWordVL::CW_VL_UnlockRamp | ControlWordVL::CW_VL_ReferenceRamp;
        _node->writeObject(_controlWordObjectId, 0x00, cmdControlWord);

        if (_node->status() != Node::STARTED)
        {
            this->setEnabled(false);
        }
    }
}

void WidgetDebug::updateData()
{
    if (_node)
    {
        this->setWindowTitle("402 : " + _node->name() +  ", Status :" + _node->statusStr());
        if (_node->status() == Node::STARTED)
        {
            this->setEnabled(true);
            cmdControlWord = ControlWordVL::CW_VL_EnableRamp | ControlWordVL::CW_VL_UnlockRamp | ControlWordVL::CW_VL_ReferenceRamp;
            _node->writeObject(_controlWordObjectId, 0x00, cmdControlWord);
        }
        else
        {
            // this->setEnabled(false);
        }
    }
}

void WidgetDebug::stateMachineClicked(int id)
{
    cmdControlWord = (cmdControlWord & ~CW_Mask);

    // !! id is not the current state, it's id id groupbutton
    switch (id)
    {
    case STATE_NotReadyToSwitchOn: // 1_Not ready to switch on
        break;
    case STATE_SwitchOnDisabled: // 2_Switch on disabled
        if (stateMachineCurrent == STATE_Fault)
        {
            cmdControlWord |= CW_FaultReset;
            _node->writeObject(_controlWordObjectId, 0x00, QVariant(cmdControlWord));
        }
        cmdControlWord = (cmdControlWord & ~CW_Mask);
        break;
    case STATE_ReadyToSwitchOn: // 3_Ready to switch on
        cmdControlWord |= (CW_EnableVoltage | CW_QuickStop);
        break;
    case STATE_SwitchedOn: // 4_Switched on
        cmdControlWord |= CW_EnableVoltage | CW_QuickStop | CW_SwitchOn;
        break;
    case STATE_OperationEnabled: // 5_Operation enabled
        cmdControlWord |= CW_EnableVoltage | CW_QuickStop | CW_SwitchOn | CW_EnableOperation;
        break;
    case STATE_QuickStopActive: // 6_Quick stop active
        cmdControlWord |= CW_EnableVoltage;
        break;
    case STATE_FaultReactionActive: // 7_Fault reaction active
        break;
    case STATE_Fault: // 8_Fault
        break;
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(cmdControlWord));
    _controlWordLabel->setText("0x" + QString::number(cmdControlWord, 16).toUpper());
}

void WidgetDebug::controlWordHaltClicked()
{
    cmdControlWord |= CW_Halt;
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(cmdControlWord));
    _controlWordLabel->setText("0x" + QString::number(cmdControlWord, 16).toUpper());
    // Initiate bit halt
    cmdControlWord = (cmdControlWord & ~CW_Halt);
}

void WidgetDebug::haltOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_haltObjectId, 0x00, QVariant(value));
}

void WidgetDebug::quickStopOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_quickStopObjectId, 0x00, QVariant(value));
}

void WidgetDebug::abortConnectionOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_abortConnectionObjectId, 0x00, QVariant(value));
}

void WidgetDebug::faultReactionOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_faultReactionObjectId, 0x00, QVariant(value));
}

void WidgetDebug::manageNotificationControlWordObject(SDO::FlagsRequest flags)
{
    if (flags == SDO::FlagsRequest::Error)
    {
        _controlWordLabel->setText("Error SDO : 0x" + QString::number(_node->nodeOd()->errorObject(_controlWordObjectId, 0x0), 16));
    }
    quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toInt());
    if (_vlEnableRampButtonGroup->button((controlWord & CW_VL_EnableRamp) > 4))
    {
        _vlEnableRampButtonGroup->button((controlWord & CW_VL_EnableRamp) > 4)->setChecked(true);
    }
    if (_vlUnlockRampButtonGroup->button((controlWord & CW_VL_UnlockRamp) > 5))
    {
        _vlUnlockRampButtonGroup->button((controlWord & CW_VL_UnlockRamp) > 5)->setChecked(true);
    }
    if (_vlReferenceRampButtonGroup->button((controlWord & CW_VL_ReferenceRamp) > 6))
    {
        _vlReferenceRampButtonGroup->button((controlWord & CW_VL_ReferenceRamp) > 6)->setChecked(true);
    }
}

void WidgetDebug::manageNotificationStatusWordobject()
{
    if (!_node)
    {
        return;
    }

    _statusWordRawLabel->setText("0x" + QString::number(_node->nodeOd()->value(_statusWordObjectId, 0x00).toUInt(), 16));
    quint16 state = static_cast<quint16>(_node->nodeOd()->value(_statusWordObjectId, 0x00).toUInt());

    if ((state & Mask1) == SW_StateNotReadyToSwitchOn)
    {
        stateMachineCurrent = STATE_NotReadyToSwitchOn;
        _statusWordLabel->setText(tr("NotReadyToSwitchOn"));
        setCheckableStateMachine(STATE_NotReadyToSwitchOn);

    }
    if ((state & Mask1) == SW_StateSwitchOnDisabled)
    {
        stateMachineCurrent = STATE_SwitchOnDisabled;
        _statusWordLabel->setText(tr("SwitchOnDisabled"));
        setCheckableStateMachine(STATE_SwitchOnDisabled);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);

    }
    if ((state & Mask2) == SW_StateReadyToSwitchOn)
    {
        stateMachineCurrent = STATE_ReadyToSwitchOn;
        _statusWordLabel->setText(tr("ReadyToSwitchOn"));
        setCheckableStateMachine(STATE_ReadyToSwitchOn);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
    }
    if ((state & Mask2) == SW_StateSwitchedOn)
    {
        stateMachineCurrent = STATE_SwitchedOn;
        _statusWordLabel->setText(tr("SwitchedOn"));
        setCheckableStateMachine(STATE_SwitchedOn);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
    }
    if ((state & Mask2) == SW_StateOperationEnabled)
    {
        stateMachineCurrent = STATE_OperationEnabled;
        _statusWordLabel->setText(tr("OperationEnabled"));
        setCheckableStateMachine(STATE_OperationEnabled);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(true);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(true);
    }
    if ((state & Mask2) == SW_StateQuickStopActive)
    {
        stateMachineCurrent = STATE_QuickStopActive;
        _statusWordLabel->setText(tr("QuickStopActive"));
        setCheckableStateMachine(STATE_QuickStopActive);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(true);
    }
    if ((state & Mask1) == SW_StateFaultReactionActive)
    {
        stateMachineCurrent = STATE_FaultReactionActive;
        _statusWordLabel->setText(tr("FaultReactionActive"));
        setCheckableStateMachine(STATE_FaultReactionActive);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
    }
    if ((state & Mask1) == SW_StateFault)
    {
        stateMachineCurrent = STATE_Fault;
        _statusWordLabel->setText(tr("Fault"));
        setCheckableStateMachine(STATE_Fault);
        _stateMachineGroup->button(STATE_SwitchOnDisabled)->setEnabled(true);
        _stateMachineGroup->button(STATE_ReadyToSwitchOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_SwitchedOn)->setEnabled(false);
        _stateMachineGroup->button(STATE_OperationEnabled)->setEnabled(false);
        _stateMachineGroup->button(STATE_QuickStopActive)->setEnabled(false);
    }
    update();

    _voltageEnabledLabel->setText(tr("False"));
    _warningLabel->setText(tr("False"));
    _remoteLabel->setText(tr("False"));
    _targetReachedLabel->setText(tr("False"));
    _internalLimitActiveLabel->setText(tr("False"));
    _operationModeSpecificLabel->setText("0x0");
    _manufacturerSpecificLabel->setText("0x0");

    if ((state & SW_VoltageEnabled) == SW_VoltageEnabled)
    {
        _voltageEnabledLabel->setText(tr("True"));
    }
    if ((state & SW_Warning) == SW_Warning)
    {
        _warningLabel->setText(tr("True"));
    }
    if ((state & SW_Remote) == SW_Remote)
    {
        _remoteLabel->setText(tr("True"));
    }
    if ((state & SW_TargetReached) == SW_TargetReached)
    {
        _targetReachedLabel->setText(tr("True"));
    }
    if ((state & SW_InternalLimitActive) == SW_InternalLimitActive)
    {
        _internalLimitActiveLabel->setText(tr("True"));
    }
    if ((state & SW_OperationModeSpecific) == SW_OperationModeSpecific)
    {
        _operationModeSpecificLabel->setText("0x" + QString::number(state & SW_OperationModeSpecific, 16));
    }
    if ((state & SW_ManufacturerSpecific) == SW_ManufacturerSpecific)
    {
        _manufacturerSpecificLabel->setText("0x" + QString::number(state & SW_ManufacturerSpecific, 16));
    }
}

void WidgetDebug::setCheckableStateMachine(int id)
{
    for (int i = 1; i <= 8; i++)
    {
        _stateMachineGroup->button(i)->setCheckable(false);
    }
    _stateMachineGroup->button(id)->setCheckable(true);
    _stateMachineGroup->button(id)->setChecked(true);
}
void WidgetDebug::vlTargetVelocitySpinboxFinished()
{
    qint16 value = static_cast<qint16>(_vlTargetVelocitySpinBox->value());
    _node->writeObject(_vlTargetVelocityObjectId, 0x00, QVariant(value));
    _vlTargetVelocitySlider->setValue(value);
}
void WidgetDebug::vlTargetVelocitySliderChanged()
{
    qint16 value = static_cast<qint16>(_vlTargetVelocitySpinBox->value());
    _node->writeObject(_vlTargetVelocityObjectId, 0x00, QVariant(value));
}
void WidgetDebug::vlMinAmountEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlMinVelocityMinMaxAmountSpinBox->value());
    _node->writeObject(0x6046, 0x01, QVariant(value));
}
void WidgetDebug::vlMaxAmountEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlMaxVelocityMinMaxAmountSpinBox->value());
    _node->writeObject(0x6046, 0x02, QVariant(value));
}
void WidgetDebug::vlAccelerationDeltaSpeedEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlAccelerationDeltaSpeedSpinBox->value());
    _node->writeObject(0x6048, 0x01, QVariant(value));
}
void WidgetDebug::vlAccelerationDeltaTimeEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlAccelerationDeltaTimeSpinBox->value());
    _node->writeObject(0x6048, 0x02, QVariant(value));
}
void WidgetDebug::vlDecelerationDeltaSpeedEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlDecelerationDeltaSpeedDecelerationSpinBox->value());
    _node->writeObject(0x6049, 0x01, QVariant(value));
}
void WidgetDebug::vlDecelerationDeltaTimeEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlDecelerationDeltaTimeSpinBox->value());
    _node->writeObject(0x6049, 0x02, QVariant(value));
}
void WidgetDebug::vlQuickStopDeltaSpeedEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlQuickStopDeltaSpeedSpinBox->value());
    _node->writeObject(0x604A, 0x01, QVariant(value));
}
void WidgetDebug::vlQuickStopDeltaTimeEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlQuickStopDeltaTimeSpinBox->value());
    _node->writeObject(0x604A, 0x02, QVariant(value));
}
void WidgetDebug::vlSetPointFactorNumeratorEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlSetPointFactorNumeratorSpinBox->value());
    _node->writeObject(0x604B, 0x01, QVariant(value));
}
void WidgetDebug::vlSetPointFactorDenominatorEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlSetPointFactorDenominatorSpinBox->value());
    _node->writeObject(0x604B, 0x02, QVariant(value));
}

void WidgetDebug::vlDimensionFactorNumeratorEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlDimensionFactorNumeratorSpinBox->value());
    _node->writeObject(0x604C, 0x01, QVariant(value));
}
void WidgetDebug::vlDimensionFactorDenominatorEditingFinished()
{
    quint32 value = static_cast<quint32>(_vlDimensionFactorDenominatorSpinBox->value());
    _node->writeObject(0x604C, 0x02, QVariant(value));
}

void WidgetDebug::vlEnableRampClicked(int id)
{
    if (id == 0)
    {
        cmdControlWord = (cmdControlWord & ~ControlWordVL::CW_VL_EnableRamp);
    }
    else if (id == 1)
    {
        cmdControlWord |= CW_VL_EnableRamp;
    }
    else
    {
        return;
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(cmdControlWord));
    _controlWordLabel->setText("0x" + QString::number(cmdControlWord, 16).toUpper());
}
void WidgetDebug::vlUnlockRampClicked(int id)
{
    if (id == 0)
    {
        cmdControlWord = (cmdControlWord & ~ControlWordVL::CW_VL_UnlockRamp);
    }
    else if (id == 1)
    {
        cmdControlWord |= CW_VL_UnlockRamp;
    }
    else
    {
        return;
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(cmdControlWord));
    _controlWordLabel->setText("0x" + QString::number(cmdControlWord, 16).toUpper());
}
void WidgetDebug::vlReferenceRampClicked(int id)
{
    if (id == 0)
    {
        cmdControlWord = (cmdControlWord & ~ControlWordVL::CW_VL_ReferenceRamp);
    }
    else if (id == 1)
    {
        cmdControlWord |= CW_VL_ReferenceRamp;
    }
    else
    {
        return;
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(cmdControlWord));
    _controlWordLabel->setText("0x" + QString::number(cmdControlWord, 16).toUpper());
}
void WidgetDebug::vlHaltClicked(int id)
{
    if (id == 0)
    {
        cmdControlWord = (cmdControlWord & ~CW_Halt);
    }
    else if (id == 1)
    {
        cmdControlWord |= CW_Halt;
    }
    else
    {
        return;
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(cmdControlWord));
    _controlWordLabel->setText("0x" + QString::number(cmdControlWord, 16).toUpper());
}

void WidgetDebug::dataLogger()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *_dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLogger->addData({_node->busId(), _node->nodeId(), _vlVelocityActualObjectId, 0x0, QMetaType::Type::Short});
    dataLogger->addData({_node->busId(), _node->nodeId(), _vlTargetVelocityObjectId, 0x0, QMetaType::Type::Short});
    dataLogger->addData({_node->busId(), _node->nodeId(), _vlVelocityDemandObjectId, 0x0, QMetaType::Type::Short});
    _dataLoggerWidget->show();
}
void WidgetDebug::pdoMapping()
{
    QList<NodeObjectId> vlRpdoObjectList = {{_node->busId(), _node->nodeId(), _controlWordObjectId, 0x0, QMetaType::Type::UShort},
                                            {_node->busId(), _node->nodeId(), _vlTargetVelocityObjectId, 0x0, QMetaType::Type::Short}};

    _node->rpdos().at(0)->writeMapping(vlRpdoObjectList);
    QList<NodeObjectId> vlTpdoObjectList = {{_node->busId(), _node->nodeId(), _statusWordObjectId, 0x0, QMetaType::Type::UShort},
                                            {_node->busId(), _node->nodeId(), _vlVelocityActualObjectId, 0x0, QMetaType::Type::Short}};

    _node->tpdos().at(2)->writeMapping(vlTpdoObjectList);
}

void WidgetDebug::createWidgets()
{
    // FIRST COLUMM
    QLayout *firstLayout = new QVBoxLayout();
    firstLayout->setMargin(0);

    // toolbar TIMER
    _toolBar = new QToolBar(tr("Read Status Word"));
    // start stop
    _startStopAction = _toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _startStopAction->setToolTip(tr("Start or stop"));
    connect(_startStopAction, &QAction::triggered, this, &WidgetDebug::toggleStart);
    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(500);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    _toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ setTimer(i); });
    connect(&_timer, &QTimer::timeout, this, &WidgetDebug::readData);
    firstLayout->addWidget(_toolBar);

    // Group Box State Machine
    QGroupBox *stateMachineGroupBox = new QGroupBox(tr("State Machine"));
    stateMachineGroupBox->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    QFormLayout *stateMachineLayoutGroupBox = new QFormLayout();

    _stateMachineGroup = new QButtonGroup(this);
    _stateMachineGroup->setExclusive(true);

    QPushButton *stateNotReadyToSwitchOnPushButton = new QPushButton(tr("1_Not ready to switch on"));
    stateNotReadyToSwitchOnPushButton->setEnabled(false);
    stateMachineLayoutGroupBox->addRow(stateNotReadyToSwitchOnPushButton);
    _stateMachineGroup->addButton(stateNotReadyToSwitchOnPushButton, STATE_NotReadyToSwitchOn);
    QPushButton *stateSwitchOnDisabledPushButton = new QPushButton(tr("2_Switch on disabled"));
    stateMachineLayoutGroupBox->addRow(stateSwitchOnDisabledPushButton);
    _stateMachineGroup->addButton(stateSwitchOnDisabledPushButton, STATE_SwitchOnDisabled);
    QPushButton *stateReadyToSwitchOnPushButton = new QPushButton(tr("3_Ready to switch on"));
    stateMachineLayoutGroupBox->addRow(stateReadyToSwitchOnPushButton);
    _stateMachineGroup->addButton(stateReadyToSwitchOnPushButton, STATE_ReadyToSwitchOn);
    QPushButton *stateSwitchedOnPushButton = new QPushButton(tr("4_Switched on"));
    stateMachineLayoutGroupBox->addRow(stateSwitchedOnPushButton);
    _stateMachineGroup->addButton(stateSwitchedOnPushButton, STATE_SwitchedOn);
    QPushButton *stateOperationEnabledPushButton = new QPushButton(tr("5_Operation enabled"));
    stateMachineLayoutGroupBox->addRow(stateOperationEnabledPushButton);
    _stateMachineGroup->addButton(stateOperationEnabledPushButton, STATE_OperationEnabled);
    QPushButton *stateQuickStopActivePushButton = new QPushButton(tr("6_Quick stop active"));
    stateMachineLayoutGroupBox->addRow(stateQuickStopActivePushButton);
    _stateMachineGroup->addButton(stateQuickStopActivePushButton, STATE_QuickStopActive);
    QPushButton *stateFaultReactionActivePushButton = new QPushButton(tr("7_Fault reaction active"));
    stateFaultReactionActivePushButton->setEnabled(false);
    stateMachineLayoutGroupBox->addRow(stateFaultReactionActivePushButton);
    _stateMachineGroup->addButton(stateFaultReactionActivePushButton, STATE_FaultReactionActive);
    QPushButton *stateFaultPushButton = new QPushButton(tr("8_Fault"));
    stateFaultPushButton->setEnabled(false);
    stateMachineLayoutGroupBox->addRow(stateFaultPushButton);
    _stateMachineGroup->addButton(stateFaultPushButton, STATE_Fault);

    connect(_stateMachineGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { stateMachineClicked(id); });
    stateMachineGroupBox->setLayout(stateMachineLayoutGroupBox);
    // END Group Box State Machine

    // Group Box Control Word
    QGroupBox *controlWordGroupBox = new QGroupBox(tr("Control Word (0x6040)"));
    QFormLayout *controlWordLayout = new QFormLayout();

    QPushButton *_haltPushButton = new QPushButton(tr("Halt"));
    controlWordLayout->addRow(_haltPushButton);
    controlWordGroupBox->setLayout(controlWordLayout);

    _controlWordLabel = new QLabel();
    controlWordLayout->addRow(tr("ControlWord sended:"), _controlWordLabel);
    connect(_haltPushButton, &QPushButton::clicked, this, &WidgetDebug::controlWordHaltClicked);
    // END Group Box Control Word

    // Group Box Status Word
    QGroupBox *statusWordGroupBox = new QGroupBox(tr("Status Word (0x6041)"));
    QFormLayout *statusWordLayout = new QFormLayout();

    _statusWordRawLabel = new QLabel();
    statusWordLayout->addRow(tr("StatusWord raw:"), _statusWordRawLabel);
    _statusWordLabel = new QLabel();
    statusWordLayout->addRow(tr("StatusWord State:"), _statusWordLabel);
    _voltageEnabledLabel = new QLabel();
    statusWordLayout->addRow(tr("Voltage Enabled :"), _voltageEnabledLabel);
    _warningLabel = new QLabel();
    statusWordLayout->addRow(tr("Warning :"), _warningLabel);
    _remoteLabel = new QLabel();
    statusWordLayout->addRow(tr("Remote :"), _remoteLabel);
    _targetReachedLabel = new QLabel();
    statusWordLayout->addRow(tr("Target Reached :"), _targetReachedLabel);
    _internalLimitActiveLabel = new QLabel();
    statusWordLayout->addRow(tr("Internal Limit Active :"), _internalLimitActiveLabel);
    _operationModeSpecificLabel = new QLabel();
    statusWordLayout->addRow(tr("Operation Mode Specific:"), _operationModeSpecificLabel);
    _manufacturerSpecificLabel = new QLabel();
    statusWordLayout->addRow(tr("Manufacturer Specific:"), _manufacturerSpecificLabel);
    statusWordGroupBox->setLayout(statusWordLayout);
    // END Group Box Status Word
    firstLayout->addWidget(stateMachineGroupBox);
    firstLayout->addWidget(controlWordGroupBox);
    firstLayout->addWidget(statusWordGroupBox);
    // END FIRST COLUMM

    // SECOND COLUMM
    QLayout *secondColumnlayout = new QVBoxLayout();
    secondColumnlayout->setMargin(0);

    // Group Box Halt option
    QGroupBox *haltOptionGroupBox = new QGroupBox(tr("Halt option (0x605D)"));
    QFormLayout *haltOptionLayout = new QFormLayout();
    _haltOptionGroup = new QButtonGroup(this);
    _haltOptionGroup->setExclusive(true);
    QRadioButton *_slowDownRampCheckBox = new QRadioButton(tr("+1 Slow down on slow down ramp and stay in operation enabled"));
    haltOptionLayout->addRow(_slowDownRampCheckBox);
    _haltOptionGroup->addButton(_slowDownRampCheckBox, 1);
    QRadioButton *_quickStopRampCheckBox = new QRadioButton(tr("+2 Slow down on quick stop ramp and stay in operation enabled"));
    haltOptionLayout->addRow(_quickStopRampCheckBox);
    _haltOptionGroup->addButton(_quickStopRampCheckBox, 2);
    haltOptionGroupBox->setLayout(haltOptionLayout);
    connect(_haltOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { haltOptionClicked(id); });
    // END Group Box Halt option

    // Group Box Quick stop option
    QGroupBox *quickStopOptionGroupBox = new QGroupBox(tr("Quick stop option (0x605A)"));
    QFormLayout *quickStopOptionLayout = new QFormLayout();
    _quickStopOptionGroup = new QButtonGroup(this);
    _quickStopOptionGroup->setExclusive(true);
    QRadioButton *_0QuickStopOptionCheckBox = new QRadioButton(tr("0 Disable drive function"));
    quickStopOptionLayout->addRow(_0QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_0QuickStopOptionCheckBox, 0);
    QRadioButton *_1QuickStopOptionCheckBox = new QRadioButton(tr("+1 Slow down on slow down ramp and transit into switch on disabled"));
    quickStopOptionLayout->addRow(_1QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_1QuickStopOptionCheckBox, 1);
    QRadioButton *_2QuickStopOptionCheckBox = new QRadioButton(tr("+2 Slow down on quick stop ramp and transit into switch on disabled"));
    quickStopOptionLayout->addRow(_2QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_2QuickStopOptionCheckBox, 2);
    QRadioButton *_5QuickStopOptionCheckBox = new QRadioButton(tr("+5 Slow down on slow down ramp and stay in quick stop active"));
    quickStopOptionLayout->addRow(_5QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_5QuickStopOptionCheckBox, 5);
    QRadioButton *_6QuickStopOptionCheckBox = new QRadioButton(tr("+6 Slow down on quick stop ramp and stay in quick stop active"));
    quickStopOptionLayout->addRow(_6QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_6QuickStopOptionCheckBox, 6);
    quickStopOptionGroupBox->setLayout(quickStopOptionLayout);
    connect(_quickStopOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { quickStopOptionClicked(id); });
    // END Group Box Quick stop option

    // Group Box Quick stop option
    QGroupBox *abortConnectionOptionGroupBox = new QGroupBox(tr("Abort connection option (0x6007)"));
    QFormLayout *abortConnectionOptionLayout = new QFormLayout();
    _abortConnectionOptionGroup = new QButtonGroup(this);
    _abortConnectionOptionGroup->setExclusive(true);
    QRadioButton *_0AbortConnectionOptionCheckBox = new QRadioButton(tr("0 No action"));
    abortConnectionOptionLayout->addRow(_0AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_0AbortConnectionOptionCheckBox, 0);
    QRadioButton *_1AbortConnectionOptionCheckBox = new QRadioButton(tr("+1 Fault signal"));
    abortConnectionOptionLayout->addRow(_1AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_1AbortConnectionOptionCheckBox, 1);
    QRadioButton *_2AbortConnectionOptionCheckBox = new QRadioButton(tr("+2 Disable voltage command"));
    abortConnectionOptionLayout->addRow(_2AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_2AbortConnectionOptionCheckBox, 2);
    QRadioButton *_3AbortConnectionOptionCheckBox = new QRadioButton(tr("+3 Quick stop command"));
    abortConnectionOptionLayout->addRow(_3AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_3AbortConnectionOptionCheckBox, 3);
    abortConnectionOptionGroupBox->setLayout(abortConnectionOptionLayout);
    connect(_abortConnectionOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { abortConnectionOptionClicked(id); });
    // END Group Box Quick stop option

    // Group Box Fault reaction option
    QGroupBox *faultReactionOptionGroupBox = new QGroupBox(tr("Fault reaction option (0x605E)"));
    QFormLayout *faultReactionOptionLayout = new QFormLayout();
    _faultReactionOptionGroup = new QButtonGroup(this);
    _faultReactionOptionGroup->setExclusive(true);
    QRadioButton *_0FaultReactionOptionCheckBox = new QRadioButton(tr("0 Disable drive function, motor is free to rotate"));
    faultReactionOptionLayout->addRow(_0FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_0FaultReactionOptionCheckBox, 0);
    QRadioButton *_1FaultReactionOptionCheckBox = new QRadioButton(tr("+1 Slow down on slow down ramp"));
    faultReactionOptionLayout->addRow(_1FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_1FaultReactionOptionCheckBox, 1);
    QRadioButton *_2FaultReactionOptionCheckBox = new QRadioButton(tr("+2 Slow down on quick stop ramp"));
    faultReactionOptionLayout->addRow(_2FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_2FaultReactionOptionCheckBox, 2);
    faultReactionOptionGroupBox->setLayout(faultReactionOptionLayout);
    connect(_faultReactionOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { faultReactionOptionClicked(id); });
    // END Group Box Quick stop option

    secondColumnlayout->addWidget(haltOptionGroupBox);
    secondColumnlayout->addWidget(quickStopOptionGroupBox);
    secondColumnlayout->addWidget(abortConnectionOptionGroupBox);
    secondColumnlayout->addWidget(faultReactionOptionGroupBox);
    // END SECOND COLUMM

    // THIRD COLUMM
    QLayout *thirdColumnlayout = new QVBoxLayout();
    thirdColumnlayout->setMargin(0);

    // Group Box VL mode
    QGroupBox *vlGroupBox = new QGroupBox(tr("Velocity mode"));
    QFormLayout *vlLayout = new QFormLayout();

    _vlTargetVelocitySpinBox = new QSpinBox();
    _vlTargetVelocitySpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    vlLayout->addRow("Target velocity (0x6042) :", _vlTargetVelocitySpinBox);

    _vlTargetVelocitySlider = new QSlider(Qt::Horizontal);
    _vlTargetVelocitySlider->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    vlLayout->addRow(_vlTargetVelocitySlider);

    connect(_vlTargetVelocitySlider, &QSlider::valueChanged, _vlTargetVelocitySpinBox, &QSpinBox::setValue);
    connect(_vlTargetVelocitySlider, &QSlider::valueChanged, this, &WidgetDebug::vlTargetVelocitySliderChanged);
    connect(_vlTargetVelocitySpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlTargetVelocitySpinboxFinished);

    _vlVelocityDemandLabel = new QLabel();
    _vlVelocityDemandLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vlLayout->addRow("Velocity_demand (0x6043) :", _vlVelocityDemandLabel);

    _vlVelocityActualLabel = new QLabel();
    _vlVelocityActualLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vlLayout->addRow("Velocity_actual_value (0x6044) :", _vlVelocityActualLabel);

    QLabel *vlVelocityMinMaxAmountLabel = new QLabel(tr("Velocity min max amount (0x6046) :"));
    vlLayout->addRow(vlVelocityMinMaxAmountLabel);
    QLayout *vlVelocityMinMaxAmountlayout = new QHBoxLayout();
    _vlMinVelocityMinMaxAmountSpinBox = new QSpinBox();
    _vlMinVelocityMinMaxAmountSpinBox->setToolTip("min ");
    _vlMinVelocityMinMaxAmountSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    vlVelocityMinMaxAmountlayout->addWidget(_vlMinVelocityMinMaxAmountSpinBox);
    _vlMaxVelocityMinMaxAmountSpinBox = new QSpinBox();
    _vlMaxVelocityMinMaxAmountSpinBox->setToolTip("max ");
    _vlMaxVelocityMinMaxAmountSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    vlVelocityMinMaxAmountlayout->addWidget(_vlMaxVelocityMinMaxAmountSpinBox);
    vlLayout->addRow(vlVelocityMinMaxAmountlayout);
    connect(_vlMinVelocityMinMaxAmountSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlMinAmountEditingFinished);
    connect(_vlMaxVelocityMinMaxAmountSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlMaxAmountEditingFinished);

    QLabel *vlVelocityAccelerationLabel = new QLabel(tr("Velocity acceleration (0x6048) :"));
    vlLayout->addRow(vlVelocityAccelerationLabel);
    QLayout *vlVelocityAccelerationlayout = new QHBoxLayout();
    _vlAccelerationDeltaSpeedSpinBox = new QSpinBox();
    _vlAccelerationDeltaSpeedSpinBox->setSuffix(" inc/ms");
    _vlAccelerationDeltaSpeedSpinBox->setToolTip("Delta Speed");
    _vlAccelerationDeltaSpeedSpinBox->setRange(0, std::numeric_limits<int>::max());
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaSpeedSpinBox);
    _vlAccelerationDeltaTimeSpinBox = new QSpinBox();
    _vlAccelerationDeltaTimeSpinBox->setSuffix(" ms");
    _vlAccelerationDeltaTimeSpinBox->setToolTip("Delta Time");
    _vlAccelerationDeltaTimeSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityAccelerationlayout);
    connect(_vlAccelerationDeltaSpeedSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlAccelerationDeltaSpeedEditingFinished);
    connect(_vlAccelerationDeltaTimeSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlAccelerationDeltaTimeEditingFinished);

    QLabel *vlVelocityDecelerationLabel = new QLabel(tr("Velocity deceleration (0x6049) :"));
    vlLayout->addRow(vlVelocityDecelerationLabel);
    QLayout *vlVelocityDecelerationlayout = new QHBoxLayout();
    _vlDecelerationDeltaSpeedDecelerationSpinBox = new QSpinBox();
    _vlDecelerationDeltaSpeedDecelerationSpinBox->setSuffix(" inc/ms");
    _vlDecelerationDeltaSpeedDecelerationSpinBox->setToolTip("Delta Speed");
    _vlDecelerationDeltaSpeedDecelerationSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaSpeedDecelerationSpinBox);
    _vlDecelerationDeltaTimeSpinBox = new QSpinBox();
    _vlDecelerationDeltaTimeSpinBox->setSuffix(" ms");
    _vlDecelerationDeltaTimeSpinBox->setToolTip("Delta Time");
    _vlDecelerationDeltaTimeSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityDecelerationlayout);
    connect(_vlDecelerationDeltaSpeedDecelerationSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlDecelerationDeltaSpeedEditingFinished);
    connect(_vlDecelerationDeltaTimeSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlDecelerationDeltaTimeEditingFinished);

    QLabel *vlVelocityQuickStopLabel = new QLabel(tr("Velocity quick stop (0x604A) :"));
    vlLayout->addRow(vlVelocityQuickStopLabel);
    QLayout *vlVelocityQuickStoplayout = new QHBoxLayout();
    _vlQuickStopDeltaSpeedSpinBox = new QSpinBox();
    _vlQuickStopDeltaSpeedSpinBox->setSuffix(" inc/ms");
    _vlQuickStopDeltaSpeedSpinBox->setToolTip("Delta Speed");
    _vlQuickStopDeltaSpeedSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaSpeedSpinBox);
    _vlQuickStopDeltaTimeSpinBox = new QSpinBox();
    _vlQuickStopDeltaTimeSpinBox->setSuffix(" ms");
    _vlQuickStopDeltaTimeSpinBox->setToolTip("Delta Time");
    _vlQuickStopDeltaTimeSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<int>::max());
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityQuickStoplayout);
    connect(_vlQuickStopDeltaSpeedSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlQuickStopDeltaSpeedEditingFinished);
    connect(_vlQuickStopDeltaTimeSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlQuickStopDeltaTimeEditingFinished);

    QLabel *vlSetPointFactorLabel = new QLabel(tr("Set-point factor (0x604B) :"));
    vlLayout->addRow(vlSetPointFactorLabel);
    QLayout *vlSetPointFactorlayout = new QHBoxLayout();
    _vlSetPointFactorNumeratorSpinBox = new QSpinBox();
    _vlSetPointFactorNumeratorSpinBox->setToolTip("Numerator");
    _vlSetPointFactorNumeratorSpinBox->setRange(1, std::numeric_limits<qint32>::max());
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorNumeratorSpinBox);
    _vlSetPointFactorDenominatorSpinBox = new QSpinBox();
    _vlSetPointFactorDenominatorSpinBox->setToolTip("Denominator");
    _vlSetPointFactorDenominatorSpinBox->setRange(1, std::numeric_limits<qint32>::max());
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorDenominatorSpinBox);
    vlLayout->addRow(vlSetPointFactorlayout);
    connect(_vlSetPointFactorNumeratorSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlSetPointFactorNumeratorEditingFinished);
    connect(_vlSetPointFactorDenominatorSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlSetPointFactorDenominatorEditingFinished);

    QLabel *vlDimensionFactorLabel = new QLabel(tr("Dimension factor (0x604C) :"));
    vlLayout->addRow(vlDimensionFactorLabel);
    QLayout *vlDimensionFactorlayout = new QHBoxLayout();
    _vlDimensionFactorNumeratorSpinBox = new QSpinBox();
    _vlDimensionFactorNumeratorSpinBox->setToolTip("Numerator");
    _vlDimensionFactorNumeratorSpinBox->setRange(1, std::numeric_limits<qint32>::max());
    vlDimensionFactorlayout->addWidget(_vlDimensionFactorNumeratorSpinBox);
    _vlDimensionFactorDenominatorSpinBox = new QSpinBox();
    _vlDimensionFactorDenominatorSpinBox->setToolTip("Denominator");
    _vlDimensionFactorDenominatorSpinBox->setRange(1, std::numeric_limits<qint32>::max());
    vlDimensionFactorlayout->addWidget(_vlDimensionFactorDenominatorSpinBox);
    vlLayout->addRow(vlDimensionFactorlayout);
    connect(_vlDimensionFactorNumeratorSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlDimensionFactorNumeratorEditingFinished);
    connect(_vlDimensionFactorDenominatorSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlDimensionFactorDenominatorEditingFinished);

    vlGroupBox->setLayout(vlLayout);

    // Group Box Control Word
    QGroupBox *modeControlWordGroupBox = new QGroupBox(tr("Control Word (0x6040) bit 4, bit 5, bit 6, bit 8"));
    QFormLayout *modeControlWordLayout = new QFormLayout();

    QLabel *vlEnableRampLabel = new QLabel(tr("Enable ramp (bit 4) :"));
    modeControlWordLayout->addRow(vlEnableRampLabel);
    _vlEnableRampButtonGroup = new QButtonGroup(this);
    _vlEnableRampButtonGroup->setExclusive(true);
    QVBoxLayout *vlEnableRamplayout = new QVBoxLayout();
    QRadioButton *vl0EnableRamp = new QRadioButton(tr("Velocity demand value shall be controlled in any other"));
    vlEnableRamplayout->addWidget(vl0EnableRamp);
    QRadioButton *vl1EnableRamp = new QRadioButton(tr("Velocity demand value shall accord with ramp output value"));
    vlEnableRamplayout->addWidget(vl1EnableRamp);
    _vlEnableRampButtonGroup->addButton(vl0EnableRamp, 0);
    _vlEnableRampButtonGroup->addButton(vl1EnableRamp, 1);
    modeControlWordLayout->addRow(vlEnableRamplayout);
    connect(_vlEnableRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { vlEnableRampClicked(id); });

    QLabel *vlUnlockRampLabel = new QLabel(tr("Unlock ramp (bit 5) :"));
    modeControlWordLayout->addRow(vlUnlockRampLabel);
    _vlUnlockRampButtonGroup = new QButtonGroup(this);
    _vlUnlockRampButtonGroup->setExclusive(true);
    QVBoxLayout *vlUnlockRamplayout = new QVBoxLayout();
    _vlUnlockRampButtonGroup = new QButtonGroup(this);
    QRadioButton *vl0UnlockRamp = new QRadioButton(tr("Ramp output value shall be locked to current output value"));
    vlUnlockRamplayout->addWidget(vl0UnlockRamp);
    QRadioButton *vl1UnlockRamp = new QRadioButton(tr("Ramp output value shall follow ramp input value"));
    vlUnlockRamplayout->addWidget(vl1UnlockRamp);
    _vlUnlockRampButtonGroup->addButton(vl0UnlockRamp, 0);
    _vlUnlockRampButtonGroup->addButton(vl1UnlockRamp, 1);
    modeControlWordLayout->addRow(vlUnlockRamplayout);
    connect(_vlUnlockRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { vlUnlockRampClicked(id); });

    QLabel *_vlReferenceRampLabel = new QLabel(tr("Reference ramp (bit 6) :"));
    modeControlWordLayout->addRow(_vlReferenceRampLabel);
    _vlReferenceRampButtonGroup = new QButtonGroup(this);
    _vlReferenceRampButtonGroup->setExclusive(true);
    QVBoxLayout *_vlReferenceRamplayout = new QVBoxLayout();
    QRadioButton *vl0ReferenceRamp = new QRadioButton(tr("Ramp input value shall be set to zero"));
    _vlReferenceRamplayout->addWidget(vl0ReferenceRamp);
    QRadioButton *vl1ReferenceRamp = new QRadioButton(tr("Ramp input value shall accord with ramp reference"));
    _vlReferenceRamplayout->addWidget(vl1ReferenceRamp);
    _vlReferenceRampButtonGroup->addButton(vl0ReferenceRamp, 0);
    _vlReferenceRampButtonGroup->addButton(vl1ReferenceRamp, 1);
    modeControlWordLayout->addRow(_vlReferenceRamplayout);
    connect(_vlReferenceRampButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { vlReferenceRampClicked(id); });

    QLabel *vlHaltLabel = new QLabel(tr("Halt ramp (bit 8) : Motor stopped"));
    modeControlWordLayout->addRow(vlHaltLabel);
    //    _vlHaltButtonGroup = new QButtonGroup(this);
    //    _vlHaltButtonGroup->setExclusive(true);
    //    QVBoxLayout *vlHaltlayout = new QVBoxLayout();
    //    QRadioButton *vl0Halt = new QRadioButton(tr("No command"));
    //    vlHaltlayout->addWidget(vl0Halt);
    //    QRadioButton *vl1Halt = new QRadioButton(tr("Motor shall be stopped"));
    //    vlHaltlayout->addWidget(vl1Halt);
    //    _vlHaltButtonGroup->addButton(vl0Halt, 0);
    //    _vlHaltButtonGroup->addButton(vl1Halt, 1);
    //    modeControlWordLayout->addRow(vlHaltlayout);
    //    connect(_vlHaltButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id){ vlHaltClicked(id); });
    modeControlWordGroupBox->setLayout(modeControlWordLayout);

    QPushButton *dataLoggerPushButton = new QPushButton(tr("Data Logger"));
    connect(dataLoggerPushButton, &QPushButton::clicked, this, &WidgetDebug::dataLogger);

    QPushButton *mappingPdoPushButton = new QPushButton(tr("Mapping Pdo"));
    connect(mappingPdoPushButton, &QPushButton::clicked, this, &WidgetDebug::pdoMapping);

    QPixmap vlModePixmap;
    QLabel *vlModeLabel;
    vlModeLabel = new QLabel();
    vlModePixmap.load(QDir::homePath() + "/Seafile/Produits/1_UMC/VLDiagram.png");
    vlModeLabel->setPixmap(vlModePixmap);
    QPushButton *imgPushButton = new QPushButton(tr("Diagram VL mode"));
    connect(imgPushButton, SIGNAL(clicked()), vlModeLabel, SLOT(show()));
    QHBoxLayout *vlButtonLayout = new QHBoxLayout();
    vlButtonLayout->addWidget(dataLoggerPushButton);
    vlButtonLayout->addWidget(mappingPdoPushButton);
    vlButtonLayout->addWidget(imgPushButton);

    thirdColumnlayout->addWidget(vlGroupBox);
    thirdColumnlayout->addWidget(modeControlWordGroupBox);
    thirdColumnlayout->addItem(vlButtonLayout);
    // END THIRD COLUMM

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->setMargin(0);
    hBoxLayout->addLayout(firstLayout);
    hBoxLayout->addLayout(secondColumnlayout);
    hBoxLayout->addLayout(thirdColumnlayout);

    setLayout(hBoxLayout);
}

void WidgetDebug::toggleStart(bool start)
{
    if (start)
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-stop.png"));
        _timer.start(_logTimerSpinBox->value());
    }
    else
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
        _timer.stop();
    }
}

void WidgetDebug::setTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _timer.start(ms);
    }
}

void WidgetDebug::readData()
{
    _node->readObject(_statusWordObjectId, 0x0);
    _node->readObject(_vlVelocityDemandObjectId, 0x0);
    _node->readObject(_vlVelocityActualObjectId, 0x0);
}

void WidgetDebug::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    int value;
    if (!_node)
    {
        return;
    }
    if (objId.index == _controlWordObjectId && objId.subIndex == 0x00)
    {
        manageNotificationControlWordObject(flags);
    }
    if (objId.index == _statusWordObjectId && objId.subIndex == 0x00)
    {
        manageNotificationStatusWordobject();
    }

    if (objId.index == _haltObjectId && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        value = _node->nodeOd()->value(_haltObjectId).toInt();
        if (_haltOptionGroup->button(value))
        {
            _haltOptionGroup->button(_node->nodeOd()->value(_haltObjectId).toInt())->setChecked(true);
        }
    }
    if (objId.index == _quickStopObjectId && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        value = _node->nodeOd()->value(_quickStopObjectId).toInt();
        if (_quickStopOptionGroup->button(value))
        {
            _quickStopOptionGroup->button(value)->setChecked(true);
        }
    }
    if (objId.index == _abortConnectionObjectId && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        value = _node->nodeOd()->value(_abortConnectionObjectId).toInt();
        if (_abortConnectionOptionGroup->button(value))
        {
            _abortConnectionOptionGroup->button(value)->setChecked(true);
        }
    }
    if (objId.index == _faultReactionObjectId && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        value = _node->nodeOd()->value(_faultReactionObjectId).toInt();
        if (_faultReactionOptionGroup->button(value))
        {
            _faultReactionOptionGroup->button(value)->setChecked(true);
        }
    }

    if (objId.index == _vlVelocityDemandObjectId && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        if (_node->nodeOd()->indexExist(_vlVelocityDemandObjectId))
        {
            value = _node->nodeOd()->value(_vlVelocityDemandObjectId).toInt();
            _vlVelocityDemandLabel->setNum(value);
        }
    }

    if (objId.index == _vlVelocityActualObjectId && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        if (_node->nodeOd()->indexExist(_vlVelocityActualObjectId))
        {
            value = _node->nodeOd()->value(_vlVelocityActualObjectId).toInt();
            _vlVelocityActualLabel->setNum(value);
        }
    }
}