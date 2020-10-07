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
    if ((objId.index() == _objectCommId) && (objId.subIndex() == 0x01))
    {
        emit enabledChanged(isEnabled());
    }

    if (_statusPdo == STATE_NONE && objId.index() == _objectMappingId)
    {
        if (_objectCommList.size() != 0)
        {
            createListObjectMapped();
        }
    }

    if (_statusPdo == STATE_READ)
    {
        managementRespReadCommAndMapping(objId, flags);
    }

    if (_statusPdo == STATE_WRITE)
    {
        managementRespProcessMapping(objId, flags);
    }
}

void RPDO::setBus(CanOpenBus *bus)
{
    _bus = bus;
    connect(_bus->sync(), &Sync::signalBeforeSync, this, &RPDO::prepareAndSendData);
}

/**
 * @brief Set Transmission Type
 * @param type Sync:0x00->0xF0; event-driven (manufacturer-specific):0xFE; event-driven (device profile and application profile specific):0xFF
 * @return bool
 */
bool RPDO::setTransmissionType(quint8 type)
{
    _statusPdo = STATE_NONE;
    if ((type <= RPDO_SYNC_MAX) || (type == RPDO_EVENT_MS) || (type == RPDO_EVENT_DP))
    {
        _waitingConf.transType = type;
        _node->writeObject(_objectCommList[1].index(), PDO_COMM_TRANSMISSION_TYPE, _waitingConf.transType);
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

void RPDO::receiveSync()
{
    if ((_objectCurrentMapped.isEmpty()) || (!isEnabled()))
    {
        return;
    }

    // Update data of object in NodeOd after a sync
    for (NodeObjectId objectIterator : _objectCurrentMapped)
    {
        if (_dataObjectCurrentMapped.contains(objectIterator.key())
            && _node->nodeOd()->indexExist(objectIterator.index())
            && _node->nodeOd()->subIndexExist(objectIterator.index(), objectIterator.subIndex()))
        {
            _node->nodeOd()->index(objectIterator.index())->subIndex(objectIterator.subIndex())->setValue(_dataObjectCurrentMapped.value(objectIterator.key()));
        }
    }
}

/**
 * @brief Save data of object before send
 * @param index
 * @param subindex
 * @param data
 */
void RPDO::write(const NodeObjectId &object, const QVariant &data)
{
    if ((_objectCurrentMapped.isEmpty()) || (!isEnabled()))
    {
        return;
    }

    for (NodeObjectId objectIterator : _objectCurrentMapped)
    {
        if (objectIterator.index() == object.index() && objectIterator.subIndex() == object.subIndex())
        {
            if (_dataObjectCurrentMapped.contains(objectIterator.key()))
            {
                _dataObjectCurrentMapped.remove(objectIterator.key());
            }
            _dataObjectCurrentMapped.insert(object.key(), data);
            return;
        }
    }
}

/**
 * @brief Clear list data waiting
 */
void RPDO::clearDataWaiting()
{
    _dataObjectCurrentMapped.clear();
}

/**
 * @brief Prepares the data before the sync signal
 */
void RPDO::prepareAndSendData()
{
    if ((_objectCurrentMapped.isEmpty()) || (!isEnabled()) || _node->status() != Node::STARTED)
    {
        return;
    }

    QDataStream request(&_rpdoDataToSendReqPayload, QIODevice::WriteOnly);
    request.setByteOrder(QDataStream::LittleEndian);

    for (NodeObjectId objectIterator : _objectCurrentMapped)
    {
        quint8 size = static_cast<quint8>(QMetaType::sizeOf(objectIterator.dataType()));
        if (size > 8)
        {
            setError(ERROR_EXCEED_PDO_LENGTH);
        }
        if (_dataObjectCurrentMapped.contains(objectIterator.key()))
        {
           convertQVariantToQDataStream(request, _dataObjectCurrentMapped.value(objectIterator.key()), _node->nodeOd()->dataType(objectIterator));
        }
        else
        {
            convertQVariantToQDataStream(request, _node->nodeOd()->value(objectIterator), _node->nodeOd()->dataType(objectIterator));
        }
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
