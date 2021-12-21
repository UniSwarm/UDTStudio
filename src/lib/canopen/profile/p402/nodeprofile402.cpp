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

#include "nodeprofile402.h"

#include "indexdb402.h"
#include "modecp.h"
#include "modedty.h"
#include "modeip.h"
#include "modepp.h"
#include "modetq.h"
#include "modecstca.h"
#include "modevl.h"
#include "node.h"
#include "nodeobjectid.h"
#include "nodeodsubscriber.h"

#define TIMER_READ_MODE_OPERATION_DISPLAY 100u
#define STATE_MACHINE_REQUESTED_ATTEMPT   10

enum ControlWord : quint16
{
    CW_SwitchOn = 0x01,
    CW_EnableVoltage = 0x02,
    CW_QuickStop = 0x04,
    CW_EnableOperation = 0x08,
    CW_FaultReset = 0x80,
    CW_Halt = 0x100,
    CW_OperationModeSpecific = 0x270,
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
    SW_StateMask1 = 0x4F,
    SW_StateMask2 = 0x6F,

    SW_EventVoltageEnabled = 0x10,
    SW_EventWarning = 0x80,
    SW_EventRemote = 0x200,
    SW_EventTargetReached = 0x400,
    SW_EventSetPointAcknowledgeIpModeActive = 0x1000,
    SW_EventFollowingError = 0x2000,  // -> specific ip
    SW_EventInternalLimitActive = 0x800,
    SW_EventMask = 0xE90,

    SW_OperationModeSpecific = 0x3000,
    SW_ManufacturerSpecific = 0xC000
};

enum SupportedDriveModes : quint32
{
    SDM_PP = 0x1,
    SDM_VL = 0x2,
    SDM_PV = 0x4,
    SDM_TQ = 0x8,
    SDM_HM = 0x20,
    SDM_IP = 0x40,
    SDM_CSP = 0x80,
    SDM_CSV = 0x100,
    SDM_CST = 0x200,
    SDM_CSTCA = 0x400,
    SDM_DTY = 0x10000,
    SDM_CP = 0x100000
};

NodeProfile402::NodeProfile402(Node *node, uint8_t axis)
    : NodeProfile(node)
{
    if (axis > 8)
    {
        return;
    }

    _axisId = axis;
    _modeCurrent = NoMode;
    _modeRequested = NoMode;
    _modeStatus = MODE_CHANGED;

    _stateState = NONE_STATE;
    _stateMachineRequested = State402::STATE_NotReadyToSwitchOn;
    _stateMachineCurrent = State402::STATE_NotReadyToSwitchOn;

    _nodeProfileState = State::NODEPROFILE_STOPED;

    connect(&_nodeProfleTimer, &QTimer::timeout, this, &NodeProfile402::readRealTimeObjects);

    setNodeInterrest(node);

    initializeObjectsId();

    decodeSupportedDriveModes(_node->nodeOd()->value(_supportedDriveModesObjectId).toUInt());

    connect(_node, &Node::statusChanged, this, &NodeProfile402::statusNodeChanged);

    _modes.insert(IP, new ModeIp(this));
    _modes.insert(TQ, new ModeTq(this));
    _modes.insert(DTY, new ModeDty(this));
    _modes.insert(VL, new ModeVl(this));
    _modes.insert(PP, new ModePp(this));
    _modes.insert(CP, new ModeCp(this));
    _modes.insert(CSTCA, new ModeCstca(this));

    _controlWord = 0;
    _stateCountMachineRequested = STATE_MACHINE_REQUESTED_ATTEMPT;
}

void NodeProfile402::init()
{
    _node->readObject(_controlWordObjectId);
    _node->readObject(_modesOfOperationDisplayObjectId);
}

void NodeProfile402::setTarget(qint32 target)
{
    if (_modeCurrent != OperationMode::NoMode)
    {
        _modes[_modeCurrent]->setTarget(target);
    }
}

bool NodeProfile402::toggleHalt()
{
    if (_nodeProfileState == State::NODEPROFILE_STOPED)
    {
        return false;
    }

    quint16 cmdControlWord;
    if (_stateMachineCurrent == STATE_OperationEnabled)
    {
        if ((_controlWord & CW_Halt) != 0)
        {
            cmdControlWord = (_controlWord & ~CW_Halt);
        }
        else
        {
            cmdControlWord = _controlWord | CW_Halt;
        }
        _node->writeObject(_controlWordObjectId, QVariant(cmdControlWord));
        return true;
    }
    return true;
}

