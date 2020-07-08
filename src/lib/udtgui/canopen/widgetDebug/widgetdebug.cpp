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

#include <QApplication>
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
    _modesOfOperationObjectId = 0x6060;
    _modesOfOperationDisplayObjectId = 0x6061;

    createWidgets();
    setCheckableStateMachine(2);
    registerObjId({_controlWordObjectId, 0x00});
    registerObjId({_statusWordObjectId, 0x00});
    registerObjId({_modesOfOperationObjectId, 0x00});
    registerObjId({_modesOfOperationDisplayObjectId, 0x00});

    setNode(node);
}
WidgetDebug::~WidgetDebug()
{
    unRegisterFullOd();
}
void WidgetDebug::setNode(Node *node)
{
    if ((node->nodeOd()->value(0x1000, 0x0).toUInt() & 0xFFFF) != 0x192)
    {
        return;
    }

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
        setWindowTitle("402 : " + _node->name() +  ", Status :" + _node->statusStr());

        connect(_node, &Node::statusChanged, this, &WidgetDebug::updateData);
        _node->readObject(_statusWordObjectId, 0x0);
        _node->readObject(_controlWordObjectId, 0x00);
        _node->readObject(_modesOfOperationDisplayObjectId, 0x00);

        if (_node->status() != Node::STARTED)
        {
            //this->setEnabled(false);
        }
        _p402Option->setNode(_node);
        _p402vl->setNode(_node);
        _p402ip->setNode(_node);
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
            _node->readObject(_controlWordObjectId, 0x00);
        }
        else
        {
            // this->setEnabled(false);
        }
    }
}
void WidgetDebug::preop()
{
    if (_node)
    {
        _node->sendPreop();
    }
}

void WidgetDebug::start()
{
    if (_node)
    {
        _node->sendStart();
    }
}

void WidgetDebug::stop()
{
    if (_node)
    {
        _node->sendStop();
    }
}

void WidgetDebug::resetCom()
{
    if (_node)
    {
        _node->sendResetComm();
    }
}

void WidgetDebug::resetNode()
{
    if (_node)
    {
        _node->sendResetNode();
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
    cmdControlWord = (cmdControlWord & ~CW_Halt);
    _haltPushButton->setChecked(false);
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(cmdControlWord));
    _controlWordLabel->setText("0x" + QString::number(cmdControlWord, 16).toUpper());
}

void WidgetDebug::controlWordHaltClicked()
{
    _haltPushButton->setCheckable(true);
    if ((cmdControlWord & CW_Halt) != 0)
    {
        cmdControlWord = (cmdControlWord & ~CW_Halt);
        _haltPushButton->setChecked(false);
    }
    else
    {
        cmdControlWord |= CW_Halt;
        _haltPushButton->setChecked(true);
    }
    _node->writeObject(_controlWordObjectId, 0x00, QVariant(cmdControlWord));
    _controlWordLabel->setText("0x" + QString::number(cmdControlWord, 16).toUpper());
}

void WidgetDebug::gotoStateOEClicked()
{
    if (stateMachineCurrent == STATE_Fault)
    {
        stateMachineClicked(STATE_SwitchOnDisabled);
        _operationEnabledTimer.singleShot(70, this, SLOT(gotoStateOEClicked()));
    }
    if (stateMachineCurrent == STATE_SwitchOnDisabled)
    {
        stateMachineClicked(STATE_ReadyToSwitchOn);
        _operationEnabledTimer.singleShot(70, this, SLOT(gotoStateOEClicked()));
    }
    if (stateMachineCurrent == STATE_ReadyToSwitchOn)
    {
        stateMachineClicked(STATE_SwitchedOn);
        _operationEnabledTimer.singleShot(70, this, SLOT(gotoStateOEClicked()));
    }
    if (stateMachineCurrent == STATE_SwitchedOn)
    {
        stateMachineClicked(STATE_OperationEnabled);
    }
}

void WidgetDebug::manageNotificationControlWordObject(SDO::FlagsRequest flags)
{
    if (flags == SDO::FlagsRequest::Error)
    {
        _controlWordLabel->setText("Error SDO : 0x" + QString::number(_node->nodeOd()->errorObject(_controlWordObjectId, 0x0), 16));
    }
    quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toInt());
    cmdControlWord = controlWord;
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
        _haltPushButton->setEnabled(false);

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
        _haltPushButton->setEnabled(false);
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
        _haltPushButton->setEnabled(false);
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
        _haltPushButton->setEnabled(true);
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
        _haltPushButton->setEnabled(false);
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
        _haltPushButton->setEnabled(false);
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
        _haltPushButton->setEnabled(false);
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

