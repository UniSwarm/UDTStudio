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

#include "modepc.h"

#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

ModePc::ModePc(NodeProfile402 *nodeProfile402)
    : ModeHm(nodeProfile402)
{
    _positionDemandValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_DEMAND_VALUE, _nodeProfile402->axisId());
    _positionDemandValueObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _positionActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_ACTUAL_VALUE, _nodeProfile402->axisId());
    _positionActualValueObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _positionRangeLimitMinObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MIN, _nodeProfile402->axisId());
    _positionRangeLimitMinObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());
    _positionRangeLimitMaxObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MAX, _nodeProfile402->axisId());
    _positionRangeLimitMaxObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _softwarePositionLimitMinObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MIN, _nodeProfile402->axisId());
    _softwarePositionLimitMinObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());
    _softwarePositionLimitMaxObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MAX, _nodeProfile402->axisId());
    _softwarePositionLimitMaxObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _profileVelocityObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_VELOCITY, _nodeProfile402->axisId());
    _profileVelocityObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _endVelocityObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_END_VELOCITY, _nodeProfile402->axisId());
    _endVelocityObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _maxProfileVelocityObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_PROFILE_VELOCITY, _nodeProfile402->axisId());
    _maxProfileVelocityObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _maxMotorSpeedObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_MOTOR_SPEED, _nodeProfile402->axisId());
    _maxMotorSpeedObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _profileAccelerationObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_ACCELERATION, _nodeProfile402->axisId());
    _profileAccelerationObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _maxAccelerationObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_ACCELERATION, _nodeProfile402->axisId());
    _maxAccelerationObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _profileDecelerationObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_PROFILE_DECELERATION, _nodeProfile402->axisId());
    _profileDecelerationObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _maxDecelerationObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_DECELERATION, _nodeProfile402->axisId());
    _maxDecelerationObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _quickStopDecelerationObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_QUICK_STOP_DECELERATION, _nodeProfile402->axisId());
    _quickStopDecelerationObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());
}

const NodeObjectId &ModePc::positionDemandValueObjectId() const
{
    return _positionDemandValueObjectId;
}

const NodeObjectId &ModePc::positionActualValueObjectId() const
{
    return _positionActualValueObjectId;
}

const NodeObjectId &ModePc::positionRangeLimitMinObjectId() const
{
    return _positionRangeLimitMinObjectId;
}

const NodeObjectId &ModePc::positionRangeLimitMaxObjectId() const
{
    return _positionRangeLimitMaxObjectId;
}

const NodeObjectId &ModePc::softwarePositionLimitMinObjectId() const
{
    return _softwarePositionLimitMinObjectId;
}

const NodeObjectId &ModePc::softwarePositionLimitMaxObjectId() const
{
    return _softwarePositionLimitMaxObjectId;
}

const NodeObjectId &ModePc::profileVelocityObjectId() const
{
    return _profileVelocityObjectId;
}

const NodeObjectId &ModePc::endVelocityObjectId() const
{
    return _endVelocityObjectId;
}

const NodeObjectId &ModePc::maxProfileVelocityObjectId() const
{
    return _maxProfileVelocityObjectId;
}

const NodeObjectId &ModePc::maxMotorSpeedObjectId() const
{
    return _maxMotorSpeedObjectId;
}

const NodeObjectId &ModePc::profileAccelerationObjectId() const
{
    return _profileAccelerationObjectId;
}

const NodeObjectId &ModePc::maxAccelerationObjectId() const
{
    return _maxAccelerationObjectId;
}

const NodeObjectId &ModePc::profileDecelerationObjectId() const
{
    return _profileDecelerationObjectId;
}

const NodeObjectId &ModePc::maxDecelerationObjectId() const
{
    return _maxDecelerationObjectId;
}

const NodeObjectId &ModePc::quickStopDecelerationObjectId() const
{
    return _quickStopDecelerationObjectId;
}

void ModePc::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    Q_UNUSED(objId)
    Q_UNUSED(flags)
}

void ModePc::readRealTimeObjects()
{
    _nodeProfile402->node()->readObject(_positionDemandValueObjectId);
    _nodeProfile402->node()->readObject(_positionActualValueObjectId);
}

void ModePc::readAllObjects()
{
    readRealTimeObjects();
    _nodeProfile402->node()->readObject(_positionRangeLimitMinObjectId);
    _nodeProfile402->node()->readObject(_positionRangeLimitMaxObjectId);
    _nodeProfile402->node()->readObject(_softwarePositionLimitMinObjectId);
    _nodeProfile402->node()->readObject(_softwarePositionLimitMaxObjectId);
    _nodeProfile402->node()->readObject(_profileVelocityObjectId);
    _nodeProfile402->node()->readObject(_maxProfileVelocityObjectId);
    _nodeProfile402->node()->readObject(_maxMotorSpeedObjectId);
    _nodeProfile402->node()->readObject(_profileAccelerationObjectId);
    _nodeProfile402->node()->readObject(_maxAccelerationObjectId);
    _nodeProfile402->node()->readObject(_profileDecelerationObjectId);
    _nodeProfile402->node()->readObject(_maxDecelerationObjectId);
    ModeHm::readAllObjects();
}