void NodeProfile402::setDefaultValueOfMode()
{
    if (_modeCurrent != OperationMode::NoMode)
    {
        _modes[_modeCurrent]->setCwDefaultflag();
    }
}

NodeProfile402::OperationMode NodeProfile402::actualMode()
{
    if ((_modeCurrent == CP) || (_modeCurrent == DTY) || (_modeCurrent == NoMode) || (_modeCurrent == PP) || (_modeCurrent == VL) || (_modeCurrent == PV) || (_modeCurrent == TQ)
        || (_modeCurrent == HM) || (_modeCurrent == IP) || (_modeCurrent == CSP) || (_modeCurrent == CSV) || (_modeCurrent == CST) || (_modeCurrent == CSTCA))
    {
        return static_cast<OperationMode>(_modeCurrent);
    }
    else if (_modeCurrent < -1)
    {
        return OperationMode::MS;
    }
    else
    {
        return OperationMode::Reserved;
    }
}

bool NodeProfile402::setMode(OperationMode mode)
{
    if (_modeCurrent == mode)
    {
        return true;
    }

    if ((mode == CP) || (mode == DTY) || (mode == NoMode) || (mode == PP) || (mode == VL) || (mode == TQ) ||  (mode == IP) || (mode == CSTCA))
    {
        _node->writeObject(_modesOfOperationObjectId, QVariant(mode));
        _modeStatus = MODE_CHANGING;
        _modeRequested = mode;
        return true;
    }
    else
    {
        return false;
    }
}

QString NodeProfile402::modeStr(NodeProfile402::OperationMode mode)
{
    switch (mode)
    {
        case OperationMode::CP:
            return tr("Continuous position (CP)");

        case OperationMode::DTY:
            return tr("Duty cycle (DTY)");

        case OperationMode::NoMode:
            return tr("No mode");

        case OperationMode::PP:
            return tr("Profile position (PP)");

        case OperationMode::VL:
            return tr("Velocity (VL)");

        case OperationMode::PV:
            return tr("Profile velocity (PV)");

        case OperationMode::TQ:
            return tr("Torque profile(TQ)");

        case OperationMode::HM:
            return tr("Homing (HM)");

        case OperationMode::IP:
            return tr("Interpolated position (IP)");

        case OperationMode::CSP:
            return tr("Cyclic sync position (CSP)");

        case OperationMode::CSV:
            return tr("Cyclic sync velocity (CSV)");

        case OperationMode::CST:
            return tr("Cyclic sync torque (CST)");

        case OperationMode::CSTCA:
            return tr("Cyclic sync torque with angle (CSTCA)");

        default:
            if (_modeCurrent < -1)
            {
                return tr("Manufacturer-specific");
            }
            else
            {
                return tr("Reserved");
            }
    }
}

bool NodeProfile402::isModeSupported(OperationMode mode)
{
    for (const OperationMode &modesupported : qAsConst(_modesSupported))
    {
        if (modesupported == mode)
        {
            return true;
        }
    }
    return false;
}

QList<NodeProfile402::OperationMode> NodeProfile402::modesSupported()
{
    return _modesSupported;
}

QList<NodeProfile402::OperationMode> NodeProfile402::modesSupportedByType(IndexDb402::OdMode402 mode)
{
    QList<NodeProfile402::OperationMode> modes;
    for (const OperationMode &modesupported : qAsConst(_modesSupported))
    {
        if (mode == IndexDb402::MODE402_TORQUE && (modesupported == OperationMode::TQ || modesupported == OperationMode::CST || modesupported == OperationMode::CSTCA))
        {
            modes.append(modesupported);
        }
        else if (mode == IndexDb402::MODE402_VELOCITY && (modesupported == OperationMode::VL || modesupported == OperationMode::PV || modesupported == OperationMode::CSV))
        {
            modes.append(modesupported);
        }
        else if (mode == IndexDb402::MODE402_POSITION
                 && (modesupported == OperationMode::PP || modesupported == OperationMode::IP || modesupported == OperationMode::CP || modesupported == OperationMode::CSP))
        {
            modes.append(modesupported);
        }
        else if (mode == IndexDb402::MODE402_OTHER && (modesupported == OperationMode::DTY))
        {
            modes.append(modesupported);
        }
    }
    return modes;
}

