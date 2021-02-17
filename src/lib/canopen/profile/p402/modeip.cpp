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
    _targetObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_SET_POINT, _axisId);
    _targetObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    _bufferClearObjectId = IndexDb402::getObjectId(IndexDb402::OD_IP_BUFFER_CLEAR, _axisId);
    _bufferClearObjectId.setBusIdNodeId(_node->busId(), _node->nodeId());

    _mode = NodeProfile402::OperationMode::IP;

    setNodeInterrest(nodeProfile402->node());
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
    quint16 cw = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_IP_EnableRamp) | _cmdControlWordFlag;
    _node->writeObject(_controlWordObjectId, QVariant(cw));
}

bool ModeIp::isEnableRamp(void)
{
    return (_cmdControlWordFlag & CW_IP_EnableRamp) >> 4;
}

void ModeIp::bufferClear()
{
    quint8 value = 0;
    _node->writeObject(_bufferClearObjectId, QVariant(value));
}

void ModeIp::setTarget(qint32 target)
{
    _node->writeObject(_targetObjectId, QVariant(target));
}

quint16 ModeIp::getSpecificCwFlag()
{
    return _cmdControlWordFlag & CW_IP_EnableRamp;
}

void ModeIp::setCwDefaultflag()
{
    _cmdControlWordFlag = CW_IP_EnableRamp;
    quint16 cw = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());
    cw = (cw & ~CW_IP_EnableRamp) | _cmdControlWordFlag;
    _node->writeObject(_controlWordObjectId, QVariant(cw));
}

void ModeIp::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags & SDO::FlagsRequest::Error)
    {
        return;
    }

    if ((objId == _controlWordObjectId) && _nodeProfile402->actualMode() == _mode)
    {
        quint16 controlWord = static_cast<quint16>(_node->nodeOd()->value(_controlWordObjectId).toUInt());

        if (_cmdControlWordFlag != (controlWord & (CW_IP_EnableRamp)))
        {
            emit enableRampEvent(_cmdControlWordFlag >> 4);
        }
        _cmdControlWordFlag = (controlWord & (CW_IP_EnableRamp));
    }
}
