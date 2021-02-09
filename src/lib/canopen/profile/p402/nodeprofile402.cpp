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
#include "node.h"
#include "nodeobjectid.h"
#include "nodeodsubscriber.h"
#include "modeip.h"
#include "modepp.h"
#include "modetq.h"
#include "modevl.h"

#define TIMER_READ_MODE_OPERATION_DISPLAY 100u

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
    SW_StateMask1 = 0x4F,
    SW_StateMask2 = 0x6F,

    SW_EventVoltageEnabled = 0x10,
    SW_EventWarning = 0x80,
    SW_EventRemote = 0x200,
    SW_EventTargetReached = 0x400,
    SW_EventFollowingError = 0x2000,
    SW_EventInternalLimitActive = 0x800,
    SW_EventMask = 0xE90,

    SW_OperationModeSpecific = 0x3000,
    SW_ManufacturerSpecific = 0xC000
};

NodeProfile402::NodeProfile402(Node *node, uint8_t axis)
    : NodeProfile(node)
{
    if (axis > 8)
    {
        return;
    }

    _axisId = axis;

    _modesOfOperationObjectId = IndexDb402::getObjectId(IndexDb402::OD_MODES_OF_OPERATION, axis);
    _modesOfOperationDisplayObjectId = IndexDb402::getObjectId(IndexDb402::OD_MODES_OF_OPERATION_DISPLAY, axis);
    _supportedDriveModesObjectId = IndexDb402::getObjectId(IndexDb402::OD_SUPPORTED_DRIVE_MODES, axis);
    _controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD, axis);
    _statusWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_STATUSWORD, axis);

    _modesOfOperationObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _modesOfOperationDisplayObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _supportedDriveModesObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _statusWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    setNodeInterrest(node);
    registerObjId(_modesOfOperationObjectId);
    registerObjId(_modesOfOperationDisplayObjectId);
    registerObjId(_supportedDriveModesObjectId);
    registerObjId(_controlWordObjectId);
    registerObjId(_statusWordObjectId);

    manageSupportedDriveModes(_node->nodeOd()->value(_supportedDriveModesObjectId).toUInt());

    _node->readObject(_modesOfOperationDisplayObjectId);
    _node->readObject(_controlWordObjectId);

    connect(_node, &Node::statusChanged, this, &NodeProfile402::statusNodeChanged);

    _modeIp = new ModeIp(this);
    _modeTq = new ModeTq(this);
    _modeVl = new ModeVl(this);
    _modePp = new ModePp(this);
    _modes.insert(IP, _modeIp);
    _modes.insert(TQ, _modeTq);
    _modes.insert(VL, _modeVl);
    _modes.insert(PP, _modePp);

    _requestedStateMachine = State402::STATE_NotReadyToSwitchOn;
    _stateMachineCurrent = State402::STATE_NotReadyToSwitchOn;
    _currentMode = NoMode;
    _requestedChangeMode = NoMode;
    _stateMode = NONE_MODE;
    _stateState = NONE_STATE;

    _cmdControlWord = 0;
}

NodeProfile402::OperationMode NodeProfile402::actualMode()
{
    if ((_currentMode == NoMode) || (_currentMode == PP) || (_currentMode == VL) || (_currentMode == PV) || (_currentMode == TQ) || (_currentMode == HM) || (_currentMode == IP) ||
        (_currentMode == CSP) || (_currentMode == CSV) || (_currentMode == CST) || (_currentMode == CSTCA))
    {
        return static_cast<OperationMode>(_currentMode);
    }
    else if (_currentMode < 0)
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
    if (_currentMode == mode)
    {
        return true;
    }

    if ((mode == NoMode) || (mode == PP) || (mode == VL) || (mode == PV) || (mode == TQ) || (mode == HM) || (mode == IP) || (mode == CSP) || (mode == CSV) || (mode == CST) || (mode == CSTCA))
    {
        _node->writeObject(_modesOfOperationObjectId, QVariant(mode));
        _stateMode = MODE_CHANGE;
        _requestedChangeMode = mode;

        if (_requestedChangeMode == OperationMode::VL)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeVl->getSpecificControlWord();
            emit enableRampEvent(_modeVl->isEnableRamp());
            emit referenceRampEvent(_modeVl->isReferenceRamp());
            emit unlockRampEvent(_modeVl->isUnlockRamp());
        }
        else if (_requestedChangeMode == OperationMode::IP)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeIp->getSpecificControlWord();
            emit enableRampEvent(_modeIp->isEnableRamp());
        }
        else if (_requestedChangeMode == OperationMode::TQ)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeTq->getSpecificControlWord();
        }

        else if (_requestedChangeMode == OperationMode::PP)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modePp->getSpecificControlWord();
        }

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
        return tr("Cyclic sync torque mode with commutation angle (CTCA)");
    default:
        if (_currentMode < 0)
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
    for (const OperationMode &modesupported : _supportedModes)
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
    return _supportedModes;
}

