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
#include "node.h"

enum ControlWordIP : quint16
{
    CW_IP_EnableRamp = 0x10,
    CW_Halt = 0x100
};

NodeProfile402Ip::NodeProfile402Ip(Node *node) : _node(node)
{
    _targetObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x60C1, 1);
    registerObjId({_targetObjectId});
    _controlWordObjectId = NodeObjectId(_node->busId(), _node->nodeId(), 0x6040, 0);
    registerObjId({_controlWordObjectId});

    setNodeInterrest(_node);
}

void NodeProfile402Ip::enableMode(void)
{
    _cmdControlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    _cmdControlWord |= CW_IP_EnableRamp;
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

void NodeProfile402Ip::enableRamp(bool ok)
{
    _cmdControlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    if (ok)
    {
        _cmdControlWord |= CW_IP_EnableRamp;
    }
    else
    {
        _cmdControlWord = (_cmdControlWord & ~CW_IP_EnableRamp);
    }
    _node->writeObject(_controlWordObjectId, QVariant(_cmdControlWord));
}

bool NodeProfile402Ip::isEnableRamp(void)
{
    return _enableRamp;
}

void NodeProfile402Ip::target(qint32 position)
{
    _node->writeObject(_targetObjectId, QVariant(position));
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
    if (objId == _controlWordObjectId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
        }
        else
        {
            quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
            _enableRamp = false;
            if (controlWord & CW_IP_EnableRamp)
            {
                _enableRamp = true;
                emit enableRamp();
            }
        }
    }
}
