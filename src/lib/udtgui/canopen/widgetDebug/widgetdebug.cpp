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

    registerObjId({_controlWordObjectId, 0x00});
    registerObjId({_statusWordObjectId, 0x00});
    registerObjId({_haltObjectId, 0x00});
    registerObjId({_quickStopObjectId, 0x00});
    registerObjId({_abortConnectionObjectId, 0x00});
    registerObjId({_faultReactionObjectId, 0x00});

    // VL_MODE
    registerObjId({0x6043, 0x00});
    registerObjId({0x6044, 0x00});

    this->setWindowTitle("402");

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
        connect(_node, &Node::statusChanged, this, &WidgetDebug::updateData);
        _node->readObject(_statusWordObjectId, 0x0);
        _node->readObject(_haltObjectId, 0x0);
        _node->readObject(_quickStopObjectId, 0x0);
        _node->readObject(_abortConnectionObjectId, 0x0);
        _node->readObject(_faultReactionObjectId, 0x0);

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
        if (_node->status() == Node::STARTED)
        {
            this->setEnabled(true);
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

    switch (id)
    {
    case 1: // 1_Not ready to switch on
        break;
    case 2: // 2_Switch on disabled
        // nothing = 0 cmdControlWord;
        break;
    case 3: // 3_Ready to switch on
        cmdControlWord |= (CW_EnableVoltage | CW_QuickStop);
        break;
    case 4: // 4_Switched on
        cmdControlWord |= CW_EnableVoltage | CW_QuickStop | CW_SwitchOn;
        break;
    case 5: // 5_Operation enabled
        cmdControlWord |= CW_EnableVoltage | CW_QuickStop | CW_SwitchOn | CW_EnableOperation;
        break;
    case 6: // 6_Quick stop active
        cmdControlWord |= CW_EnableVoltage;
        break;
    case 7: // 7_Fault reaction active
        break;
    case 8: // 8_Fault
        cmdControlWord |= CW_FaultReset;
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
    if (_vlEnableRampButtonGroup->button(_node->nodeOd()->value(_controlWordObjectId).toInt() & CW_VL_EnableRamp))
    {
        _vlEnableRampButtonGroup->button(_node->nodeOd()->value(_controlWordObjectId).toUInt() & CW_VL_EnableRamp)->setChecked(true);
    }
    if (_vlUnlockRampButtonGroup->button(_node->nodeOd()->value(_controlWordObjectId).toInt() & CW_VL_UnlockRamp))
    {
        _vlUnlockRampButtonGroup->button(_node->nodeOd()->value(_controlWordObjectId).toUInt() & CW_VL_UnlockRamp)->setChecked(true);
    }
    if (_vlReferenceRampButtonGroup->button(_node->nodeOd()->value(_controlWordObjectId).toInt() & CW_VL_ReferenceRamp))
    {
        _vlReferenceRampButtonGroup->button(_node->nodeOd()->value(_controlWordObjectId).toUInt() & CW_VL_ReferenceRamp)->setChecked(true);
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
        _statusWordLabel->setText(tr("NotReadyToSwitchOn"));
        setCheckableStateMachine(1);
    }
    if ((state & Mask1) == SW_StateSwitchOnDisabled)
    {
        _statusWordLabel->setText(tr("SwitchOnDisabled"));
        setCheckableStateMachine(2);
    }
    if ((state & Mask2) == SW_StateReadyToSwitchOn)
    {
        _statusWordLabel->setText(tr("ReadyToSwitchOn"));
        setCheckableStateMachine(3);
    }
    if ((state & Mask2) == SW_StateSwitchedOn)
    {
        _statusWordLabel->setText(tr("SwitchedOn"));
        setCheckableStateMachine(4);
    }
    if ((state & Mask2) == SW_StateOperationEnabled)
    {
        _statusWordLabel->setText(tr("OperationEnabled"));
        setCheckableStateMachine(5);
    }
    if ((state & Mask2) == SW_StateQuickStopActive)
    {
        _statusWordLabel->setText(tr("QuickStopActive"));
        setCheckableStateMachine(6);
    }
    if ((state & Mask1) == SW_StateFaultReactionActive)
    {
        _statusWordLabel->setText(tr("FaultReactionActive"));
        setCheckableStateMachine(7);
    }
    if ((state & Mask1) == SW_StateFault)
    {
        _statusWordLabel->setText(tr("Fault"));
        setCheckableStateMachine(8);
    }
    this->update();

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
void WidgetDebug::vlTargetVelocityFinished()
{
    qint16 value = static_cast<qint16>(_vlTargetVelocitySpinBox->value());
    _node->writeObject(0x6042, 0x00, QVariant(value));
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

void WidgetDebug::createWidgets()
{
    // FIRST COLUMM
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box State Machine
    QGroupBox *stateMachineGroupBox = new QGroupBox(tr("State Machine"));
    stateMachineGroupBox->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    QFormLayout *stateMachineLayoutGroupBox = new QFormLayout();

    _stateMachineGroup = new QButtonGroup(this);
    _stateMachineGroup->setExclusive(true);

    QPushButton *stateNotReadyToSwitchOnPushButton = new QPushButton(tr("1_Not ready to switch on"));
    stateMachineLayoutGroupBox->addRow(stateNotReadyToSwitchOnPushButton);
    _stateMachineGroup->addButton(stateNotReadyToSwitchOnPushButton, 1);
    QPushButton *stateSwitchOnDisabledPushButton = new QPushButton(tr("2_Switch on disabled"));
    stateMachineLayoutGroupBox->addRow(stateSwitchOnDisabledPushButton);
    _stateMachineGroup->addButton(stateSwitchOnDisabledPushButton, 2);
    QPushButton *stateReadyToSwitchOnPushButton = new QPushButton(tr("3_Ready to switch on"));
    stateMachineLayoutGroupBox->addRow(stateReadyToSwitchOnPushButton);
    _stateMachineGroup->addButton(stateReadyToSwitchOnPushButton, 3);
    QPushButton *stateSwitchedOnPushButton = new QPushButton(tr("4_Switched on"));
    stateMachineLayoutGroupBox->addRow(stateSwitchedOnPushButton);
    _stateMachineGroup->addButton(stateSwitchedOnPushButton, 4);
    QPushButton *stateOperationEnabledPushButton = new QPushButton(tr("5_Operation enabled"));
    stateMachineLayoutGroupBox->addRow(stateOperationEnabledPushButton);
    _stateMachineGroup->addButton(stateOperationEnabledPushButton, 5);
    QPushButton *stateQuickStopActivePushButton = new QPushButton(tr("6_Quick stop active"));
    stateMachineLayoutGroupBox->addRow(stateQuickStopActivePushButton);
    _stateMachineGroup->addButton(stateQuickStopActivePushButton, 6);
    QPushButton *stateFaultReactionActivePushButton = new QPushButton(tr("7_Fault reaction active"));
    stateMachineLayoutGroupBox->addRow(stateFaultReactionActivePushButton);
    _stateMachineGroup->addButton(stateFaultReactionActivePushButton, 7);
    QPushButton *stateFaultPushButton = new QPushButton(tr("8_Fault"));
    stateMachineLayoutGroupBox->addRow(stateFaultPushButton);
    _stateMachineGroup->addButton(stateFaultPushButton, 8);

    connect(_stateMachineGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { stateMachineClicked(id); });
    stateMachineGroupBox->setLayout(stateMachineLayoutGroupBox);
    // END Group Box State Machine

    // Group Box Control Word
    QGroupBox *controlWordGroupBox = new QGroupBox(tr("Control Word (0x6040)"));
    QFormLayout *controlWordLayoutGroupBox = new QFormLayout();

    QPushButton *_haltPushButton = new QPushButton(tr("Halt"));
    controlWordLayoutGroupBox->addRow(_haltPushButton);
    controlWordGroupBox->setLayout(controlWordLayoutGroupBox);

    _controlWordLabel = new QLabel();
    controlWordLayoutGroupBox->addRow(tr("ControlWord sended:"), _controlWordLabel);
    connect(_haltPushButton, &QPushButton::clicked, this, &WidgetDebug::controlWordHaltClicked);
    // END Group Box Control Word

    // Group Box Status Word
    QGroupBox *statusWordGroupBox = new QGroupBox(tr("Status Word (0x6041)"));
    QFormLayout *statusWordLayoutGroupBox = new QFormLayout();
    _statusWordRawLabel = new QLabel();
    statusWordLayoutGroupBox->addRow(tr("StatusWord raw:"), _statusWordRawLabel);
    _statusWordLabel = new QLabel();
    statusWordLayoutGroupBox->addRow(tr("StatusWord State:"), _statusWordLabel);
    _voltageEnabledLabel = new QLabel();
    statusWordLayoutGroupBox->addRow(tr("Voltage Enabled :"), _voltageEnabledLabel);
    _warningLabel = new QLabel();
    statusWordLayoutGroupBox->addRow(tr("Warning :"), _warningLabel);
    _remoteLabel = new QLabel();
    statusWordLayoutGroupBox->addRow(tr("Remote :"), _remoteLabel);
    _targetReachedLabel = new QLabel();
    statusWordLayoutGroupBox->addRow(tr("Target Reached :"), _targetReachedLabel);
    _internalLimitActiveLabel = new QLabel();
    statusWordLayoutGroupBox->addRow(tr("Internal Limit Active :"), _internalLimitActiveLabel);
    _operationModeSpecificLabel = new QLabel();
    statusWordLayoutGroupBox->addRow(tr("Operation Mode Specific:"), _operationModeSpecificLabel);
    _manufacturerSpecificLabel = new QLabel();
    statusWordLayoutGroupBox->addRow(tr("Manufacturer Specific:"), _manufacturerSpecificLabel);
    statusWordGroupBox->setLayout(statusWordLayoutGroupBox);
    // END Group Box Status Word
    layout->addWidget(stateMachineGroupBox);
    layout->addWidget(controlWordGroupBox);
    layout->addWidget(statusWordGroupBox);
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
    QGroupBox *vlGroupBox = new QGroupBox(tr("VL mode"));
    QFormLayout *vlLayout = new QFormLayout();

    _vlTargetVelocitySpinBox = new QSpinBox();
    _vlTargetVelocitySpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    vlLayout->addRow("Target velocity (0x6042) :", _vlTargetVelocitySpinBox);

    QSlider *vlTargetVelocitySlider = new QSlider(Qt::Horizontal);
    vlTargetVelocitySlider->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    vlLayout->addRow(vlTargetVelocitySlider);

    connect(vlTargetVelocitySlider, &QSlider::valueChanged, _vlTargetVelocitySpinBox, &QSpinBox::setValue);
    connect(_vlTargetVelocitySpinBox, qOverload<int>(&QSpinBox::valueChanged), vlTargetVelocitySlider, &QSlider::setValue);
    connect(vlTargetVelocitySlider, &QSlider::valueChanged, this, &WidgetDebug::vlTargetVelocityFinished);
    connect(_vlTargetVelocitySpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlTargetVelocityFinished);

    _vlVelocityDemandLabel = new QLabel();
    _vlVelocityDemandLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vlLayout->addRow("Velocity_demand (0x6043) :", _vlVelocityDemandLabel);

    _vlVelocityActualLabel = new QLabel();
    _vlVelocityActualLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    vlLayout->addRow("Velocity_actual_value (0x6044) :", _vlVelocityActualLabel);

    QLabel *vlVelocityMinMaxAmountLabel = new QLabel(tr("vl velocity min max amount (0x6046) :"));
    vlLayout->addRow(vlVelocityMinMaxAmountLabel);
    QLayout *vlVelocityMinMaxAmountlayout = new QHBoxLayout();
    _vlMinVelocityMinMaxAmountSpinBox = new QSpinBox();
    _vlMinVelocityMinMaxAmountSpinBox->setToolTip("min ");
    _vlMinVelocityMinMaxAmountSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<quint32>::min());
    vlVelocityMinMaxAmountlayout->addWidget(_vlMinVelocityMinMaxAmountSpinBox);
    _vlMaxVelocityMinMaxAmountSpinBox = new QSpinBox();
    _vlMaxVelocityMinMaxAmountSpinBox->setToolTip("max ");
    _vlMaxVelocityMinMaxAmountSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<quint32>::min());
    vlVelocityMinMaxAmountlayout->addWidget(_vlMaxVelocityMinMaxAmountSpinBox);
    vlLayout->addRow(vlVelocityMinMaxAmountlayout);
    connect(_vlMinVelocityMinMaxAmountSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlMinAmountEditingFinished);
    connect(_vlMaxVelocityMinMaxAmountSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlMaxAmountEditingFinished);

    QLabel *vlVelocityAccelerationLabel = new QLabel(tr("vl velocity acceleration (0x6048) :"));
    vlLayout->addRow(vlVelocityAccelerationLabel);
    QLayout *vlVelocityAccelerationlayout = new QHBoxLayout();
    _vlAccelerationDeltaSpeedSpinBox = new QSpinBox();
    _vlAccelerationDeltaSpeedSpinBox->setToolTip("Delta Speed");
    _vlAccelerationDeltaSpeedSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<quint32>::min());
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaSpeedSpinBox);
    _vlAccelerationDeltaTimeSpinBox = new QSpinBox();
    _vlAccelerationDeltaTimeSpinBox->setToolTip("Delta Time");
    _vlAccelerationDeltaTimeSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<quint32>::min());
    vlVelocityAccelerationlayout->addWidget(_vlAccelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityAccelerationlayout);
    connect(_vlAccelerationDeltaSpeedSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlAccelerationDeltaSpeedEditingFinished);
    connect(_vlAccelerationDeltaTimeSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlAccelerationDeltaTimeEditingFinished);

    QLabel *vlVelocityDecelerationLabel = new QLabel(tr("vl velocity deceleration (0x6049) :"));
    vlLayout->addRow(vlVelocityDecelerationLabel);
    QLayout *vlVelocityDecelerationlayout = new QHBoxLayout();
    _vlDecelerationDeltaSpeedDecelerationSpinBox = new QSpinBox();
    _vlDecelerationDeltaSpeedDecelerationSpinBox->setToolTip("Delta Speed");
    _vlDecelerationDeltaSpeedDecelerationSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<quint32>::min());
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaSpeedDecelerationSpinBox);
    _vlDecelerationDeltaTimeSpinBox = new QSpinBox();
    _vlDecelerationDeltaTimeSpinBox->setToolTip("Delta Time");
    _vlDecelerationDeltaTimeSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<quint32>::min());
    vlVelocityDecelerationlayout->addWidget(_vlDecelerationDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityDecelerationlayout);
    connect(_vlDecelerationDeltaSpeedDecelerationSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlDecelerationDeltaSpeedEditingFinished);
    connect(_vlDecelerationDeltaTimeSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlDecelerationDeltaTimeEditingFinished);

    QLabel *vlVelocityQuickStopLabel = new QLabel(tr("vl velocity quick stop (0x604A) :"));
    vlLayout->addRow(vlVelocityQuickStopLabel);
    QLayout *vlVelocityQuickStoplayout = new QHBoxLayout();
    _vlQuickStopDeltaSpeedSpinBox = new QSpinBox();
    _vlQuickStopDeltaSpeedSpinBox->setToolTip("Delta Speed");
    _vlQuickStopDeltaSpeedSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<quint32>::min());
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaSpeedSpinBox);
    _vlQuickStopDeltaTimeSpinBox = new QSpinBox();
    _vlQuickStopDeltaTimeSpinBox->setToolTip("Delta Time");
    _vlQuickStopDeltaTimeSpinBox->setRange(std::numeric_limits<quint32>::min(), std::numeric_limits<quint32>::min());
    vlVelocityQuickStoplayout->addWidget(_vlQuickStopDeltaTimeSpinBox);
    vlLayout->addRow(vlVelocityQuickStoplayout);
    connect(_vlQuickStopDeltaSpeedSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlQuickStopDeltaSpeedEditingFinished);
    connect(_vlQuickStopDeltaTimeSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlQuickStopDeltaTimeEditingFinished);

    QLabel *vlSetPointFactorLabel = new QLabel(tr("vl set-point factor (0x604B) :"));
    vlLayout->addRow(vlSetPointFactorLabel);
    QLayout *vlSetPointFactorlayout = new QHBoxLayout();
    _vlSetPointFactorNumeratorSpinBox = new QSpinBox();
    _vlSetPointFactorNumeratorSpinBox->setToolTip("Numerator");
    _vlSetPointFactorNumeratorSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorNumeratorSpinBox);
    _vlSetPointFactorDenominatorSpinBox = new QSpinBox();
    _vlSetPointFactorDenominatorSpinBox->setToolTip("Denominator");
    _vlSetPointFactorDenominatorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
    vlSetPointFactorlayout->addWidget(_vlSetPointFactorDenominatorSpinBox);
    vlLayout->addRow(vlSetPointFactorlayout);
    connect(_vlSetPointFactorNumeratorSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlSetPointFactorNumeratorEditingFinished);
    connect(_vlSetPointFactorDenominatorSpinBox, &QSpinBox::editingFinished, this, &WidgetDebug::vlSetPointFactorDenominatorEditingFinished);

    QLabel *vlDimensionFactorLabel = new QLabel(tr("vl dimension factor (0x604C) :"));
    vlLayout->addRow(vlDimensionFactorLabel);
    QLayout *vlDimensionFactorlayout = new QHBoxLayout();
    _vlDimensionFactorNumeratorSpinBox = new QSpinBox();
    _vlDimensionFactorNumeratorSpinBox->setToolTip("Numerator");
    _vlDimensionFactorNumeratorSpinBox->setRange(std::numeric_limits<qint16>::min(), std::numeric_limits<qint16>::max());
    vlDimensionFactorlayout->addWidget(_vlDimensionFactorNumeratorSpinBox);
    _vlDimensionFactorDenominatorSpinBox = new QSpinBox();
    _vlDimensionFactorDenominatorSpinBox->setToolTip("Denominator");
    _vlDimensionFactorDenominatorSpinBox->setRange(1, std::numeric_limits<qint16>::max());
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

    QPixmap vlModePixmap;
    QLabel *vlModeLabel;
    vlModeLabel = new QLabel();
    vlModePixmap.load(QDir::homePath() + "/Seafile/Produits/1_UMC/VLDiagram.png");
    vlModeLabel->setPixmap(vlModePixmap);

    QPushButton *imgPushButton = new QPushButton(tr("Diagram VL mode"));
    connect(imgPushButton, SIGNAL(clicked()), vlModeLabel, SLOT(show()));

    thirdColumnlayout->addWidget(vlGroupBox);
    thirdColumnlayout->addWidget(modeControlWordGroupBox);
    thirdColumnlayout->addWidget(imgPushButton);
    // END THIRD COLUMM

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->setMargin(0);
    hBoxLayout->addLayout(layout);
    hBoxLayout->addLayout(secondColumnlayout);
    hBoxLayout->addLayout(thirdColumnlayout);

    setLayout(hBoxLayout);
}

