/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#include "nodeprofile402tq.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

NodeProfile402Tq::NodeProfile402Tq(Node *node, uint8_t axis, NodeProfile402 *nodeProfile402)
    : _node(node), _axis(axis), _nodeProfile402(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_TQ_TARGET_TORQUE, axis);
    _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId({_targetObjectId});
    setNodeInterrest(_node);
}

void NodeProfile402Tq::setTarget(qint16 torque)
{
    _node->writeObject(_targetObjectId, QVariant(torque));
}

void NodeProfile402Tq::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId == _targetObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else
        {
            emit isAppliedTarget();
        }
    }
}
