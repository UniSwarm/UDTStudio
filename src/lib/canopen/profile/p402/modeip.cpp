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
    : ModePc(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_DATA_RECORD_SET_POINT, _nodeProfile402->axis());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _bufferClearObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_BUFFER_CLEAR, _nodeProfile402->axis());
    _bufferClearObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _timePeriodUnitObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_TIME_PERIOD_TIME_UNITS, _nodeProfile402->axis());
    _timePeriodUnitObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _timePeriodIndexObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_TIME_PERIOD_TIME_INDEX, _nodeProfile402->axis());
    _timePeriodIndexObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

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

bool ModeIp::isEnableRamp() const
{
    return ((_cmdControlWordFlag & CW_IP_EnableRamp) >> 4) != 0;
}

void ModeIp::bufferClear()
{
    quint8 value = 0;
    _nodeProfile402->node()->writeObject(_bufferClearObjectId, QVariant(value));
}

const NodeObjectId &ModeIp::targetObjectId() const
{
    return _targetObjectId;
}

const NodeObjectId &ModeIp::bufferClearObjectId() const
{
    return _bufferClearObjectId;
}

const NodeObjectId &ModeIp::timePeriodUnitsObjectId() const
{
    return _timePeriodUnitObjectId;
}

const NodeObjectId &ModeIp::timePeriodIndexObjectId() const
{
    return _timePeriodIndexObjectId;
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

void ModeIp::reset()
{
    _nodeProfile402->node()->readObject(_targetObjectId);
}

void ModeIp::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if ((flags & NodeOd::FlagsRequest::Error) != 0)
    {
        return;
    }

    if ((objId == _controlWordObjectId) && _nodeProfile402->actualMode() == _mode && _nodeProfile402->modeStatus() == NodeProfile402::MODE_CHANGED)
    {
        quint16 controlWord = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
        _cmdControlWordFlag = controlWord & CW_IP_EnableRamp;

        emit enableRampEvent((_cmdControlWordFlag >> 4) != 0);
    }
}
