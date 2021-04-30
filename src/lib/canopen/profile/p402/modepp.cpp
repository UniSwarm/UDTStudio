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
    CW_Mask = 0x270
};

ModePp::ModePp(NodeProfile402 *nodeProfile402)
    : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_PP_POSITION_TARGET, _nodeProfile402->axisId());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _positionDemandValueLabel = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_DEMAND_VALUE, _nodeProfile402->axisId());
    _positionActualValueLabel = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_ACTUAL_VALUE, _nodeProfile402->axisId());

    _positionRangelLimitMinSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MIN, _nodeProfile402->axisId());
    _positionRangelLimitMaxSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MAX, _nodeProfile402->axisId());
    _softwarePositionLimitMinSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MIN, _nodeProfile402->axisId());
    _softwarePositionLimitMaxSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MAX, _nodeProfile402->axisId());
    _homeOffsetSpinBox = IndexDb402::getObjectId(IndexDb402::OD_HM_HOME_OFFSET, _nodeProfile402->axisId());
    _polaritySpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_VELOCITY, _nodeProfile402->axisId());
    _profileVelocitySpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_VELOCITY, _nodeProfile402->axisId());
    _endVelocitySpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_END_VELOCITY, _nodeProfile402->axisId());
    _maxProfileVelocitySpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_PROFILE_VELOCITY, _nodeProfile402->axisId());
    _maxMotorSpeedSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_MOTOR_SPEED, _nodeProfile402->axisId());
    _profileAccelerationSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_ACCELERATION, _nodeProfile402->axisId());
    _maxAccelerationSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_ACCELERATION, _nodeProfile402->axisId());
    _profileDecelerationSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_DECELERATION, _nodeProfile402->axisId());
    _maxDecelerationSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_DECELERATION, _nodeProfile402->axisId());
    _quickStopDecelerationSpinBox = IndexDb402::getObjectId(IndexDb402::OD_PC_QUICK_STOP_DECELERATION, _nodeProfile402->axisId());

    _positionDemandValueLabel.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _positionActualValueLabel.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());

    _positionRangelLimitMinSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _positionRangelLimitMaxSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _softwarePositionLimitMinSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _softwarePositionLimitMaxSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _homeOffsetSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _polaritySpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _profileVelocitySpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _endVelocitySpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _maxProfileVelocitySpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _maxMotorSpeedSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _profileAccelerationSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _maxAccelerationSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _profileDecelerationSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _maxDecelerationSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _quickStopDecelerationSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());


    _mode = NodeProfile402::OperationMode::PP;

    setNodeInterrest(_nodeProfile402->node());
    registerObjId(_controlWordObjectId);

    // TODO : redesign the process for default value witg setCwDefaultflag()
    _cmdControlWordFlag = 0;
}

void ModePp::newSetPoint(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_PP_NewSetPoint;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_PP_NewSetPoint);
    }

    quint16 cw = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _nodeProfile402->node()->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModePp::isNewSetPoint()
{
    return (_cmdControlWordFlag & CW_PP_NewSetPoint) >> 4;
}

void ModePp::setChangeSetImmediately(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_PP_ChangeSetImmediately;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_PP_ChangeSetImmediately);
    }
    quint16 cw = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _nodeProfile402->node()->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModePp::isChangeSetImmediately()
{
    return (_cmdControlWordFlag & CW_PP_ChangeSetImmediately) >> 5;
}

bool ModePp::isAbsRel()
{
    return (_cmdControlWordFlag & CW_PP_AbsRel) >> 6;
}

void ModePp::setChangeOnSetPoint(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_PP_ChangeOnSetPoint;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_PP_ChangeOnSetPoint);
    }
    quint16 cw = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _nodeProfile402->node()->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModePp::isChangeOnSetPoint()
{
    return (_cmdControlWordFlag & CW_PP_ChangeOnSetPoint) >> 9;
}

void ModePp::setAbsRel(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_PP_AbsRel;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_PP_AbsRel);
    }
    quint16 cw = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _nodeProfile402->node()->writeObject(_controlWordObjectId, QVariant(cw));
}

void ModePp::setTarget(qint32 target)
{
    _nodeProfile402->node()->writeObject(_targetObjectId, QVariant(target));
}

quint16 ModePp::getSpecificCwFlag()
{
    return _cmdControlWordFlag & CW_Mask;
}

void ModePp::setCwDefaultflag()
{
    _cmdControlWordFlag = 0;
}

void ModePp::readRealTimeObjects()
{
    _nodeProfile402->node()->readObject(_positionDemandValueLabel);
    _nodeProfile402->node()->readObject( _positionActualValueLabel);
}

void ModePp::readAllObjects()
{
    _nodeProfile402->node()->readObject(_positionDemandValueLabel);
    _nodeProfile402->node()->readObject(_positionActualValueLabel);
    _nodeProfile402->node()->readObject(_positionRangelLimitMinSpinBox);
    _nodeProfile402->node()->readObject(_positionRangelLimitMaxSpinBox);
    _nodeProfile402->node()->readObject(_softwarePositionLimitMinSpinBox);
    _nodeProfile402->node()->readObject(_softwarePositionLimitMaxSpinBox);
    _nodeProfile402->node()->readObject(_profileVelocitySpinBox);
    _nodeProfile402->node()->readObject(_maxProfileVelocitySpinBox);
    _nodeProfile402->node()->readObject(_maxMotorSpeedSpinBox);
    _nodeProfile402->node()->readObject(_profileAccelerationSpinBox);
    _nodeProfile402->node()->readObject(_maxAccelerationSpinBox);
    _nodeProfile402->node()->readObject(_profileDecelerationSpinBox);
    _nodeProfile402->node()->readObject(_maxDecelerationSpinBox);
}


void ModePp::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((objId == _controlWordObjectId) && _nodeProfile402->actualMode() == _mode)
    {
        quint16 controlWord = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
        _cmdControlWordFlag = controlWord & CW_Mask;

        emit changeNewSetPoint((_cmdControlWordFlag & CW_PP_NewSetPoint) >> 4);
        emit changeSetImmediatelyEvent((_cmdControlWordFlag & CW_PP_ChangeSetImmediately) >> 5);
        emit absRelEvent((_cmdControlWordFlag & CW_PP_AbsRel) >> 6);
        emit changeOnSetPointEvent((_cmdControlWordFlag & CW_PP_ChangeOnSetPoint) >> 9);
    }
}
