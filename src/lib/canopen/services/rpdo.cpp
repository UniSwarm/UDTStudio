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

#include "rpdo.h"

#include "canopenbus.h"

#include <QDebug>

RPDO::RPDO(Node *node, quint8 number)
    : PDO(node, number)
{
    _cobId = 0x200 + 0x100 * _number + node->nodeId();
    _cobIds.append(_cobId);
    _objectCommId = 0x1400 + _number;
    _objectMappingId = 0x1600 + _number;

    registerObjId({_objectCommId, 255});
    registerObjId({_objectMappingId, 255});
    setNodeInterrest(node);

    _objectCommList = {{_objectCommId, 0x1}, {_objectCommId, 0x2}, {_objectCommId, 0x3}, {_objectCommId, 0x5}};
}

QString RPDO::type() const
{
    return QLatin1String("RPDO") + QString::number(_number + 1, 10);
}

void RPDO::parseFrame(const QCanBusFrame &frame)
{
    Q_UNUSED(frame)
}

void RPDO::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (statusPdo == STATE_READ)
    {
        notifyReadPdo(objId, flags);
    }

    if (statusPdo == STATE_WRITE)
    {
        notifyWritePdo(objId, flags);
    }
}

void RPDO::setBus(CanOpenBus *bus)
{
    _bus = bus;
    connect(_bus->sync(), &Sync::syncEmitted, this, &RPDO::receiveSync);
    refreshPdo();
}

quint8 RPDO::number() const
{
    return _number;
}

void RPDO::setCommParam(PDO_conf &conf)
{
    if ((conf.transType <= RPDO_SYNC_MAX) || (conf.transType == RPDO_EVENT_MS) || (conf.transType == RPDO_EVENT_DP))
    {
        _waitingParam.transType = conf.transType;
    }
    else
    {
        _waitingParam.transType = 0;
    }

    _waitingParam.eventTimer = conf.eventTimer;
    _waitingParam.inhibitTime = conf.inhibitTime;
    _waitingParam.syncStartValue = 0;
}

void RPDO::receiveSync()
{
    saveData();
    sendData(data);
}
void RPDO::saveData()
{
    for (NodeObjectId object : _objectCurrentMapped)
    {
        quint8 size = static_cast<quint8>(QMetaType::sizeOf(object.dataType));
        if (size > 8)
        {
            // ERROR : CO_SDO_ABORT_CODE_EXCEED_PDO_LENGTH
            return;
        }
        arrangeData(data, _nodeOd->value(object.index, object.subIndex));
    }
}
