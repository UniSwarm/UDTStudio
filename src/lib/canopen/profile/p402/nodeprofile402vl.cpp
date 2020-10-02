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

#include "nodeprofile402vl.h"
#include "node.h"

enum ControlWordVL : quint16
{
    CW_VL_EnableRamp = 0x10,
    CW_VL_UnlockRamp = 0x20,
    CW_VL_ReferenceRamp = 0x40,
    CW_Halt = 0x100
};

NodeProfile402Vl::NodeProfile402Vl(Node *node) : _node(node)
{
    _targetObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6042, 0);
    registerObjId({_targetObjectId});
    _controlWordObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6040, 0);
    registerObjId({_controlWordObjectId});

    setNodeInterrest(_node);
}

void NodeProfile402Vl::enableMode(void)
{
    _cmdControlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
//    _cmdControlWord |= CW_VL_EnableRamp;
    _cmdControlWord |= CW_VL_UnlockRamp;
    _cmdControlWord |= CW_VL_ReferenceRamp;
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

void NodeProfile402Vl::enableRamp(bool ok)
{
    _cmdControlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    if (ok)
    {
        _cmdControlWord |= CW_VL_EnableRamp;
    }
    else
    {
        _cmdControlWord = (_cmdControlWord & ~CW_VL_EnableRamp);
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

void NodeProfile402Vl::unlockRamp(bool ok)
{
    _cmdControlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    if (ok)
    {
        _cmdControlWord |= CW_VL_UnlockRamp;
    }
    else
    {
        _cmdControlWord = (_cmdControlWord & ~CW_VL_UnlockRamp);
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

void NodeProfile402Vl::referenceRamp(bool ok)
{
    _cmdControlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    if (ok)
    {
        _cmdControlWord |= CW_VL_ReferenceRamp;
    }
    else
    {
        _cmdControlWord = (_cmdControlWord & ~CW_VL_ReferenceRamp);
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

bool NodeProfile402Vl::isEnableRamp(void)
{
    return _enableRamp;
}

bool NodeProfile402Vl::isUnlockRamp(void)
{
    return _unlockRamp;
}
bool NodeProfile402Vl::isReferenceRamp(void)
{
    return _referenceRamp;
}

void NodeProfile402Vl::target(qint16 velocity)
{
    _node->writeObject(_targetObjectId, QVariant(velocity));
}

void NodeProfile402Vl::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId == _targetObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else
        {
            emit isAppliedTarget();
        }
    }
    if (objId == _controlWordObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else
        {
            quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
            _enableRamp = false;
            _referenceRamp = false;
            _unlockRamp = false;
            if (controlWord & CW_VL_EnableRamp)
            {
                _enableRamp = true;
                emit enableRamp();
            }
            if (controlWord & CW_VL_ReferenceRamp)
            {
                _referenceRamp = true;
                emit referenceRamp();
            }
            if (controlWord & CW_VL_UnlockRamp)
            {
                _unlockRamp = true;
                emit unlockRamp();
            }
        }
    }
}