NodeProfile402::State402 NodeProfile402::currentState() const
{
    return _stateMachineCurrent;
}

void NodeProfile402::goToState(const State402 state)
{
    if (_node->status() != Node::STARTED)
    {
        return;
    }
    _stateState = STATE_CHANGE;
    _requestedStateMachine = state;
    manageState(_requestedStateMachine);
}

QString NodeProfile402::stateStr(State402 state) const
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

bool NodeProfile402::toggleHalt()
{
    quint16 cmdControlWord;
    if (_stateMachineCurrent == STATE_OperationEnabled)
    {
        if ((_cmdControlWord & CW_Halt) != 0)
        {
            cmdControlWord = (_cmdControlWord & ~CW_Halt);
        }
        else
        {
            cmdControlWord = _cmdControlWord | CW_Halt;
        }
        _node->writeObject(_controlWordObjectId, QVariant(cmdControlWord));
        return 0;
    }
    return 0;
}

void NodeProfile402::setTarget(qint32 target)
{
    switch (_currentMode)
    {
    case NodeProfile402::MS:
        break;

    case NodeProfile402::NoMode:
        break;

    case NodeProfile402::PP:
        _modePp->setTarget(target);
        break;

    case NodeProfile402::VL:
        _modeVl->setTarget(static_cast<qint16>(target));
        break;

    case NodeProfile402::PV:
        break;

    case NodeProfile402::TQ:
        _modeTq->setTarget(static_cast<qint16>(target));
        break;

    case NodeProfile402::HM:
        break;

    case NodeProfile402::IP:
        _modeIp->setTarget(target);
        break;

    case NodeProfile402::CSP:
        break;

    case NodeProfile402::CSV:
        break;

    case NodeProfile402::CST:
        break;

    case NodeProfile402::CSTCA:
        break;

    case NodeProfile402::Reserved:
        break;
    }
}

QString NodeProfile402::event402Str(quint8 event402)
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
    default:
        return QString();
    }
}

