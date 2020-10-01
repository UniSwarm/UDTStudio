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
#include "node.h"
#include "nodeobjectid.h"
#include "nodeodsubscriber.h"

NodeProfile402::NodeProfile402(Node *node)
    : NodeProfile(node)
{
    //Mode
    _modesOfOperationObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6060, 0);
    _modesOfOperationDisplayObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6061, 0);
    _supportedDriveModesObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6502, 0);
    registerObjId({_modesOfOperationObjectId});
    registerObjId({_modesOfOperationDisplayObjectId});
    registerObjId({_supportedDriveModesObjectId});

    //ControlWord/StatusWord
    _controlWordObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6040, 0);
    _statusWordObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6041, 0);
    registerObjId({_controlWordObjectId});
    registerObjId({_statusWordObjectId});

    setNodeInterrest(node);

    //Mode
    _node->readObject(_modesOfOperationDisplayObjectId);
    _node->readObject(_supportedDriveModesObjectId);
    //ControlWord/StatusWord
    _node->readObject(_controlWordObjectId);
}

NodeProfile402::Mode NodeProfile402::actualMode()
{
    if ((_currentMode == NoMode) || (_currentMode == PP) || (_currentMode == VL) || (_currentMode == PV) || (_currentMode == TQ) || (_currentMode == HM) || (_currentMode == IP) ||
        (_currentMode == CSP) || (_currentMode == CSV) || (_currentMode == CST) || (_currentMode == CSTCA))
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

QString NodeProfile402::actualModeStr()
{
    return modeStr(_currentMode);
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

bool NodeProfile402::setMode(Mode mode)
{
    if (_currentMode == mode)
    {
        return true;
    }

    if ((mode == NoMode) || (mode == PP) || (mode == VL) || (mode == PV) || (mode == TQ) || (mode == HM) || (mode == IP) || (mode == CSP) || (mode == CSV) || (mode == CST) || (mode == CSTCA))
    {
        _node->writeObject(_modesOfOperationObjectId, QVariant(mode));
        _state = STATE_CHANGE_MODE;
        _currentError = NO_ERROR;
        return true;
    }
    else
    {
        _currentError = WRONG_MODE;
        return false;
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

NodeProfile402::StateOf402 NodeProfile402::currentStateOf402() const
{
    return _stateMachineCurrent;
}

void NodeProfile402::changeStateOf402(const StateOf402 stateOf402)
{
    if (!_node)
    {
        return;
    }

    switch (_stateMachineCurrent)
    {
    case STATE_NotReadyToSwitchOn:
        break;
    case STATE_SwitchOnDisabled:
        if (stateOf402 == STATE_ReadyToSwitchOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop);
        }
        break;
    case STATE_ReadyToSwitchOn:
        if (stateOf402 == STATE_SwitchedOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn);
        }
        if (stateOf402 == STATE_OperationEnabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn | CW_EnableOperation);
        }
        if (stateOf402 == STATE_SwitchOnDisabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
        }
        break;
    case STATE_SwitchedOn:
        if (stateOf402 == STATE_OperationEnabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn | CW_EnableOperation);
        }
        if (stateOf402 == STATE_ReadyToSwitchOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop);
        }
        if (stateOf402 == STATE_SwitchOnDisabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
        }
        break;
    case STATE_OperationEnabled:
        if (stateOf402 == STATE_SwitchOnDisabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
        }
        if (stateOf402 == STATE_ReadyToSwitchOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop);
        }
        if (stateOf402 == STATE_SwitchedOn)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage | CW_QuickStop | CW_SwitchOn);
        }
        if (stateOf402 == STATE_QuickStopActive)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
            _cmdControlWord |= (CW_EnableVoltage);
        }
        break;
    case STATE_QuickStopActive:
        if (stateOf402 == STATE_SwitchOnDisabled)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
        }
        break;
    case STATE_FaultReactionActive:
        break;
    case STATE_Fault:
        if (stateOf402 == STATE_Fault)
        {
            _cmdControlWord = (_cmdControlWord & ~CW_Mask);
        }
        break;
    }
    _cmdControlWord = (_cmdControlWord & ~CW_Halt);
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

QString NodeProfile402::currentStateOf402Str() const
{
   return stateOf402Str(_stateMachineCurrent);
}

QString NodeProfile402::stateOf402Str(StateOf402 stateOf402) const
{
    switch (stateOf402)
    {
    case StateOf402::STATE_NotReadyToSwitchOn:
        return tr("1_Not ready to switch on");
    case StateOf402::STATE_SwitchOnDisabled:
        return tr("2_Switch on disabled");
    case StateOf402::STATE_ReadyToSwitchOn:
        return tr("3_Ready to switch on");
    case StateOf402::STATE_SwitchedOn:
        return tr("4_Switched on");
    case StateOf402::STATE_OperationEnabled:
        return tr("5_Operation enabled");
    case StateOf402::STATE_QuickStopActive:
        return tr("6_Quick stop active");
    case StateOf402::STATE_FaultReactionActive:
        return tr("7_Fault reaction active");
    case StateOf402::STATE_Fault:
        return tr("8_Fault");
    }
    return QString();
}

