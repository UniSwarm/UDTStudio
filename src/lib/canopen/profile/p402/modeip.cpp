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

#include "modeip.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

enum ControlWordIP : quint16
{
    CW_IP_EnableRamp = 0x10,
};

ModeIp::ModeIp(NodeProfile402 *nodeProfile402)
    : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_DATA_RECORD_SET_POINT, _nodeProfile402->axisId());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _bufferClearObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_BUFFER_CLEAR, _nodeProfile402->axisId());
    _bufferClearObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _positionDemandValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_DEMAND_VALUE, _nodeProfile402->axisId());
    _positionActualValueObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_ACTUAL_VALUE, _nodeProfile402->axisId());

    _timePeriodUnitObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_TIME_PERIOD_TIME_UNITS, _nodeProfile402->axisId());
    _timePeriodIndexObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_TIME_PERIOD_TIME_INDEX, _nodeProfile402->axisId());

    _positionRangelLimitMinObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MIN, _nodeProfile402->axisId());
    _positionRangelLimitMaxObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_POSITION_RANGE_LIMIT_MAX, _nodeProfile402->axisId());
    _softwarePositionLimitMinObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MIN, _nodeProfile402->axisId());
    _softwarePositionLimitMaxObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_SOFTWARE_POSITION_LIMIT_MAX, _nodeProfile402->axisId());

    _maxProfileVelocityObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_PROFILE_VELOCITY, _nodeProfile402->axisId());
    _maxMotorSpeedObjectId = IndexDb402::getObjectId(IndexDb402::OD_PC_MAX_MOTOR_SPEED, _nodeProfile402->axisId());

    _homeOffsetObjectId = IndexDb402::getObjectId(IndexDb402::OD_HM_HOME_OFFSET, _nodeProfile402->axisId());



    //_polarityObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_TIME_PERIOD_TIME_UNITS, _nodeProfile402->axisId());

    _positionDemandValueObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _positionActualValueObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());

    _timePeriodUnitObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _timePeriodIndexObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());

    _positionRangelLimitMinObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _positionRangelLimitMaxObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _softwarePositionLimitMinObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _softwarePositionLimitMaxObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());

    _maxProfileVelocityObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _maxMotorSpeedObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());

    _homeOffsetObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());
    _polarityObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(),_nodeProfile402->node()->nodeId());










    _mode = NodeProfile402::OperationMode::IP;

    setNodeInterrest(_nodeProfile402->node());
    registerObjId(_controlWordObjectId);

    // TODO : redesign the process for default value witg setCwDefaultflag()
    _cmdControlWordFlag = CW_IP_EnableRamp;
}

void ModeIp::setEnableRamp(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_IP_EnableRamp;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_IP_EnableRamp);
    }
    quint16 cw = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_IP_EnableRamp) | _cmdControlWordFlag;
    _nodeProfile402->node()->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModeIp::isEnableRamp(void)
{
    return (_cmdControlWordFlag & CW_IP_EnableRamp) >> 4;
}

void ModeIp::bufferClear()
{
    quint8 value = 0;
    _nodeProfile402->node()->writeObject(_bufferClearObjectId, QVariant(value));
}

void ModeIp::setTarget(qint32 target)
{
    _nodeProfile402->node()->writeObject(_targetObjectId, QVariant(target));
}

quint16 ModeIp::getSpecificCwFlag()
{
    return _cmdControlWordFlag & CW_IP_EnableRamp;
}

void ModeIp::setCwDefaultflag()
{
    _cmdControlWordFlag = CW_IP_EnableRamp;
}

void ModeIp::readRealTimeObjects()
{
    _nodeProfile402->node()->readObject(_positionDemandValueObjectId);
    _nodeProfile402->node()->readObject(_positionActualValueObjectId);
}

void ModeIp::readAllObjects()
{
    _nodeProfile402->node()->readObject(_positionDemandValueObjectId);
    _nodeProfile402->node()->readObject(_positionActualValueObjectId);
    _nodeProfile402->node()->readObject(_positionRangelLimitMinObjectId);
    _nodeProfile402->node()->readObject(_positionRangelLimitMaxObjectId);
    _nodeProfile402->node()->readObject(_softwarePositionLimitMinObjectId);
    _nodeProfile402->node()->readObject(_softwarePositionLimitMaxObjectId);
    _nodeProfile402->node()->readObject( _maxProfileVelocityObjectId);
    _nodeProfile402->node()->readObject(_maxMotorSpeedObjectId);
}

void ModeIp::reset()
{
    _nodeProfile402->node()->readObject(_targetObjectId);
}

void ModeIp::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((objId == _controlWordObjectId) && _nodeProfile402->actualMode() == _mode)
    {
        quint16 controlWord = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
        _cmdControlWordFlag = controlWord & CW_IP_EnableRamp;

        emit enableRampEvent(_cmdControlWordFlag >> 4);
    }
}