void NodeProfile402::setEnableRamp(bool ok)
{
    if (_currentMode == OperationMode::VL)
    {
        _modeVl->setEnableRamp(ok);
        _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeVl->getSpecificControlWord();
    }
    if (_currentMode == OperationMode::IP)
    {
        _modeIp->setEnableRamp(ok);
        _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeIp->getSpecificControlWord();
    }
    if (_currentMode == OperationMode::PP)
    {
        _modePp->setEnableRamp(ok);
        _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeIp->getSpecificControlWord();
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

bool NodeProfile402::isEnableRamp()
{
    if (_currentMode == OperationMode::VL)
    {
        return _modeVl->isEnableRamp();
    }
    if (_currentMode == OperationMode::IP)
    {
        return _modeIp->isEnableRamp();
    }
    return false;
}

void NodeProfile402::setUnlockRamp(bool ok)
{
    if (_currentMode == OperationMode::VL)
    {
        _modeVl->setUnlockRamp(ok);
        _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeVl->getSpecificControlWord();
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

bool NodeProfile402::isUnlockRamp()
{
    if (_currentMode == OperationMode::VL)
    {
        return _modeVl->isUnlockRamp();
    }
    return false;
}

void NodeProfile402::setReferenceRamp(bool ok)
{
    if (_currentMode == OperationMode::VL)
    {
        _modeVl->setReferenceRamp(ok);
        _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeVl->getSpecificControlWord();
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

bool NodeProfile402::isReferenceRamp()
{
    if (_currentMode == OperationMode::VL)
    {
        return _modeVl->isReferenceRamp();
    }
    return false;
}

const Mode *NodeProfile402::mode(NodeProfile402::OperationMode mode) const
{
    return _modes[mode];
}

void NodeProfile402::statusNodeChanged(Node::Status status)
{
    if (status == Node::STARTED)
    {
        _node->readObject(_modesOfOperationDisplayObjectId);
    }

    _node->readObject(_statusWordObjectId);
}

void NodeProfile402::activeSpecificBitControlWord(quint16 &cmdControlWord)
{
    switch (_currentMode)
    {
    case NodeProfile402::MS:
        break;

    case NodeProfile402::NoMode:
        break;

    case NodeProfile402::PP:
        cmdControlWord |= _modePp->getSpecificControlWord();
        break;

    case NodeProfile402::VL:
        cmdControlWord |= _modeVl->getSpecificControlWord();
        break;

    case NodeProfile402::PV:
        break;

    case NodeProfile402::TQ:
        cmdControlWord |= _modeTq->getSpecificControlWord();
        break;

    case NodeProfile402::HM:
        break;

    case NodeProfile402::IP:
        cmdControlWord |= _modeIp->getSpecificControlWord();
        break;

    case NodeProfile402::CSP:
        break;

    case NodeProfile402::CSV:
        break;

    case NodeProfile402::CST:
        break;

    case NodeProfile402::CSTCA:
        break;

    case NodeProfile402::Reserved:
        break;
    }
}

void NodeProfile402::manageState(const State402 state)
{
    if (!_node)
    {
        return;
    }
    if (_node->status() != Node::STARTED)
    {
        return;
    }

    if (_currentMode == OperationMode::VL)
    {
        _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeVl->getSpecificControlWord();
    }
    else if (_currentMode == OperationMode::IP)
    {
        _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeIp->getSpecificControlWord();
    }
    else if (_currentMode == OperationMode::TQ)
    {
        _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeTq->getSpecificControlWord();
    }

    else if (_currentMode == OperationMode::PP)
    {
        _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modePp->getSpecificControlWord();
    }

    switch (_stateMachineCurrent)
    {
    case STATE_NotReadyToSwitchOn:
    case STATE_SwitchOnDisabled:
        if (state >= STATE_ReadyToSwitchOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop);
        }
        break;

    case STATE_ReadyToSwitchOn:
        if (state == STATE_SwitchOnDisabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
        }
        else if (state >= STATE_SwitchedOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn);
        }
        else if (state >= STATE_OperationEnabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn | CW_EnableOperation);
            activeSpecificBitControlWord(_cmdControlWord);
        }
        break;

    case STATE_SwitchedOn:
        if (state == STATE_OperationEnabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn | CW_EnableOperation);
            activeSpecificBitControlWord(_cmdControlWord);
        }
        if (state == STATE_ReadyToSwitchOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop);
        }
        if (state == STATE_SwitchOnDisabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
        }
        break;

    case STATE_OperationEnabled:
        if (state == STATE_SwitchOnDisabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
        }
        if (state == STATE_ReadyToSwitchOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop);
        }
        if (state == STATE_SwitchedOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn);
        }
        if (state == STATE_QuickStopActive)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage);
        }
        break;

    case STATE_QuickStopActive:
        if (state == STATE_SwitchOnDisabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
        }
        break;

    case STATE_FaultReactionActive:
        break;

    case STATE_Fault:
        if (state >= STATE_SwitchOnDisabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_FaultReset);
        }
        break;
    }

    _cmdControlWord = (_cmdControlWord & ~CW_Halt);
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

void NodeProfile402::manageEventStatusWord(quint16 statusWord)
{
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
    if (eventStatusWord != _eventStatusWord)
    {
        _eventStatusWord = eventStatusWord;
        emit eventHappened(_eventStatusWord);
    }
}

void NodeProfile402::manageStateStatusWord(quint16 statusWord)
{
    if ((statusWord & SW_StateMask1) == SW_StateNotReadyToSwitchOn)
    {
        _stateMachineCurrent = STATE_NotReadyToSwitchOn;
    }
    if ((statusWord & SW_StateMask1) == SW_StateSwitchOnDisabled)
    {
        if (_currentMode == OperationMode::VL)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeVl->getSpecificControlWord();
            emit enableRampEvent(_modeVl->isEnableRamp());
            emit referenceRampEvent(_modeVl->isReferenceRamp());
            emit unlockRampEvent(_modeVl->isUnlockRamp());
        }

        if (_currentMode == OperationMode::IP)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_OperationModeSpecific) | _modeIp->getSpecificControlWord();
            emit enableRampEvent(_modeIp->isEnableRamp());
        }

        _stateMachineCurrent = STATE_SwitchOnDisabled;
        if ((_cmdControlWord & CW_Mask) == CW_FaultReset)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_FaultReset);
            _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
        }
    }
    if ((statusWord & SW_StateMask2) == SW_StateReadyToSwitchOn)
    {
        _stateMachineCurrent = STATE_ReadyToSwitchOn;
    }
    if ((statusWord & SW_StateMask2) == SW_StateSwitchedOn)
    {
        _stateMachineCurrent = STATE_SwitchedOn;
    }
    if ((statusWord & SW_StateMask2) == SW_StateOperationEnabled)
    {
        _stateMachineCurrent = STATE_OperationEnabled;
    }
    if ((statusWord & SW_StateMask2) == SW_StateQuickStopActive)
    {
        _stateMachineCurrent = STATE_QuickStopActive;
    }
    if ((statusWord & SW_StateMask1) == SW_StateFaultReactionActive)
    {
        _stateMachineCurrent = STATE_FaultReactionActive;
    }
    if ((statusWord & SW_StateMask1) == SW_StateFault)
    {
        _stateMachineCurrent = STATE_Fault;
    }

    if ((_stateState == STATE_CHANGE) && (_requestedStateMachine != _stateMachineCurrent))
    {
        manageState(_requestedStateMachine);
        return;
    }
    _stateState = NONE_STATE;
    emit stateChanged();
}