Mode *NodeProfile402::mode(NodeProfile402::OperationMode mode) const
{
    return _modes[mode];
}

NodeProfile402::State402 NodeProfile402::currentState() const
{
    return _stateMachineCurrent;
}

void NodeProfile402::goToState(const State402 state)
{
    if (_nodeProfileState == State::NODEPROFILE_STOPED)
    {
        return;
    }

    if (_node->status() != Node::STARTED)
    {
        return;
    }
    _stateState = STATE_CHANGE;
    _stateMachineRequested = state;
    changeStateMachine(_stateMachineRequested);
}

QString NodeProfile402::stateStr(const State402 state) const
{
    switch (state)
    {
        case State402::STATE_NotReadyToSwitchOn:
            return tr("1_Not ready to switch on");

        case State402::STATE_SwitchOnDisabled:
            return tr("2_Switch on disabled");

        case State402::STATE_ReadyToSwitchOn:
            return tr("3_Ready to switch on");

        case State402::STATE_SwitchedOn:
            return tr("4_Switched on");

        case State402::STATE_OperationEnabled:
            return tr("5_Operation enabled");

        case State402::STATE_QuickStopActive:
            return tr("6_Quick stop active");

        case State402::STATE_FaultReactionActive:
            return tr("7_Fault reaction active");

        case State402::STATE_Fault:
            return tr("8_Fault");
    }
    return QString();
}

QString NodeProfile402::event402Str(quint8 event402) const
{
    switch (event402)
    {
        case Event402::None:
            return tr("None");

        case Event402::InternalLimitActive:
            return tr("Internal Limit Active");

        case Event402::Warning:
            return tr("Warning");

        case Event402::FollowingError:
            return tr("Following Error");

        case Event402::VoltageEnabled:
            return tr("Voltage Enabled");

        case Event402::Remote:
            return tr("Remote");

        case Event402::TargetReached:
            return tr("Target Reached");

        case Event402::ModeSpecific:
            if (_modeCurrent == OperationMode::PP)
            {
                return tr("Set-point acknowledge");
            }
            else if (_modeCurrent == OperationMode::IP)
            {
                return tr("Interpolation active");
            }
            else
            {
                return QString();
            }

        default:
            return QString();
    }
}

// be used only for profile position (pp) mode and cyclic sync position mode (csp).
void NodeProfile402::setPolarityPosition(bool polarity)
{
    quint8 value = static_cast<quint8>(_node->nodeOd()->value(_fgPolaritybjectId).toInt());
    value = (value & ~MASK_POLARITY_POSITION) | (MASK_POLARITY_POSITION & static_cast<uint8_t>(MASK_POLARITY_POSITION * polarity));
    _node->writeObject(_fgPolaritybjectId, QVariant(value));
}

void NodeProfile402::setPolarityVelocity(bool polarity)
{
    quint8 value = static_cast<quint8>(_node->nodeOd()->value(_fgPolaritybjectId).toInt());
    value = (value & ~MASK_POLARITY_VELOCITY) | (MASK_POLARITY_VELOCITY & static_cast<uint8_t>(MASK_POLARITY_VELOCITY * polarity));
    _node->writeObject(_fgPolaritybjectId, QVariant(value));
}

bool NodeProfile402::polarityPosition() const
{
    bool value = (static_cast<quint8>(_node->nodeOd()->value(_fgPolaritybjectId).toInt()) & MASK_POLARITY_POSITION) >> 7;
    return value;
}

bool NodeProfile402::polarityVelocity() const
{
    bool value = (static_cast<quint8>(_node->nodeOd()->value(_fgPolaritybjectId).toInt()) & MASK_POLARITY_VELOCITY) >> 6;
    return value;
}

const NodeObjectId &NodeProfile402::modesOfOperationObjectId() const
{
    return _modesOfOperationObjectId;
}

const NodeObjectId &NodeProfile402::modesOfOperationDisplayObjectId() const
{
    return _modesOfOperationDisplayObjectId;
}

const NodeObjectId &NodeProfile402::supportedDriveModesObjectId() const
{
    return _supportedDriveModesObjectId;
}

