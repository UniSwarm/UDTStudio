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

#include "modetc.h"

#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

ModeTc::ModeTc(NodeProfile402 *nodeProfile402)
    : ModeHm(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_TARGET, _nodeProfile402->axis());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _torqueDemandObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_DEMAND, _nodeProfile402->axis());
    _torqueDemandObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _torqueActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE, _nodeProfile402->axis());
    _torqueActualValueObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _targetSlopeObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_SLOPE, _nodeProfile402->axis());
    _targetSlopeObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _maxTorqueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_TORQUE, _nodeProfile402->axis());
    _maxTorqueObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _commutationAngleObjectId = IndexDb402::getObjectId(IndexDb402::OD_CSTCA_COMMUTATION_ANGLE, _nodeProfile402->axis());
    _commutationAngleObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _torqueOffsetObjectId = IndexDb402::getObjectId(IndexDb402::OD_CSP_TORQUE_OFFSET, _nodeProfile402->axis());
    _torqueOffsetObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _maxMotorSpeedObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_MOTOR_SPEED, _nodeProfile402->axis());
    _maxMotorSpeedObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _motorRatedTorqueObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_TORQUE, _nodeProfile402->axis());
    _motorRatedTorqueObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());
}

const NodeObjectId &ModeTc::targetObjectId() const
{
    return _targetObjectId;
}

const NodeObjectId &ModeTc::torqueDemandObjectId() const
{
    return _torqueDemandObjectId;
}

const NodeObjectId &ModeTc::torqueActualValueObjectId() const
{
    return _torqueActualValueObjectId;
}

const NodeObjectId &ModeTc::commutationAngleObjectId() const
{
    return _commutationAngleObjectId;
}

const NodeObjectId &ModeTc::torqueOffsetObjectId() const
{
    return _torqueOffsetObjectId;
}

const NodeObjectId &ModeTc::targetSlopeObjectId() const
{
    return _targetSlopeObjectId;
}

const NodeObjectId &ModeTc::maxTorqueObjectId() const
{
    return _maxTorqueObjectId;
}

const NodeObjectId &ModeTc::maxMotorSpeedObjectId() const
{
    return _maxMotorSpeedObjectId;
}

const NodeObjectId &ModeTc::motorRatedTorqueObjectId() const
{
    return _motorRatedTorqueObjectId;
}

void ModeTc::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    Q_UNUSED(objId)
    Q_UNUSED(flags)
}

void ModeTc::readRealTimeObjects()
{
    _nodeProfile402->node()->readObject(_torqueDemandObjectId);
    _nodeProfile402->node()->readObject(_torqueActualValueObjectId);
}

void ModeTc::readAllObjects()
{
    readRealTimeObjects();
    _nodeProfile402->node()->readObject(_targetObjectId);
    _nodeProfile402->node()->readObject(_commutationAngleObjectId);
    _nodeProfile402->node()->readObject(_torqueOffsetObjectId);
    _nodeProfile402->node()->readObject(_targetSlopeObjectId);
    _nodeProfile402->node()->readObject(_maxTorqueObjectId);
    _nodeProfile402->node()->readObject(_maxMotorSpeedObjectId);
    _nodeProfile402->node()->readObject(_motorRatedTorqueObjectId);
    ModeHm::readAllObjects();
}