void NodeProfile402::manageSupportedDriveModes(quint32 supportedDriveModes)
{
    _supportedModes.clear();
    if ((supportedDriveModes & 0x7EF) == 0)
    {
        _supportedModes.append(OperationMode::NoMode);
        return;
    }
    for (quint32 i = 0; i <= 10; i++)
    {
        switch ((supportedDriveModes & (1 << i)))
        {
        case 0x1:
            _supportedModes.append(OperationMode::PP);
            break;

        case 0x2:
            _supportedModes.append(OperationMode::VL);
            break;

        case 0x4:
            _supportedModes.append(OperationMode::PV);
            break;

        case 0x8:
            _supportedModes.append(OperationMode::TQ);
            break;

        case 0x20:
            _supportedModes.append(OperationMode::HM);
            break;

        case 0x40:
            _supportedModes.append(OperationMode::IP);
            break;

        case 0x80:
            _supportedModes.append(OperationMode::CSP);
            break;

        case 0x100:
            _supportedModes.append(OperationMode::CSV);
            break;

        case 0x200:
            _supportedModes.append(OperationMode::CST);
            break;

        case 0x400:
            _supportedModes.append(OperationMode::CSTCA);
            break;
        }
    }
    emit supportedDriveModesUdpdated();
}

void NodeProfile402::readModeOfOperationDisplay()
{
    _node->readObject(_modesOfOperationDisplayObjectId);
}

bool NodeProfile402::status() const
{
    return true;
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
    _requestedStateMachine = State402::STATE_NotReadyToSwitchOn;
    _stateMachineCurrent = State402::STATE_NotReadyToSwitchOn;
    _currentMode = NoMode;

    manageSupportedDriveModes(_node->nodeOd()->value(_supportedDriveModesObjectId).toUInt());

    _modeIp->setEnableRamp(true);
    _modeVl->setEnableRamp(true);
    _modeVl->setUnlockRamp(true);
    _modeVl->setReferenceRamp(true);
}

void NodeProfile402::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId == _modesOfOperationObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            _node->readObject(_modesOfOperationDisplayObjectId);
        }
    }

    if (objId == _modesOfOperationDisplayObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            NodeProfile402::OperationMode mode = static_cast<NodeProfile402::OperationMode>(_node->nodeOd()->value(_modesOfOperationDisplayObjectId).toInt());
            if ((_stateMode == NodeProfile402::MODE_CHANGE) && (_requestedChangeMode != mode))
            {
                _modeTimer.singleShot(TIMER_READ_MODE_OPERATION_DISPLAY, this, SLOT(readModeOfOperationDisplay()));
                return;
            }

            _currentMode = mode;
            _requestedChangeMode = _currentMode;
            _stateMode = NodeProfile402::StateMode::NONE_MODE;
            emit modeChanged(_axisId, _currentMode);
            return;
        }
    }

    if (objId == _supportedDriveModesObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            quint32 modes = static_cast<quint32>(_node->nodeOd()->value(_supportedDriveModesObjectId).toUInt());
            manageSupportedDriveModes(modes);
        }
    }

    if (objId == _controlWordObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toInt());
            if ((_cmdControlWord & CW_Halt) != (controlWord & CW_Halt))
            {
                emit isHalted(static_cast<bool>(((controlWord & CW_Halt) >> 8)));
            }
            _cmdControlWord = controlWord;
            _node->readObject(_statusWordObjectId);
        }
    }

    if (objId == _statusWordObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            quint16 statusWord = static_cast<quint16>(_node->nodeOd()->value(_statusWordObjectId).toUInt());
            manageStateStatusWord(statusWord);
            manageEventStatusWord(statusWord);
        }
    }
}