void WidgetDebug::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
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
        if (_haltOptionGroup->button(_node->nodeOd()->value(_haltObjectId).toInt()))
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
        if (_quickStopOptionGroup->button(_node->nodeOd()->value(_quickStopObjectId).toInt()))
        {
            _quickStopOptionGroup->button(_node->nodeOd()->value(_quickStopObjectId).toInt())->setChecked(true);
        }
    }
    if (objId.index == _abortConnectionObjectId && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        if (_abortConnectionOptionGroup->button(_node->nodeOd()->value(_abortConnectionObjectId).toInt()))
        {
            _abortConnectionOptionGroup->button(_node->nodeOd()->value(_abortConnectionObjectId).toInt())->setChecked(true);
        }
    }
    if (objId.index == _faultReactionObjectId && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        if (_faultReactionOptionGroup->button(_node->nodeOd()->value(_faultReactionObjectId).toInt()))
        {
            _faultReactionOptionGroup->button(_node->nodeOd()->value(_faultReactionObjectId).toInt())->setChecked(true);
        }
    }

    if (objId.index == 0x6042 && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        if (_node->nodeOd()->indexExist(0x6042))
        {
            _vlVelocityDemandLabel->setNum(_node->nodeOd()->value(0x6042).toInt());
        }
    }

    if (objId.index == 0x6043 && objId.subIndex == 0x00)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        if (_node->nodeOd()->indexExist(0x6043))
        {
            _vlVelocityActualLabel->setNum(_node->nodeOd()->value(0x6043).toInt());
        }
    }
}