const NodeObjectId &NodeProfile402::controlWordObjectId() const
{
    return _controlWordObjectId;
}

const NodeObjectId &NodeProfile402::statusWordObjectId() const
{
    return _statusWordObjectId;
}

const NodeObjectId &NodeProfile402::fgPolaritybjectId() const
{
    return _fgPolaritybjectId;
}

const NodeObjectId &NodeProfile402::abortConnectionObjectId() const
{
    return _abortConnectionObjectId;
}

const NodeObjectId &NodeProfile402::quickStopObjectId() const
{
    return _quickStopObjectId;
}

const NodeObjectId &NodeProfile402::shutdownObjectId() const
{
    return _shutdownObjectId;
}

const NodeObjectId &NodeProfile402::disableObjectId() const
{
    return _disableObjectId;
}

const NodeObjectId &NodeProfile402::haltObjectId() const
{
    return _haltObjectId;
}

const NodeObjectId &NodeProfile402::faultReactionObjectId() const
{
    return _faultReactionObjectId;
}

void NodeProfile402::readOptionObjects() const
{
    for (const NodeObjectId &optionObjectId : qAsConst(_optionObjectIds))
    {
        _node->readObject(optionObjectId);
    }
}

NodeProfile402::ModeStatus NodeProfile402::modeStatus() const
{
    return _modeStatus;
}

void NodeProfile402::initializeObjectsId()
{
    _modesOfOperationObjectId = IndexDb402::getObjectId(IndexDb402::OD_MODES_OF_OPERATION, _axisId);
    _modesOfOperationObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_modesOfOperationObjectId);

    _modesOfOperationDisplayObjectId = IndexDb402::getObjectId(IndexDb402::OD_MODES_OF_OPERATION_DISPLAY, _axisId);
    _modesOfOperationDisplayObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_modesOfOperationDisplayObjectId);

    _supportedDriveModesObjectId = IndexDb402::getObjectId(IndexDb402::OD_SUPPORTED_DRIVE_MODES, _axisId);
    _supportedDriveModesObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _supportedDriveModesObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_supportedDriveModesObjectId);

    _controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD, _axisId);
    _controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _controlWordObjectId.setDataType(QMetaType::Type::UShort);
    registerObjId(_controlWordObjectId);

    _statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD, _axisId);
    _statusWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _statusWordObjectId.setDataType(QMetaType::Type::UShort);
    registerObjId(_statusWordObjectId);

    // Specific
    _fgPolaritybjectId = IndexDb402::getObjectId(IndexDb402::OD_FG_POLARITY, _axisId);
    _fgPolaritybjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_fgPolaritybjectId);
    _optionObjectIds.append(_fgPolaritybjectId);

    _abortConnectionObjectId = IndexDb402::getObjectId(IndexDb402::OD_ABORT_CONNECTION_OPTION, _axisId);
    _abortConnectionObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_abortConnectionObjectId);
    _optionObjectIds.append(_abortConnectionObjectId);

    _quickStopObjectId = IndexDb402::getObjectId(IndexDb402::OD_QUICK_STOP_OPTION, _axisId);
    _quickStopObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_quickStopObjectId);
    _optionObjectIds.append(_quickStopObjectId);

    _shutdownObjectId = IndexDb402::getObjectId(IndexDb402::OD_SHUTDOWN_OPTION, _axisId);
    _shutdownObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_shutdownObjectId);
    _optionObjectIds.append(_shutdownObjectId);

    _disableObjectId = IndexDb402::getObjectId(IndexDb402::OD_DISABLE_OPERATION_OPTION, _axisId);
    _disableObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_disableObjectId);
    _optionObjectIds.append(_disableObjectId);

    _haltObjectId = IndexDb402::getObjectId(IndexDb402::OD_HALT_OPTION, _axisId);
    _haltObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_haltObjectId);
    _optionObjectIds.append(_haltObjectId);

    _faultReactionObjectId = IndexDb402::getObjectId(IndexDb402::OD_FAULT_REACTION_OPTION, _axisId);
    _faultReactionObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId(_faultReactionObjectId);
    _optionObjectIds.append(_faultReactionObjectId);
}

void NodeProfile402::statusNodeChanged(Node::Status status)
{
    if (_nodeProfileState == State::NODEPROFILE_STOPED)
    {
        return;
    }
    if (status == Node::STARTED)
    {
        _node->readObject(_modesOfOperationDisplayObjectId);
    }

    _node->readObject(_statusWordObjectId);
}

