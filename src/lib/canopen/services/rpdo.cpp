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

    _objectCommList = {{_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_NUMBER},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_COB_ID},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_TRANSMISSION_TYPE},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_INHIBIT_TIME},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_EVENT_TIMER}};
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
    if (statusPdo == STATE_NONE)
    {
        managementRespProcessMapping(objId, flags);
    }

    if (statusPdo == STATE_READ)
    {
        managementRespReadCommAndMapping(objId, flags);
    }

    if (statusPdo == STATE_WRITE)
    {
        managementRespProcessMapping(objId, flags);
    }
}

void RPDO::setBus(CanOpenBus *bus)
{
    _bus = bus;
    connect(_bus->sync(), &Sync::syncEmitted, this, &RPDO::receiveSync);
    connect(_bus->sync(), &Sync::signalBeforeSync, this, &RPDO::prepareDataBeforeSync);
    connect(_bus->sync(), &Sync::syncOneRequested, this, &RPDO::receiveSyncOne);
}

/**
 * @brief Set Transmission Type
 * @param type Sync:0x00->0xF0; event-driven (manufacturer-specific):0xFE; event-driven (device profile and application profile specific):0xFF
 * @return bool
 */
bool RPDO::setTransmissionType(quint8 type)
{
    statusPdo = STATE_NONE;
    if ((type <= RPDO_SYNC_MAX) || (type == RPDO_EVENT_MS) || (type == RPDO_EVENT_DP))
    {
        _waitingConf.transType = type;
        _node->writeObject(_objectCommList[1].index, PDO_COMM_TRANSMISSION_TYPE, _waitingConf.transType);
        return true;
    }
    else
    {
        setError(ERROR_PARAM_IMCOMPATIBILITY);
        return false;
    }
}

/**
 * @brief Get Transmission Type
 * @return quint8
 */
quint8 RPDO::transmissionType()
{
    NodeObjectId object(_objectCommId, PDO_COMM_TRANSMISSION_TYPE);
    return static_cast<quint8>(_node->nodeOd()->value(object).toUInt());
}

/**
 * @brief Sync management received
 */
void RPDO::receiveSync()
{
    if ((_objectCurrentMapped.isEmpty()) || (!isEnabled()))
    {
        return;
    }
}

/**
 * @brief Sync One management received
 */
void RPDO::receiveSyncOne()
{
    if ((_objectCurrentMapped.isEmpty()) || (!isEnabled()))
    {
        return;
    }
    prepareDataBeforeSync();
    _bus->sync()->sendSync();
}

/**
 * @brief Prepares the data before the sync signal
 */
void RPDO::prepareDataBeforeSync()
{
    if ((_objectCurrentMapped.isEmpty()) || (!isEnabled()))
    {
        return;
    }

    QDataStream request(&_rpdoDataToSendReqPayload, QIODevice::WriteOnly);
    request.setByteOrder(QDataStream::LittleEndian);

    for (NodeObjectId object : _objectCurrentMapped)
    {
        quint8 size = static_cast<quint8>(QMetaType::sizeOf(object.dataType));
        if (size > 8)
        {
            setError(ERROR_EXCEED_PDO_LENGTH);
        }
        convertQVariantToQDataStream(request, _node->nodeOd()->value(object), _node->nodeOd()->dataType(object));
    }
    sendData();
}

/**
 * @brief Send data on bus
 */
bool RPDO::sendData()
{
    if (!bus()->canWrite())
    {
        return false;
    }

    QCanBusFrame frame;
    frame.setFrameId(_cobId);
    frame.setPayload(_rpdoDataToSendReqPayload);
    return bus()->writeFrame(frame);
}

/**
 * @brief Convert QVariant To QDataStream
 * @param request -> value converted
 * @param data value to convert
 * @param data QMetaType
 */
void RPDO::convertQVariantToQDataStream(QDataStream &request, const QVariant &data, QMetaType::Type type)
{
    switch (type)
    {
    case QMetaType::Long:
    case QMetaType::LongLong:
    case QMetaType::Int:
        request << data.value<int>();
        break;

    case QMetaType::ULong:
    case QMetaType::ULongLong:
    case QMetaType::UInt:
        request << data.value<unsigned int>();
        break;

    case QMetaType::Double:
        request << data.value<double>();
        break;

    case QMetaType::Short:
        request << data.value<short>();
        break;

    case QMetaType::Char:
        request << data.value<char>();
        break;

    case QMetaType::UShort:
        request << data.value<unsigned short>();
        break;

    case QMetaType::UChar:
        request << data.value<unsigned char>();
        break;

    case QMetaType::Float:
        request << data.value<float>();
        break;

    case QMetaType::SChar:
        request << data.value<signed char>();
        break;

    case QMetaType::QString:
        request << data;
        break;

    case QMetaType::QByteArray:
        request << data;
        break;

    default:
        break;
    }
}

bool RPDO::isTPDO() const
{
    return false;
}

bool RPDO::isRPDO() const
{
    return true;
}