void WidgetDebug::manageModeOfOperationObject()
{
    quint16 mode = static_cast<quint16>(_node->nodeOd()->value(_modesOfOperationDisplayObjectId).toInt());
    if (mode == 7)
    {
        _stackedWidget->setCurrentIndex(1);
    }
    else if(mode == 2)
    {
        _stackedWidget->setCurrentIndex(0);
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


void WidgetDebug::createWidgets()
{
    QLayout *Layout = new QVBoxLayout();
    Layout->setMargin(0);

    // toolbar nmt
    _nmtToolBar = new QToolBar(tr("Node commands"));
    QActionGroup *groupNmt = new QActionGroup(this);
    groupNmt->setExclusive(true);
    QAction *action;
    action = groupNmt->addAction(tr("Pre operationnal"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-connection-status-on.png"));
    action->setStatusTip(tr("Request node to go in preop mode"));
    connect(action, &QAction::triggered, this, &WidgetDebug::preop);

    action = groupNmt->addAction(tr("Start"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-play.png"));
    action->setStatusTip(tr("Request node to go in started mode"));
    connect(action, &QAction::triggered, this, &WidgetDebug::start);

    action = groupNmt->addAction(tr("Stop"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-stop.png"));
    action->setStatusTip(tr("Request node to go in stop mode"));
    connect(action, &QAction::triggered, this, &WidgetDebug::stop);

    action = groupNmt->addAction(tr("Reset communication"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-process.png"));
    action->setStatusTip(tr("Request node to reset com. parameters"));
    connect(action, &QAction::triggered, this, &WidgetDebug::resetCom);

    action = groupNmt->addAction(tr("Reset node"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-reset.png"));
    action->setStatusTip(tr("Request node to reset all values"));
    connect(action, &QAction::triggered, this, &WidgetDebug::resetNode);

    _nmtToolBar->addActions(groupNmt->actions());
    Layout->addWidget(_nmtToolBar);
    // toolbar TIMER
    _timerToolBar = new QToolBar(tr("Read Status Word"));
    // start stop
    _startStopAction = _timerToolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _startStopAction->setToolTip(tr("Start or stop"));
    connect(_startStopAction, &QAction::triggered, this, &WidgetDebug::toggleStart);
    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(500);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    _timerToolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ setTimer(i); });
    connect(&_timer, &QTimer::timeout, this, &WidgetDebug::readData);
    Layout->addWidget(_timerToolBar);

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

    _haltPushButton = new QPushButton(tr("Halt"));
    _haltPushButton->setStyleSheet("QPushButton:checked { background-color : #148CD2; }");
    _haltPushButton->setEnabled(false);
    controlWordLayout->addRow(_haltPushButton);
    controlWordGroupBox->setLayout(controlWordLayout);
    connect(_haltPushButton, &QPushButton::clicked, this, &WidgetDebug::controlWordHaltClicked);

    _controlWordLabel = new QLabel();
    controlWordLayout->addRow(tr("ControlWord sended:"), _controlWordLabel);


    QPushButton *_gotoOEPushButton = new QPushButton(tr("Operation enabled quickly"));
    controlWordLayout->addRow(_gotoOEPushButton);
    connect(_gotoOEPushButton, &QPushButton::clicked, this, &WidgetDebug::gotoStateOEClicked);

    controlWordGroupBox->setLayout(controlWordLayout);
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
    Layout->addWidget(stateMachineGroupBox);
    Layout->addWidget(controlWordGroupBox);
    Layout->addWidget(statusWordGroupBox);


    _p402Option = new P402OptionWidget() ;
    _p402vl = new P402VlWidget();
    _p402ip = new P402IpWidget();
    _stackedWidget = new QStackedWidget;
    _stackedWidget->addWidget(_p402vl);
    _stackedWidget->addWidget(_p402ip);

    QHBoxLayout *hBoxLayout = new QHBoxLayout();
    hBoxLayout->setMargin(0);
    hBoxLayout->addLayout(Layout);
    hBoxLayout->addWidget(_p402Option);
    hBoxLayout->addWidget(_stackedWidget);

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
    _p402ip->readData();
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
    if (objId.index == _modesOfOperationObjectId)
    {
        _node->readObject(_modesOfOperationDisplayObjectId, 0x00);
    }
    if (objId.index == _modesOfOperationDisplayObjectId)
    {
        manageModeOfOperationObject();
    }
}

void WidgetDebug::closeEvent(QCloseEvent *event)
{
    if (event->type() == QEvent::Close)
    {

    }
}
