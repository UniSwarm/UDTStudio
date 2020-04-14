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
#include <QDataStream>
#include <QDebug>

RPDO::RPDO(Node *node, quint8 number)
    : PDO(node, number)
{
    _cobId = 0x200 + 0x100 * _pdoNumber + node->nodeId();
    _cobIds.append(_cobId);
    _objectCommId = 0x1400 + _pdoNumber;
    _objectMappingId = 0x1600 + _pdoNumber;

    registerObjId({_objectCommId, 255});
    registerObjId({_objectMappingId, 255});
    setNodeInterrest(node);

    _objectCommList = {{_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_COB_ID},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_TRASMISSION_TYPE},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_INHIBIT_TIME},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_SYNC_START_VALUE}};
}

QString RPDO::type() const
{
    return QLatin1String("RPDO") + QString::number(_pdoNumber + 1, 10);
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
    readMapping();
}

bool RPDO::setTransmissionType(quint8 type)
{
    state = STATE_FREE;
    if ((type <= RPDO_SYNC_MAX) || (type== RPDO_EVENT_MS) || (type == RPDO_EVENT_DP))
    {
        _waitingConf.transType = type;
        _node->writeObject(_objectCommList[1].index, PDO_COMM_TRASMISSION_TYPE, _waitingConf.transType);
        return true;
    }
    else
    {
        // ERROR_PARAM_IMCOMPATIBILITY
        return false;
    }
}

quint8 RPDO::transmissionType()
{
    NodeObjectId object(_objectCommId, PDO_COMM_TRASMISSION_TYPE);
    return static_cast<quint8>(_node->nodeOd()->value(object).toUInt());
}

void RPDO::receiveSync()
{
    sendData();
}