void NodeProfile402::changeStateMachine(const State402 state)
{
    if (_nodeProfileState == State::NODEPROFILE_STOPED)
    {
        return;
    }
    if (!_node)
    {
        return;
    }
    if (_node->status() != Node::STARTED)
    {
        return;
    }

    if (_stateMachineCurrent != STATE_NotReadyToSwitchOn)  // for not send cw = 0 during init
    {
        // Reset specific flag of mode
        _controlWord = (_controlWord & ~CW_OperationModeSpecific);
        // Apply specific flag of mode
        if (_modeCurrent != OperationMode::NoMode)
        {
            _controlWord |= _modes[_modeCurrent]->getSpecificCwFlag();
        }
    }

    switch (_stateMachineCurrent)
    {
        case STATE_NotReadyToSwitchOn:
        case STATE_SwitchOnDisabled:
            if ((state == STATE_ReadyToSwitchOn) || (state == STATE_OperationEnabled))  // state == STATE_OperationEnabled -> For OperatiobEnabled Quickly
            {
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord |= (CW_EnableVoltage | CW_QuickStop);
            }
            break;

        case STATE_ReadyToSwitchOn:
            if (state == STATE_SwitchOnDisabled)
            {
                _controlWord = (_controlWord & ~CW_Mask);
            }
            else if (state == STATE_SwitchedOn)
            {
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn);
            }
            else if (state == STATE_OperationEnabled)
            {
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn);
            }
            break;

        case STATE_SwitchedOn:
            if (state == STATE_OperationEnabled)
            {
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn | CW_EnableOperation);
            }
            if (state == STATE_ReadyToSwitchOn)
            {
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord |= (CW_EnableVoltage | CW_QuickStop);
            }
            if (state == STATE_SwitchOnDisabled)
            {
                _controlWord = (_controlWord & ~CW_Mask);
            }
            break;

        case STATE_OperationEnabled:
            if (state == STATE_SwitchOnDisabled)
            {
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord = (_controlWord & ~CW_Mask);
            }
            if (state == STATE_ReadyToSwitchOn)
            {
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord |= (CW_EnableVoltage | CW_QuickStop);
            }
            if (state == STATE_SwitchedOn)
            {
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn);
            }
            if (state == STATE_QuickStopActive)
            {
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord |= (CW_EnableVoltage);
                qint16 quickStopOption = static_cast<qint16>(_node->nodeOd()->value(IndexDb402::getObjectId(IndexDb402::OD_QUICK_STOP_OPTION, _axisId)).toUInt());
                if ((quickStopOption == 1) || (quickStopOption == 2))
                {
                    _stateMachineRequested = STATE_SwitchOnDisabled;
                }
            }
            break;

        case STATE_QuickStopActive:
            if (state == STATE_SwitchOnDisabled || (state == STATE_OperationEnabled))  // state == STATE_OperationEnabled -> For OperatiobEnabled Quickly
            {
                _controlWord = (_controlWord & ~CW_Mask);
            }
            break;

        case STATE_FaultReactionActive:
            break;

        case STATE_Fault:
            if (state >= STATE_SwitchOnDisabled)
            {
                _node->sendStart();
                _controlWord = (_controlWord & ~CW_Mask);
                _controlWord |= (CW_FaultReset);
            }
            break;
    }

    _controlWord = (_controlWord & ~CW_Halt);
    _node->writeObject(_controlWordObjectId, QVariant(_controlWord));
}

void NodeProfile402::decodeEventStatusWord(quint16 statusWord)
{
    // TODO miss decode specific bit from mode
    quint8 eventStatusWord = 0;
    if (statusWord & SW_EventVoltageEnabled)
    {
        eventStatusWord += VoltageEnabled;
    }
    if (statusWord & SW_EventRemote)
    {
        eventStatusWord += Remote;
    }
    if (statusWord & SW_EventTargetReached)
    {
        eventStatusWord += TargetReached;
    }
    if (statusWord & SW_EventInternalLimitActive)
    {
        eventStatusWord += InternalLimitActive;
    }
    if (statusWord & SW_EventWarning)
    {
        eventStatusWord += Warning;
    }
    if (statusWord & SW_EventFollowingError)
    {
        eventStatusWord += FollowingError;
    }
    if (statusWord & SW_EventSetPointAcknowledgeIpModeActive)
    {
        eventStatusWord += ModeSpecific;
    }
    if (eventStatusWord != _statusWordEvent)
    {
        _statusWordEvent = eventStatusWord;
        emit eventHappened(_statusWordEvent);
    }
}

