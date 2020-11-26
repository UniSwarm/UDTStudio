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

#include "nodeprofile402.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeobjectid.h"
#include "nodeodsubscriber.h"
#include "nodeprofile402ip.h"
#include "nodeprofile402tq.h"
#include "nodeprofile402vl.h"

#define TIMER_READ_MODE_OPERATION_DISPLAY 100

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

NodeProfile402::NodeProfile402(Node *node, uint8_t axis) : NodeProfile(node)
{
    if (axis > 8)
    {
        return;
    }
    _axis = axis;

    _node = node;

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

    registerObjId({_modesOfOperationObjectId});
    registerObjId({_modesOfOperationDisplayObjectId});
    registerObjId({_supportedDriveModesObjectId});
    registerObjId({_controlWordObjectId});
    registerObjId({_statusWordObjectId});

    _node->readObject(_modesOfOperationDisplayObjectId);
    _node->readObject(_supportedDriveModesObjectId);

    connect(_node, &Node::statusChanged, this, &NodeProfile402::statusNodeChanged);

    _p402Ip = new NodeProfile402Ip(_node, axis, this);
    _p402Tq = new NodeProfile402Tq(_node, axis, this);
    _p402Vl = new NodeProfile402Vl(_node, axis, this);

    _requestedStateMachine = State402::STATE_NotReadyToSwitchOn;
    _stateMachineCurrent = State402::STATE_NotReadyToSwitchOn;
    _currentMode = NoMode;
    _requestedChangeMode= NoMode;
    _state = NONE;

    setNodeInterrest(node);
}

NodeProfile402::Mode NodeProfile402::actualMode()
{
    if ((_currentMode == NoMode) || (_currentMode == PP) || (_currentMode == VL) || (_currentMode == PV)
        || (_currentMode == TQ) || (_currentMode == HM) || (_currentMode == IP) || (_currentMode == CSP)
        || (_currentMode == CSV) || (_currentMode == CST) || (_currentMode == CSTCA))
    {
        return static_cast<Mode>(_currentMode);
    }
    else if (_currentMode < 0)
    {
        return Mode::MS;
    }
    else
    {
        return Mode::Reserved;
    }
}

bool NodeProfile402::setMode(Mode mode)
{
    if (_currentMode == mode)
    {
        return true;
    }

    if ((mode == NoMode) || (mode == PP) || (mode == VL) || (mode == PV) || (mode == TQ) || (mode == HM) || (mode == IP)
        || (mode == CSP) || (mode == CSV) || (mode == CST) || (mode == CSTCA))
    {
        _node->writeObject(_modesOfOperationObjectId, QVariant(mode));
        _state = MODE_CHANGE;
        _requestedChangeMode = mode;
        return true;
    }
    else
    {
        return false;
    }
}