quint16 NodeProfile402::rawStatusWord()
{
    if (!_node)
    {
        return 0;
    }
    return static_cast<quint16>(_node->nodeOd()->value(_statusWordObjectId).toUInt());
}

quint16 NodeProfile402::rawControlWord()
{
    if (!_node)
    {
        return 0;
    }
    return static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
}

 bool NodeProfile402::haltToggle()
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

uint8_t NodeProfile402::msFieldStatusWord()
{
 return _msFieldStatusWord;
}

uint8_t NodeProfile402::omsFieldStatusWord()
{
 return _informationFieldStatusWord;
}

uint8_t NodeProfile402::warningStatusWord()
{
    return _warningFieldStatusWord;
}

uint8_t NodeProfile402::informationStatusWord()
{
    return _informationFieldStatusWord;
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

void NodeProfile402::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId == _modesOfOperationObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            _currentError = MODE_NOT_APPLY;
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
            _currentError = MODE_NOT_APPLY;
            _currentMode = NoMode;
        }
        else
        {
            NodeProfile402::Mode mode = static_cast<NodeProfile402::Mode>(_node->nodeOd()->value(_modesOfOperationDisplayObjectId).toInt());
            if (_currentMode != mode)
            {
                _currentMode = mode;
                emit modeChanged();
            }
            _state = STATE_NONE;
        }
    }

    if (objId == _supportedDriveModesObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            //_currentError = MODE_NOT_APPLY;
        }
        else
        {
            quint32 modes = static_cast<quint32>(_node->nodeOd()->value(_supportedDriveModesObjectId).toUInt());
            _supportedModes.clear();
            if ((modes & 0x7EF) == 0)
            {
                _supportedModes.append(Mode::NoMode);
                return;
            }
            for (quint32 i = 0; i <= 10; i++)
            {
                switch ((modes & (1 << i)))
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
        }
    }

    if (objId == _controlWordObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            //_currentError = MODE_NOT_APPLY;
        }
        else
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
        if (flags == SDO::FlagsRequest::Error)
        {
            //_currentError = MODE_NOT_APPLY;
        }
        else
        {
            quint16 state = static_cast<quint16>(_node->nodeOd()->value(_statusWordObjectId).toUInt());
            if ((state & Mask1) == SW_StateNotReadyToSwitchOn)
            {
                _stateMachineCurrent = STATE_NotReadyToSwitchOn;
            }
            if ((state & Mask1) == SW_StateSwitchOnDisabled)
            {
                _stateMachineCurrent = STATE_SwitchOnDisabled;
            }
            if ((state & Mask2) == SW_StateReadyToSwitchOn)
            {
                _stateMachineCurrent = STATE_ReadyToSwitchOn;
            }
            if ((state & Mask2) == SW_StateSwitchedOn)
            {
                _stateMachineCurrent = STATE_SwitchedOn;
            }
            if ((state & Mask2) == SW_StateOperationEnabled)
            {
                _stateMachineCurrent = STATE_OperationEnabled;
            }
            if ((state & Mask2) == SW_StateQuickStopActive)
            {
                _stateMachineCurrent = STATE_QuickStopActive;
            }
            if ((state & Mask1) == SW_StateFaultReactionActive)
            {
                _stateMachineCurrent = STATE_FaultReactionActive;
            }
            if ((state & Mask1) == SW_StateFault)
            {
                _stateMachineCurrent = STATE_Fault;
            }

            _omsFieldStatusWord = (state & SW_OperationModeSpecific) >> 0xC;
            _msFieldStatusWord = (state & SW_ManufacturerSpecific) >> 0xE;

            _informationFieldStatusWord = InformationNone;
            if (state & SW_VoltageEnabled)
            {
                _informationFieldStatusWord += VoltageEnabled;
            }
            if (state & SW_Remote)
            {
                _informationFieldStatusWord += Remote;
            }
            if (state & SW_TargetReached)
            {
                _informationFieldStatusWord += TargetReached;
            }

            _warningFieldStatusWord = WarningNone;
            if (state & SW_InternalLimitActive)
            {
                _informationFieldStatusWord += InternalLimitActive;
            }
            if (state & SW_Warning)
            {
                _informationFieldStatusWord += Warning;
            }
            if (state & SW_FollowingError)
            {
                _informationFieldStatusWord += FollowingError;
            }

            emit stateChanged();
        }
    }
}
