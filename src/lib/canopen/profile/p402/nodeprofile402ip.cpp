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

#include "nodeprofile402ip.h"
#include "indexdb402.h"
#include "node.h"
#include "nodeprofile402.h"

enum ControlWordIP : quint16
{
    CW_IP_EnableRamp = 0x10,
    CW_Halt = 0x100
};

NodeProfile402Ip::NodeProfile402Ip(Node *node, NodeProfile402 *nodeProfile402)
    : _node(node), _nodeProfile402(nodeProfile402)
{
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_SET_POINT);
    _controlWordObjectId = IndexDb402::getObjectId(IndexDb402::OD_CONTROLWORD);
    _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    _controlWordObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());
    registerObjId({_targetObjectId});
    registerObjId({_controlWordObjectId});
    setNodeInterrest(_node);
    _enableRamp = false;

    _mode = 7;
}

void NodeProfile402Ip::setTarget(qint32 position)
{
    _node->writeObject(_targetObjectId, QVariant(position));
}

quint16 NodeProfile402Ip::enableMode(quint16 cmdControlWord)
{
    cmdControlWord |= CW_IP_EnableRamp;
    return cmdControlWord;
}

quint16 NodeProfile402Ip::setEnableRamp(quint16 cmdControlWord, bool ok)
{
    if (ok)
    {
        cmdControlWord |= CW_IP_EnableRamp;
    }
    else
    {
        cmdControlWord = (cmdControlWord & ~CW_IP_EnableRamp);
    }
    return cmdControlWord;
}

bool NodeProfile402Ip::isEnableRamp(void)
{
    return _enableRamp;
}

void NodeProfile402Ip::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
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
    if ((objId == _controlWordObjectId) && _nodeProfile402->actualMode() == _mode)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else
        {
            quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
            if (((controlWord & CW_IP_EnableRamp) >> 4) != (_enableRamp))
            {
                _enableRamp = (controlWord & CW_IP_EnableRamp);
                emit _nodeProfile402->enableRampEvent((controlWord & CW_IP_EnableRamp));
            }
        }
    }
}
