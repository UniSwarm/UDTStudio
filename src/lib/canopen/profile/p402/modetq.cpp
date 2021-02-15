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
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TARGET_TORQUE, _axisId);
    _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    _mode = NodeProfile402::OperationMode::TQ;
}

void ModeTq::setTarget(qint32 target)
{
    _node->writeObject(_targetObjectId, QVariant(static_cast<qint16>(target)));
}

quint16 ModeTq::getSpecificCwFlag()
{
    return _cmdControlWordFlag;
}

void ModeTq::setCwDefaultflag()
{
    _cmdControlWordFlag = 0;
}

void ModeTq::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
}