void NodeProfile402::decodeStateMachineStatusWord(quint16 statusWord)
{
    State402 stateMachine = STATE_NotReadyToSwitchOn;
    if ((statusWord & SW_StateMask1) == SW_StateNotReadyToSwitchOn)
    {
        stateMachine = STATE_NotReadyToSwitchOn;
    }
    else if ((statusWord & SW_StateMask1) == SW_StateSwitchOnDisabled)
    {
        stateMachine = STATE_SwitchOnDisabled;
        if ((_controlWord & CW_Mask) == CW_FaultReset)
        {
            _controlWord = (_controlWord & ~CW_FaultReset);
            _node->writeObject(_controlWordObjectId, QVariant(_controlWord));
        }
    }
    else if ((statusWord & SW_StateMask2) == SW_StateReadyToSwitchOn)
    {
        stateMachine = STATE_ReadyToSwitchOn;
    }
    else if ((statusWord & SW_StateMask2) == SW_StateSwitchedOn)
    {
        stateMachine = STATE_SwitchedOn;
    }
    else if ((statusWord & SW_StateMask2) == SW_StateOperationEnabled)
    {
        stateMachine = STATE_OperationEnabled;
    }
    else if ((statusWord & SW_StateMask2) == SW_StateQuickStopActive)
    {
        stateMachine = STATE_QuickStopActive;
    }
    else if ((statusWord & SW_StateMask1) == SW_StateFaultReactionActive)
    {
        stateMachine = STATE_FaultReactionActive;
    }
    else if ((statusWord & SW_StateMask1) == SW_StateFault)
    {
        stateMachine = STATE_Fault;
    }

    if (stateMachine != _stateMachineCurrent)
    {
        _stateMachineCurrent = stateMachine;
        emit stateChanged();
    }

    if ((_stateState == STATE_CHANGE) && (_stateMachineRequested != _stateMachineCurrent))
    {
        changeStateMachine(_stateMachineRequested);
        _stateCountMachineRequested--;
        if (!_stateCountMachineRequested)
        {
            _stateState = NONE_STATE;
            _stateMachineRequested = _stateMachineCurrent;
            _stateCountMachineRequested = STATE_MACHINE_REQUESTED_ATTEMPT;
        }

        return;
    }
    else
    {
        _stateState = NONE_STATE;
        _stateMachineRequested = _stateMachineCurrent;
        _stateCountMachineRequested = STATE_MACHINE_REQUESTED_ATTEMPT;
    }
}

void NodeProfile402::decodeSupportedDriveModes(quint32 supportedDriveModes)
{
    _modesSupported.clear();
    if ((supportedDriveModes & 0x7EF) == 0)
    {
        _modesSupported.append(OperationMode::NoMode);
        return;
    }
    for (quint32 i = 0; i <= 31; i++)
    {
        switch ((supportedDriveModes & (1 << i)))
        {
            case SDM_PP:
                _modesSupported.append(OperationMode::PP);
                break;

            case SDM_VL:
                _modesSupported.append(OperationMode::VL);
                break;

            case SDM_PV:
                _modesSupported.append(OperationMode::PV);
                break;

            case SDM_TQ:
                _modesSupported.append(OperationMode::TQ);
                break;

            case SDM_HM:
                _modesSupported.append(OperationMode::HM);
                break;

            case SDM_IP:
                _modesSupported.append(OperationMode::IP);
                break;

            case SDM_CSP:
                _modesSupported.append(OperationMode::CSP);
                break;

            case SDM_CSV:
                _modesSupported.append(OperationMode::CSV);
                break;

            case SDM_CST:
                _modesSupported.append(OperationMode::CST);
                break;

            case SDM_CSTCA:
                _modesSupported.append(OperationMode::CSTCA);
                break;

            case SDM_DTY:
                _modesSupported.append(OperationMode::DTY);
                break;

            case SDM_CP:
                _modesSupported.append(OperationMode::CP);
                break;
        }
    }
    emit supportedDriveModesUdpdated();
}

