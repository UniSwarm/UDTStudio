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

#include "modedty.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

enum ControlWordDty : quint16
{
    CW_DTY_EnableRamp = 0x10,
    CW_Mask = 0x70
};

ModeDty::ModeDty(NodeProfile402 *nodeProfile402)
    : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_MS_DUTY_CYCLE_MODE_TARGET, _nodeProfile402->axisId());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _demandObjectId = IndexDb402::getObjectId(IndexDb402::OD_MS_DUTY_CYCLE_MODE_DEMAND, _nodeProfile402->axisId());
    _slopeObjectId = IndexDb402::getObjectId(IndexDb402::OD_MS_DUTY_CYCLE_MODE_SLOPE, _nodeProfile402->axisId());
    _maxObjectId = IndexDb402::getObjectId(IndexDb402::OD_MS_DUTY_CYCLE_MODE_MAX, _nodeProfile402->axisId());

    _demandObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());
    _slopeObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());
    _maxObjectId.setBusIdNodeId(_nodeProfile402->node()->busId(), _nodeProfile402->node()->nodeId());

    _mode = NodeProfile402::OperationMode::TQ;
    _cmdControlWordFlag = CW_DTY_EnableRamp;
}

void ModeDty::setEnableRamp(bool ok)
{
    if (ok)
    {
        _cmdControlWordFlag |= CW_DTY_EnableRamp;
    }
    else
    {
        _cmdControlWordFlag = (_cmdControlWordFlag & ~CW_DTY_EnableRamp);
    }
    quint16 cw = static_cast<quint16>(_nodeProfile402->node()->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_Mask) | _cmdControlWordFlag;
    _nodeProfile402->node()->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModeDty::isEnableRamp() const
{
    return ((_cmdControlWordFlag & CW_DTY_EnableRamp) >> 4) != 0;
}

const NodeObjectId &ModeDty::targetObjectId() const
{
    return _targetObjectId;
}

const NodeObjectId &ModeDty::demandObjectId() const
{
    return _demandObjectId;
}

const NodeObjectId &ModeDty::slopeObjectId() const
{
    return _slopeObjectId;
}

const NodeObjectId &ModeDty::maxObjectId() const
{
    return _maxObjectId;
}

void ModeDty::setTarget(qint32 target)
{
    _nodeProfile402->node()->writeObject(_targetObjectId, QVariant(static_cast<qint16>(target)));
}

quint16 ModeDty::getSpecificCwFlag()
{
    return _cmdControlWordFlag;
}

void ModeDty::setCwDefaultflag()
{
    _cmdControlWordFlag = 0;
}

void ModeDty::readRealTimeObjects()
{
    _nodeProfile402->node()->readObject(_demandObjectId);
}

void ModeDty::readAllObjects()
{
    readRealTimeObjects();
    _nodeProfile402->node()->readObject(_targetObjectId);
    _nodeProfile402->node()->readObject(_slopeObjectId);
    _nodeProfile402->node()->readObject(_maxObjectId);
}

void ModeDty::reset()
{
    _nodeProfile402->node()->readObject(_targetObjectId);
}

void ModeDty::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    Q_UNUSED(objId)
    Q_UNUSED(flags)
}
