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
    state = STATE_FREE;
    _rpdo = new RPDO_();

    registerObjId({_objectCommId, 0x01});
    registerObjId({_objectMappingId, 255});
    setNodeInterrest(node);

    createListObjectMapped();

    // connect(node->bus()->sync(), &Sync::syncEmitted, this, &RPDO::receiveSync);
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
    if ((objId.index == _objectCommId) && objId.subIndex == 0x01 && (state == STATE_FREE))
    {
        state = STATE_DEACTIVATE;
        applyMapping();
    }
    if ((objId.index == _objectMappingId) && (objId.subIndex == 0x00) && (state == STATE_DEACTIVATE))
    {
        state = STATE_DISABLE;
        _numberObjectCurrent = 0;
        applyMapping();
    }
    if ((objId.index == _objectMappingId) && (objId.subIndex != 0x00) && (state == STATE_DISABLE))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            // TODO     QUOI FAIRE????
            state = STATE_FREE;
            return;
        }
        _numberObjectCurrent++;
        if (_numberObjectCurrent == _objectMap.size())
        {
            state = STATE_MODIFY;
        }
        applyMapping();
    }
    if ((objId.index == _objectMappingId) && (objId.subIndex == 0x00) && (state == STATE_MODIFY))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            // TODO     QUOI FAIRE????
            state = STATE_FREE;
            return;
        }
        state = STATE_ENABLE;
        applyMapping();
    }
    if ((objId.index == _objectCommId) && objId.subIndex == 0x01 && (state == STATE_ENABLE))
    {
        state = STATE_ACTIVATE;
        applyMapping();
    }
}

quint8 RPDO::number() const
{
    return _number;
}

void RPDO::receiveSync()
{
    saveData();
    sendData(_rpdo->data);
}
void RPDO::saveData()
{
    for (NodeObjectId object : _objectMapped)
    {
        quint8 size = static_cast<quint8>(QMetaType::sizeOf(object.dataType));
        if (size > 8)
        {
            // ERROR : CO_SDO_ABORT_CODE_EXCEED_PDO_LENGTH
            return;
        }
        arrangeData(_rpdo->data, _nodeOd->value(object.index, object.subIndex));
    }
}
