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

#include "modepp.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

enum ControlWordIP : quint16
{
    CW_PP_NewSetPoint = 0x10,
    CW_PP_ChangeSetImmediately = 0x20,
    CW_PP_AbsRel = 0x40,
    CW_PP_ChangeOnSetPoint = 0x200,
    CW_Halt = 0x100
};

ModePp::ModePp(NodeProfile402 *nodeProfile402)
    : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_SET_POINT, _axisId);
    _controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD, _axisId);
    _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    setNodeInterrest(_node);
    registerObjId(_targetObjectId);
    registerObjId(_controlWordObjectId);

    _mode = NodeProfile402::OperationMode::PP;
    _cmdControlWordSpecific = CW_PP_NewSetPoint;
}

void ModePp::setTarget(qint32 position)
{
    _node->writeObject(_targetObjectId, QVariant(position));
}

quint16 ModePp::getSpecificControlWord()
{
    return _cmdControlWordSpecific;
}

void ModePp::setEnableRamp(bool ok)
{
    //    if (ok)
    //    {
    //        _cmdControlWordSpecific |= CW_IP_EnableRamp;
    //    }
    //    else
    //    {
    //        _cmdControlWordSpecific = (_cmdControlWordSpecific & ~CW_IP_EnableRamp);
    //    }
}

// bool ModePp::isEnableRamp(void)
//{
////    return _cmdControlWordSpecific & CW_IP_EnableRamp;
//}

void ModePp::applyNewSetPoint()
{
    quint16 cmdControlWordSpecific = static_cast<quint16>((_node->nodeOd()->value(_controlWordObjectId).toUInt() | CW_PP_NewSetPoint));
    _node->writeObject(_controlWordObjectId, QVariant(cmdControlWordSpecific));
}

void ModePp::setChangeSetImmediately(bool ok)
{
    if (ok)
    {
        _cmdControlWordSpecific |= CW_PP_ChangeSetImmediately;
    }
    else
    {
        _cmdControlWordSpecific = (_cmdControlWordSpecific & ~CW_PP_ChangeSetImmediately);
    }
}

bool ModePp::isChangeSetImmediately()
{
    return _cmdControlWordSpecific & CW_PP_ChangeSetImmediately;
}

void ModePp::setChangeOnSetPoint(bool ok)
{
    if (ok)
    {
        _cmdControlWordSpecific |= CW_PP_ChangeOnSetPoint;
    }
    else
    {
        _cmdControlWordSpecific = (_cmdControlWordSpecific & ~CW_PP_ChangeOnSetPoint);
    }
}

bool ModePp::isChangeOnSetPoint()
{
    return _cmdControlWordSpecific & CW_PP_ChangeOnSetPoint;
}

void ModePp::setAbsRel(bool ok)
{
    if (ok)
    {
        _cmdControlWordSpecific |= CW_PP_AbsRel;
    }
    else
    {
        _cmdControlWordSpecific = (_cmdControlWordSpecific & ~CW_PP_AbsRel);
    }
}

bool ModePp::isAbsRel()
{
    return _cmdControlWordSpecific & CW_PP_AbsRel;
}

void ModePp::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
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
            _cmdControlWordSpecific = (controlWord & (CW_PP_ChangeSetImmediately | CW_PP_AbsRel | CW_PP_ChangeOnSetPoint));
            emit changeSetImmediatelyEvent((_cmdControlWordSpecific & CW_PP_ChangeSetImmediately) >> 5);
            emit changeOnSetPointEvent((_cmdControlWordSpecific & CW_PP_ChangeOnSetPoint) >> 9);
            emit absRelEvent((_cmdControlWordSpecific & CW_PP_AbsRel) >> 6);
        }
    }
}