QString NodeProfile402::modeStr(NodeProfile402::Mode mode)
{
    switch (mode)
    {
        case Mode::NoMode:
            return tr("No mode");
        case Mode::PP:
            return tr("Profile position");
        case Mode::VL:
            return tr("Velocity (VL)");
        case Mode::PV:
            return tr("Profile velocity");
        case Mode::TQ:
            return tr("Torque profile(TQ)");
        case Mode::HM:
            return tr("Homing");
        case Mode::IP:
            return tr("Interpolated position (IP)");
        case Mode::CSP:
            return tr("Cyclic sync position");
        case Mode::CSV:
            return tr("Cyclic sync velocity");
        case Mode::CST:
            return tr("Cyclic sync torque");
        case Mode::CSTCA:
            return tr("Cyclic sync torque mode with commutation angle");
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

bool NodeProfile402::isModeSupported(Mode mode)
{
    for (const Mode &modesupported : _supportedModes)
    {
        if (modesupported == mode)
        {
            return true;
        }
    }
    return false;
}

QList<NodeProfile402::Mode> NodeProfile402::modesSupported()
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
    _state = STATE_CHANGE;
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
            break;
        case NodeProfile402::VL:
            _p402Vl->setTarget(static_cast<qint16>(target));
            break;
        case NodeProfile402::PV:
            break;
        case NodeProfile402::TQ:
            _p402Tq->setTarget(static_cast<qint16>(target));
            break;
        case NodeProfile402::HM:
            break;
        case NodeProfile402::IP:
            _p402Ip->setTarget(target);
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
    if (_currentMode == Mode::VL)
    {
        _cmdControlWord = _p402Vl->setEnableRamp(_cmdControlWord, ok);
    }
    if (_currentMode == Mode::IP)
    {
        _cmdControlWord = _p402Ip->setEnableRamp(_cmdControlWord, ok);
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

bool NodeProfile402::isEnableRamp()
{
    if (_currentMode == Mode::VL)
    {
        return _p402Vl->isEnableRamp();
    }
    if (_currentMode == Mode::IP)
    {
        return _p402Ip->isEnableRamp();
    }
    return false;
}

void NodeProfile402::setUnlockRamp(bool ok)
{
    if (_currentMode == Mode::VL)
    {
        _cmdControlWord = _p402Vl->setUnlockRamp(_cmdControlWord, ok);
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

bool NodeProfile402::isUnlockRamp()
{
    if (_currentMode == Mode::VL)
    {
        return _p402Vl->isUnlockRamp();
    }
    return false;
}

void NodeProfile402::setReferenceRamp(bool ok)
{
    if (_currentMode == Mode::VL)
    {
        _cmdControlWord = _p402Vl->setReferenceRamp(_cmdControlWord, ok);
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

bool NodeProfile402::isReferenceRamp()
{
    if (_currentMode == Mode::VL)
    {
        return _p402Vl->isReferenceRamp();
    }
    return false;
}

void NodeProfile402::statusNodeChanged(Node::Status status)
{
    if (status == Node::STARTED)
    {
        _requestedStateMachine = State402::STATE_ReadyToSwitchOn;
        _stateMachineCurrent = State402::STATE_ReadyToSwitchOn;
        _node->readObject(_modesOfOperationDisplayObjectId);
    }
    else
    {
        _requestedStateMachine = State402::STATE_ReadyToSwitchOn;
        _stateMachineCurrent = State402::STATE_ReadyToSwitchOn;
    }
}

void NodeProfile402::enableRamp(quint16 cmdControlWord)
{
    switch (_currentMode)
    {
        case NodeProfile402::MS:
            break;
        case NodeProfile402::NoMode:
            break;
        case NodeProfile402::PP:
            break;
        case NodeProfile402::VL:
            _p402Vl->enableMode(cmdControlWord);
            break;
        case NodeProfile402::PV:
            break;
        case NodeProfile402::TQ:
            break;
        case NodeProfile402::HM:
            break;
        case NodeProfile402::IP:
            _p402Ip->enableMode(cmdControlWord);
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
                enableRamp(_cmdControlWord);
            }
            break;
        case STATE_SwitchedOn:
            if (state == STATE_OperationEnabled)
            {
                _cmdControlWord = (_cmdControlWord & ~CW_Mask);
                _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn | CW_EnableOperation);
                enableRamp(_cmdControlWord);
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
    _cmdControlWord = (_cmdControlWord & ~CW_Mask);

    if ((statusWord & SW_StateMask1) == SW_StateNotReadyToSwitchOn)
    {
        _stateMachineCurrent = STATE_NotReadyToSwitchOn;
    }
    if ((statusWord & SW_StateMask1) == SW_StateSwitchOnDisabled)
    {
        _stateMachineCurrent = STATE_SwitchOnDisabled;
        _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
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

    if ((_state == STATE_CHANGE) && (_requestedStateMachine != _stateMachineCurrent))
    {
        manageState(_requestedStateMachine);
        return;
    }

    _state = NONE;
    emit stateChanged();
}

void NodeProfile402::manageSupportedDriveModes(quint32 supportedDriveModes)
{
    _supportedModes.clear();
    if ((supportedDriveModes & 0x7EF) == 0)
    {
        _supportedModes.append(Mode::NoMode);
        return;
    }
    for (quint32 i = 0; i <= 10; i++)
    {
        switch ((supportedDriveModes & (1 << i)))
        {
            case 0x1:
                _supportedModes.append(Mode::PP);
                break;
            case 0x2:
                _supportedModes.append(Mode::VL);
                break;
            case 0x4:
                _supportedModes.append(Mode::PV);
                break;
            case 0x8:
                _supportedModes.append(Mode::TQ);
                break;
            case 0x20:
                _supportedModes.append(Mode::HM);
                break;
            case 0x40:
                _supportedModes.append(Mode::IP);
                break;
            case 0x80:
                _supportedModes.append(Mode::CSP);
                break;
            case 0x100:
                _supportedModes.append(Mode::CSV);
                break;
            case 0x200:
                _supportedModes.append(Mode::CST);
                break;
            case 0x400:
                _supportedModes.append(Mode::CSTCA);
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

    _node->readObject(_modesOfOperationDisplayObjectId);
    _node->readObject(_supportedDriveModesObjectId);
    _node->readObject(_controlWordObjectId);
}

void NodeProfile402::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId == _modesOfOperationObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else
        {
            _node->readObject(_modesOfOperationDisplayObjectId);
        }
    }

    if (objId == _modesOfOperationDisplayObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            _currentMode = NoMode;
        }
        else
        {
            NodeProfile402::Mode mode = static_cast<NodeProfile402::Mode>(_node->nodeOd()->value(_modesOfOperationDisplayObjectId).toInt());
            if ((_state == MODE_CHANGE) && (_requestedChangeMode != mode))
            {
                _modeTimer.singleShot(TIMER_READ_MODE_OPERATION_DISPLAY, this, SLOT(readModeOfOperationDisplay()));
                return;
            }

            _currentMode = mode;
            emit modeChanged(_currentMode);
            _state = NONE;
        }
    }

    if (objId == _supportedDriveModesObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else
        {
            quint32 modes = static_cast<quint32>(_node->nodeOd()->value(_supportedDriveModesObjectId).toUInt());
            manageSupportedDriveModes(modes);
        }
    }

    if (objId == _controlWordObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else
        {
            quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toInt());
            if ((_cmdControlWord & CW_Halt) != (controlWord & CW_Halt))
            {
                emit isHalted(static_cast<bool>(((controlWord & CW_Halt) >> 8)));
            }

            _node->readObject(_statusWordObjectId);
        }
    }

    if (objId == _statusWordObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else
        {
            quint16 statusWord = static_cast<quint16>(_node->nodeOd()->value(_statusWordObjectId).toUInt());
            manageStateStatusWord(statusWord);
            manageEventStatusWord(statusWord);
        }
    }
}
