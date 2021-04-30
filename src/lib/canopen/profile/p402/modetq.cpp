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

#include "modetq.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

ModeTq::ModeTq(NodeProfile402 *nodeProfile402)
    : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_TARGET, _nodeProfile402->axisId());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _torqueDemandLabel = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_DEMAND, _nodeProfile402->axisId());
    _torqueActualValueLabel = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_ACTUAL_VALUE, _nodeProfile402->axisId());
    _currentActualValueLabel = IndexDb402::getObjectId(IndexDb402::OD_TQ_CURRENT_ACTUAL_VALUE, _nodeProfile402->axisId());

    _targetSlopeSpinBox = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_SLOPE, _nodeProfile402->axisId());
    _torqueProfileTypeSpinBox = IndexDb402::getObjectId(IndexDb402::OD_TQ_TORQUE_PROFILE_TYPE, _nodeProfile402->axisId());
    _maxTorqueSpinBox = IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_TORQUE, _nodeProfile402->axisId());
    _maxCurrentSpinBox = IndexDb402::getObjectId(IndexDb402::OD_TQ_MAX_CURRENT, _nodeProfile402->axisId());
    _motorRatedTorqueSpinBox = IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_TORQUE, _nodeProfile402->axisId());
    _motorRatedCurrentSpinBox = IndexDb402::getObjectId(IndexDb402::OD_TQ_MOTOR_RATED_CURRENT, _nodeProfile402->axisId());
    _dcLinkVoltageLabel = IndexDb402::getObjectId(IndexDb402::OD_TQ_DC_LINK_CIRCUIT_VOLTAGE, _nodeProfile402->axisId());

    _torqueDemandLabel.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _torqueActualValueLabel.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _currentActualValueLabel.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());

    _targetSlopeSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _torqueProfileTypeSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _maxTorqueSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _maxCurrentSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _motorRatedTorqueSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _motorRatedCurrentSpinBox.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _dcLinkVoltageLabel.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());

    _mode = NodeProfile402::OperationMode::TQ;
}

void ModeTq::setTarget(qint32 target)
{
    _nodeProfile402->node()->writeObject(_targetObjectId, QVariant(static_cast<qint16>(target)));
}

quint16 ModeTq::getSpecificCwFlag()
{
    return _cmdControlWordFlag;
}

void ModeTq::setCwDefaultflag()
{
    _cmdControlWordFlag = 0;
}

void ModeTq::readRealTimeObjects()
{
    _nodeProfile402->node()->readObject(_torqueDemandLabel);
    _nodeProfile402->node()->readObject(_torqueActualValueLabel);
}

void ModeTq::readAllObjects()
{
    _nodeProfile402->node()->readObject(_torqueDemandLabel);
    _nodeProfile402->node()->readObject(_torqueActualValueLabel);
    //_nodeProfile402->node()->readObject(_currentActualValueLabel);
    _nodeProfile402->node()->readObject(_targetSlopeSpinBox);
    //_nodeProfile402->node()->readObject(_torqueProfileTypeSpinBox);
    _nodeProfile402->node()->readObject(_maxTorqueSpinBox);
    //_nodeProfile402->node()->readObject(_maxCurrentSpinBox);
    //_nodeProfile402->node()->readObject(_motorRatedTorqueSpinBox);
    //_nodeProfile402->node()->readObject(_motorRatedCurrentSpinBox);
    //_nodeProfile402->node()->readObject(_dcLinkVoltageLabel);
}

void ModeTq::reset()
{
    _nodeProfile402->node()->readObject(_targetObjectId);
}

void ModeTq::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
}
