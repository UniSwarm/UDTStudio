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

#include "modevl.h"
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

ModeVl::ModeVl(NodeProfile402 *nodeProfile402)
    : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_TARGET, _axisId);
    _controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD, _axisId);
    _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    setNodeInterrest(_node);
    registerObjId(_targetObjectId);
    registerObjId(_controlWordObjectId);

    _mode = NodeProfile402::OperationMode::VL;
    _cmdControlWordSpecific = CW_VL_EnableRamp | CW_VL_UnlockRamp | CW_VL_ReferenceRamp;
}

void ModeVl::setTarget(qint16 velocity)
{
    _node->writeObject(_targetObjectId, QVariant(velocity));
}

quint16 ModeVl::getSpecificControlWord()
{
    return _cmdControlWordSpecific;
}

void ModeVl::setEnableRamp(bool ok)
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

bool ModeVl::isEnableRamp(void)
{
    return _cmdControlWordSpecific & CW_VL_EnableRamp;
}

void ModeVl::setUnlockRamp(bool ok)
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

bool ModeVl::isUnlockRamp(void)
{
    return _cmdControlWordSpecific & CW_VL_UnlockRamp;
}

void ModeVl::setReferenceRamp(bool ok)
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

bool ModeVl::isReferenceRamp(void)
{
    return _cmdControlWordSpecific & CW_VL_ReferenceRamp;
}

void ModeVl::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
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
