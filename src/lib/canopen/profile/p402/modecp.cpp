/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include "modecp.h"

#include "indexdb402.h"
#include "nodeprofile402.h"

enum ControlWordIP : quint16
{
    CW_PP_AbsRel = 0x40,
    CW_Mask = 0x270
};

ModeCp::ModeCp(NodeProfile402 *nodeProfile402)
    : ModePc(nodeProfile402)
{
    _cmdControlWordFlag = 0;

    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_CP_POSITION_TARGET, _nodeProfile402->axis());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _mode = NodeProfile402::OperationMode::CP;
}

void ModeCp::setAbsRel(bool ok)
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

bool ModeCp::isAbsRel() const
{
    return ((_cmdControlWordFlag & CW_PP_AbsRel) >> 6) != 0;
}

const NodeObjectId &ModeCp::targetObjectId() const
{
    return _targetObjectId;
}

void ModeCp::setTarget(qint32 target)
{
    _nodeProfile402->node()->writeObject(_targetObjectId, QVariant(target));
}

quint16 ModeCp::getSpecificCwFlag()
{
    return _cmdControlWordFlag;
}

void ModeCp::setCwDefaultflag()
{
    _cmdControlWordFlag = 0;
}

void ModeCp::readAllObjects()
{
    ModePc::readAllObjects();
    _nodeProfile402->node()->readObject(_targetObjectId);
}
