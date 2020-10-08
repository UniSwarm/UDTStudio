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
#include "indexdb402.h"

enum ControlWordVL : quint16
{
    CW_VL_EnableRamp = 0x10,
    CW_VL_UnlockRamp = 0x20,
    CW_VL_ReferenceRamp = 0x40,
    CW_Halt = 0x100
};

NodeProfile402Vl::NodeProfile402Vl(Node *node) : _node(node)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_TARGET);
    _controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD);
    _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId({_targetObjectId});
    registerObjId({_controlWordObjectId});

    _enableRamp = false;
    _referenceRamp = false;
    _unlockRamp = false;

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

void NodeProfile402Vl::setEnableRamp(bool ok)
{
    quint16 cmdControlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    if (ok)
    {
        cmdControlWord |= CW_VL_EnableRamp;
    }
    else
    {
        cmdControlWord = (cmdControlWord & ~CW_VL_EnableRamp);
    }
    _node->writeObject(_controlWordObjectId, QVariant(cmdControlWord));
}

void NodeProfile402Vl::setUnlockRamp(bool ok)
{
    quint16 cmdControlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    if (ok)
    {
        cmdControlWord |= CW_VL_UnlockRamp;
    }
    else
    {
        cmdControlWord = (cmdControlWord & ~CW_VL_UnlockRamp);
    }
    _node->writeObject(_controlWordObjectId, QVariant(cmdControlWord));
}

void NodeProfile402Vl::setReferenceRamp(bool ok)
{
    quint16 cmdControlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    if (ok)
    {
        cmdControlWord |= CW_VL_ReferenceRamp;
    }
    else
    {
        cmdControlWord = (cmdControlWord & ~CW_VL_ReferenceRamp);
    }
    _node->writeObject(_controlWordObjectId, QVariant(cmdControlWord));
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

void NodeProfile402Vl::setTarget(qint16 velocity)
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
            if ((controlWord & CW_VL_EnableRamp) != (_cmdControlWord & CW_VL_EnableRamp))
            {
                _enableRamp = (controlWord & CW_VL_EnableRamp);
                emit enableRampEvent((controlWord & CW_VL_EnableRamp));
            }
            if ((controlWord & CW_VL_ReferenceRamp) != (_cmdControlWord & CW_VL_ReferenceRamp))
            {
                _referenceRamp = (controlWord & CW_VL_ReferenceRamp);
                emit referenceRampEvent((controlWord & CW_VL_ReferenceRamp));
            }
            if ((controlWord & CW_VL_UnlockRamp) != (_cmdControlWord & CW_VL_UnlockRamp))
            {
                _unlockRamp = (controlWord & CW_VL_UnlockRamp);
                emit unlockRampEvent(controlWord & CW_VL_UnlockRamp);
            }
            _cmdControlWord = controlWord;
        }
    }
}
