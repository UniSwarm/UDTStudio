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

#include "nodeprofile402vl.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

enum ControlWordVL : quint16
{
    CW_VL_EnableRamp = 0x10,
    CW_VL_UnlockRamp = 0x20,
    CW_VL_ReferenceRamp = 0x40,
    CW_Halt = 0x100
};

NodeProfile402Vl::NodeProfile402Vl(Node *node, uint8_t axis, NodeProfile402 *nodeProfile402)
    : _node(node)
    , _axis(axis)
    , _nodeProfile402(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_TARGET, axis);
    _controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD, axis);
    _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    setNodeInterrest(_node);
    registerObjId(_targetObjectId);
    registerObjId(_controlWordObjectId);

    _mode = 2;
    _cmdControlWordSpecific = CW_VL_EnableRamp | CW_VL_UnlockRamp | CW_VL_ReferenceRamp;
}

void NodeProfile402Vl::setTarget(qint16 velocity)
{
    _node->writeObject(_targetObjectId, QVariant(velocity));
}

quint16 NodeProfile402Vl::getSpecificControlWord()
{
    return _cmdControlWordSpecific;
}

void NodeProfile402Vl::setEnableRamp(bool ok)
{
    if (ok)
    {
        _cmdControlWordSpecific |= CW_VL_EnableRamp;
    }
    else
    {
        _cmdControlWordSpecific = (_cmdControlWordSpecific & ~CW_VL_EnableRamp);
    }
}

bool NodeProfile402Vl::isEnableRamp(void)
{
    return _cmdControlWordSpecific & CW_VL_EnableRamp;
}

void NodeProfile402Vl::setUnlockRamp(bool ok)
{
    if (ok)
    {
        _cmdControlWordSpecific |= CW_VL_UnlockRamp;
    }
    else
    {
        _cmdControlWordSpecific = (_cmdControlWordSpecific & ~CW_VL_UnlockRamp);
    }
}

bool NodeProfile402Vl::isUnlockRamp(void)
{
    return _cmdControlWordSpecific & CW_VL_UnlockRamp;
}

void NodeProfile402Vl::setReferenceRamp(bool ok)
{
    if (ok)
    {
        _cmdControlWordSpecific |= CW_VL_ReferenceRamp;
    }
    else
    {
        _cmdControlWordSpecific = (_cmdControlWordSpecific & ~CW_VL_ReferenceRamp);
    }
}

bool NodeProfile402Vl::isReferenceRamp(void)
{
    return _cmdControlWordSpecific & CW_VL_ReferenceRamp;
}

void NodeProfile402Vl::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId == _targetObjectId)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            emit isAppliedTarget();
        }
    }

    if ((objId == _controlWordObjectId) && _nodeProfile402->actualMode() == _mode)
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
            _cmdControlWordSpecific = (controlWord & (CW_VL_EnableRamp | CW_VL_ReferenceRamp | CW_VL_UnlockRamp));
            emit _nodeProfile402->enableRampEvent((_cmdControlWordSpecific & CW_VL_EnableRamp) >> 4);
            emit _nodeProfile402->referenceRampEvent((_cmdControlWordSpecific & CW_VL_ReferenceRamp) >> 6);
            emit _nodeProfile402->unlockRampEvent((_cmdControlWordSpecific & CW_VL_UnlockRamp) >> 5);
        }
    }
}