void NodeProfile402::readModeOfOperationDisplay()
{
    _node->readObject(_modesOfOperationDisplayObjectId);
}

void NodeProfile402::start(int msec)
{
    _nodeProfleTimer.start(msec);
    _nodeProfileState = State::NODEPROFILE_STARTED;
}

void NodeProfile402::stop()
{
    _nodeProfleTimer.stop();
    _nodeProfileState = State::NODEPROFILE_STOPED;
}

bool NodeProfile402::status() const
{
    return true;
}

void NodeProfile402::readRealTimeObjects() const
{
    _node->readObject(_statusWordObjectId);
    if (_modeCurrent != OperationMode::NoMode)
    {
        _modes[_modeCurrent]->readRealTimeObjects();
    }
}

void NodeProfile402::readAllObjects() const
{
    _node->readObject(_statusWordObjectId);
    _node->readObject(_modesOfOperationDisplayObjectId);
    if (_modeCurrent != OperationMode::NoMode)
    {
        _modes[_modeCurrent]->readAllObjects();
    }
}

quint16 NodeProfile402::profileNumber() const
{
    return static_cast<quint16>(_node->nodeOd()->value(0x1000).toUInt() & 0xFFFF);
}

QString NodeProfile402::profileNumberStr() const
{
    return QString("402");
}

void NodeProfile402::reset()
{
    _stateMachineRequested = State402::STATE_NotReadyToSwitchOn;

    _node->readObject(_modesOfOperationDisplayObjectId);
    _node->readObject(_statusWordObjectId);

    decodeSupportedDriveModes(_node->nodeOd()->value(_supportedDriveModesObjectId).toUInt());

    // TODO : improve set default value or read value on device?
    _modes[IP]->setCwDefaultflag();
    _modes[PP]->setCwDefaultflag();
    _modes[TQ]->setCwDefaultflag();
    _modes[VL]->setCwDefaultflag();
    _modes[CSTCA]->setCwDefaultflag();
    _modes[CP]->setCwDefaultflag();
}

void NodeProfile402::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if (objId == _modesOfOperationObjectId)
    {
        _node->readObject(_modesOfOperationDisplayObjectId);
        _controlWord = (_controlWord & ~CW_OperationModeSpecific);
        if (_modeCurrent != OperationMode::NoMode)
        {
            _controlWord |= _modes[_modeRequested]->getSpecificCwFlag();
        }
        _node->writeObject(_controlWordObjectId, QVariant(_controlWord));
    }

    if (objId == _modesOfOperationDisplayObjectId)
    {
        NodeProfile402::OperationMode mode = static_cast<NodeProfile402::OperationMode>(_node->nodeOd()->value(_modesOfOperationDisplayObjectId).toInt());
        if ((_modeStatus == NodeProfile402::MODE_CHANGING) && (_modeRequested != mode))
        {
            _modeTimer.singleShot(TIMER_READ_MODE_OPERATION_DISPLAY, this, SLOT(readModeOfOperationDisplay()));
            return;
        }
        if (_modeCurrent != mode)
        {
            _modeCurrent = mode;
            // Send new ControlWord with specific flag
            changeStateMachine(_stateMachineCurrent);
            emit modeChanged(_axisId, _modeCurrent);
        }

        _modeRequested = _modeCurrent;
        _modeStatus = NodeProfile402::ModeStatus::MODE_CHANGED;

        return;
    }

    if (objId == _supportedDriveModesObjectId)
    {
        quint32 modes = static_cast<quint32>(_node->nodeOd()->value(_supportedDriveModesObjectId).toUInt());
        decodeSupportedDriveModes(modes);
    }

    if (objId == _controlWordObjectId)
    {
        quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toInt());
        if ((_controlWord & CW_Halt) != (controlWord & CW_Halt))
        {
            emit isHalted(static_cast<bool>(((controlWord & CW_Halt) >> 8)));
        }
        _controlWord = controlWord;
    }

    if (objId == _statusWordObjectId)
    {
        quint16 statusWord = static_cast<quint16>(_node->nodeOd()->value(_statusWordObjectId).toUInt());
        decodeStateMachineStatusWord(statusWord);
        decodeEventStatusWord(statusWord);
    }
}
