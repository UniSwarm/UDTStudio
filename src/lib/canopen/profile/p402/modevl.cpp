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
    CW_Mask = 0x70
};

ModeVl::ModeVl(NodeProfile402 *nodeProfile402)
    : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_VL_VELOCITY_TARGET, _nodeProfile402->axisId());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _mode = NodeProfile402::OperationMode::VL;

    setNodeInterrest(_nodeProfile402->node());
    registerObjId(_controlWordObjectId);

    // TODO : redesign the process for default value witg setCwDefaultflag()
    _cmdControlWordFlag = CW_VL_EnableRamp | CW_VL_UnlockRamp | CW_VL_ReferenceRamp;
}

void ModeVl::setEnableRamp(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_VL_EnableRamp;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_VL_EnableRamp);
    }
    quint16 cw = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _nodeProfile402->node()->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModeVl::isEnableRamp(void)
{
    return (_cmdControlWordFlag & CW_VL_EnableRamp) >> 4;
}

void ModeVl::setUnlockRamp(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_VL_UnlockRamp;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_VL_UnlockRamp);
    }
    quint16 cw = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _nodeProfile402->node()->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModeVl::isUnlockRamp(void)
{
    return (_cmdControlWordFlag & CW_VL_UnlockRamp) >> 5;
}

void ModeVl::setReferenceRamp(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_VL_ReferenceRamp;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_VL_ReferenceRamp);
    }
    quint16 cw = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _nodeProfile402->node()->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModeVl::isReferenceRamp(void)
{
    return (_cmdControlWordFlag & CW_VL_ReferenceRamp) >> 6;
}

void ModeVl::setTarget(qint32 target)
{
    _nodeProfile402->node()->writeObject(_targetObjectId, QVariant(static_cast<qint16>(target)));
}

quint16 ModeVl::getSpecificCwFlag ()
{
    return _cmdControlWordFlag & CW_Mask;
}

void ModeVl::setCwDefaultflag()
{
    _cmdControlWordFlag = CW_VL_EnableRamp | CW_VL_UnlockRamp | CW_VL_ReferenceRamp;
}

void ModeVl::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((objId == _controlWordObjectId) && _nodeProfile402->actualMode() == _mode)
    {
        quint16 controlWord = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
        _cmdControlWordFlag = controlWord & CW_Mask;

        emit enableRampEvent((_cmdControlWordFlag & CW_VL_EnableRamp) >> 4);
        emit referenceRampEvent((_cmdControlWordFlag & CW_VL_ReferenceRamp) >> 6);
        emit unlockRampEvent((_cmdControlWordFlag & CW_VL_UnlockRamp) >> 5);
    }
}
