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

ModeDty::ModeDty(NodeProfile402 *nodeProfile402)
    : Mode(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_MS_DUTY_CYCLE_MODE_TARGET, _nodeProfile402->axisId());
    _targetObjectId.setBusIdNodeId(_nodeProfile402->busId(), _nodeProfile402->nodeId());

    _mode = NodeProfile402::OperationMode::TQ;
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

void ModeDty::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
}
