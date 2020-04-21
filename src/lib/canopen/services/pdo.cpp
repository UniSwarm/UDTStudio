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

#include "pdo.h"
#include "canopenbus.h"
#include "services/services.h"
#include <QDataStream>
#include <QDebug>

#define PDO_INDEX_MASK 0xFFFF0000
#define PDO_SUBINDEX_MASK 0x0000FF00
#define PDO_DATASIZE_MASK 0x000000FF
#define COBID_MASK 0x7FFFFFFF
#define COBID_VALID_NOT_VALID 0x80000000

PDO::PDO(Node *node, quint8 number)
    : Service(node)
    , _pdoNumber(number)
{
    statusPdo = STATE_NONE;
    state = STATE_FREE;

    _iFsm = 0;

    _waitingConf.transType = 0;
    _waitingConf.eventTimer = 0;
    _waitingConf.inhibitTime = 0;
    _waitingConf.syncStartValue = 0;
}

quint32 PDO::cobId() const
{
    return _cobId;
}

quint8 PDO::pdoNumber() const
{
    return _pdoNumber;
}

const QList<NodeObjectId> &PDO::currentMappind() const
{
    return _objectCurrentMapped;
}

bool PDO::hasMappedObject() const
{
    return !_objectCurrentMapped.isEmpty();
}

bool PDO::isMappedObject(NodeObjectId object) const
{
    for (NodeObjectId objectIterator : _objectCurrentMapped)
    {
        if ((objectIterator.index == object.index) && (objectIterator.subIndex == object.subIndex))
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Start to read all settings (Comm and Mapping) from device
 */
void PDO::readMapping()
{
    statusPdo = STATE_READ;
    readCommParam();
}

void PDO::writeMapping(const QList<NodeObjectId> &objectList)
{
    quint8 size = 0;
    for (NodeObjectId objectId : objectList)
    {
        size += objectId.bitSize();
        if (size > 64)
        {
            setError(ERROR_EXCEED_PDO_LENGTH);
            return;
        }
    }

    for (NodeObjectId objectId : objectList)
    {
        if (objectId.dataType == QMetaType::Type::UnknownType)
        {
            objectId.dataType = _node->nodeOd()->dataType(objectId.index, objectId.subIndex);
        }
    }

    _objectToMap = objectList;
    statusPdo = STATE_WRITE;
    state = STATE_FREE;
    processMapping();
}

bool PDO::isEnabled() const
{
    NodeObjectId object(_objectCommId, PDO_COMM_COB_ID);
    quint32 cobIb = _node->nodeOd()->value(object).toUInt();

    if ((cobIb & COBID_VALID_NOT_VALID) == COBID_VALID_NOT_VALID)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void PDO::setEnabled(bool enabled)
{
    statusPdo = STATE_NONE;
    NodeObjectId object(_objectCommId, PDO_COMM_COB_ID);
    if (enabled)
    {
        _node->writeObject(object.index, object.subIndex, QVariant(_cobId & COBID_MASK));
    }
    else
    {
        _node->writeObject(object.index, object.subIndex, QVariant(_cobId | COBID_VALID_NOT_VALID));
    }
}

quint32 PDO::inhibitTime() const
{
    NodeObjectId object(_objectCommId, PDO_COMM_INHIBIT_TIME);
    return _node->nodeOd()->value(object).toUInt();
}

void PDO::setInhibitTime(quint32 inhibitTime)
{
    statusPdo = STATE_NONE;
    _waitingConf.inhibitTime = inhibitTime;
    _node->writeObject(_objectCommList[2].index, PDO_COMM_INHIBIT_TIME, _waitingConf.inhibitTime);
}

quint32 PDO::eventTimer() const
{
    NodeObjectId object(_objectCommId, PDO_COMM_EVENT_TIMER);
    return _node->nodeOd()->value(object).toUInt();
}

void PDO::setEventTimer(quint32 eventTimer)
{
    statusPdo = STATE_NONE;
    _waitingConf.eventTimer = eventTimer;
    _node->writeObject(_objectCommList[3].index, PDO_COMM_EVENT_TIMER, _waitingConf.eventTimer);
}

/**
 * @brief management response from device after setInhibitTime, setEventTimer, setTransmissionType and setSyncStartValue
 */
void PDO::notifyWriteParam(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (flags == SDO::FlagsRequest::Error)
    {
        if (objId.subIndex == PDO_COMM_TRASMISSION_TYPE)
        {
            _waitingConf.transType = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
        }
        else if (objId.subIndex == PDO_COMM_INHIBIT_TIME)
        {
            _waitingConf.inhibitTime = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
        }
        else if (objId.subIndex == PDO_COMM_EVENT_TIMER)
        {
            _waitingConf.eventTimer = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
        }
        else if (objId.subIndex == PDO_COMM_SYNC_START_VALUE)
        {
            _waitingConf.syncStartValue = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
        }

        qDebug() << ">PDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex << ", Error : " << _node->nodeOd()->errorObject(objId);
        setError(ERROR_WRITE_PARAM);
        return;
    }
}

/**
 * @brief management response from device after readMappingFromDevice, readCommParam and readMappingParam
 */
void PDO::notifyReadPdo(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId.index == _objectCommId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            qDebug() << ">PDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex << ", Error : " << _node->nodeOd()->errorObject(objId);
            setError(ERROR_GENERAL_ERROR);
            _iFsm++;
        }
        else
        {
            if (objId.subIndex == PDO_COMM_TRASMISSION_TYPE)
            {
                _waitingConf.transType = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
            }
            else if (objId.subIndex == PDO_COMM_INHIBIT_TIME)
            {
                _waitingConf.inhibitTime = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
            }
            else if (objId.subIndex == PDO_COMM_EVENT_TIMER)
            {
                _waitingConf.eventTimer = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
            }
            else if (objId.subIndex == PDO_COMM_SYNC_START_VALUE)
            {
                _waitingConf.syncStartValue = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
            }
        }
        _iFsm++;
        if (_iFsm >= _objectCommList.size())
        {
            _iFsm = 0;
            readMappingParam();
            return;
        }
        readCommParam();
    }

    if (objId.index == _objectMappingId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            qDebug() << ">PDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex << ", Error : " << _node->nodeOd()->errorObject(objId);
            setError(ERROR_GENERAL_ERROR);
            state = STATE_FREE;
            return;
        }

        NodeObjectId objectComm(_node->busId(), _node->nodeId(), _objectMappingId, 0);
        if (_iFsm > static_cast<quint8>(_node->nodeOd()->value(objectComm).toUInt()))
        {
            _iFsm = 0;
            statusPdo = STATE_NONE;
            state = STATE_FREE;
            createListObjectMapped();
            return;
        }
        readMappingParam();
    }
}

/**
 * @brief Read Comm Param from device
 */
void PDO::readCommParam()
{
    _node->readObject(_objectCommList[_iFsm]);
}

/**
 * @brief Read Mapping Param from device
 */
void PDO::readMappingParam()
{
    NodeObjectId objectId(_node->busId(), _node->nodeId(), _objectMappingId, _iFsm);
    _node->readObject(objectId);
    _iFsm++;
}

bool PDO::createListObjectMapped()
{
    if ((_node->nodeOd()->value(_objectCommList[0]).toUInt() & COBID_VALID_NOT_VALID) == COBID_VALID_NOT_VALID)
    {
        setError(ERROR_COBID_NOT_VALID);
        return false;
    }
    _objectCurrentMapped.clear();
    NodeObjectId objectMapping(_objectMappingId, 0);
    quint8 numberEntries = static_cast<quint8>(_node->nodeOd()->value(objectMapping).toUInt());

    for (quint8 i = 1; i <= numberEntries; i++)
    {
        NodeObjectId objectId(_node->busId(), _node->nodeId(), _objectMappingId, i);

        quint32 mapping = _node->nodeOd()->value(objectId).toUInt();
        quint8 subIndexMapping = (mapping & PDO_SUBINDEX_MASK) >> 8;
        quint16 indexMapping = mapping >> 16;

        NodeObjectId object(_node->busId(), _node->nodeId(), indexMapping, subIndexMapping, _node->nodeOd()->dataType(indexMapping, subIndexMapping));

        _objectCurrentMapped.append(object);
    }
    emit mappingChanged();
    return true;
}

/**
 * @brief management response from device after processMapping
 */
void PDO::notifyWritePdo(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((objId.index == _objectCommId) && (state == STATE_FREE))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            setError(ERROR_DEACTIVATE_COBID);
            statusPdo = STATE_NONE;
            state = STATE_FREE;
            return;
        }
        state = STATE_DEACTIVATE;
        processMapping();
    }
    if ((objId.index == _objectMappingId) && (objId.subIndex == 0x00) && (state == STATE_DEACTIVATE))
    {
        state = STATE_DISABLE;
        _iFsm = 0;
        processMapping();
    }
    if ((objId.index == _objectMappingId) && (objId.subIndex != 0x00) && (state == STATE_DISABLE))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            // ERROR so cobId is invalid and mapping is disable
            qDebug() << ">TPDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex << ", Error : " << _node->nodeOd()->errorObject(objId);
            setError(ERROR_MODIFY_MAPPING);
            state = STATE_FREE;
            return;
        }
        _iFsm++;
        if (_iFsm == _objectToMap.size())
        {
            state = STATE_MODIFY;
        }
        processMapping();
    }
    if ((objId.index == _objectMappingId) && (objId.subIndex == 0x00) && (state == STATE_MODIFY))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            // ERROR so cobId is invalid and mapping is disable
            qDebug() << ">TPDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex << ", Error : " << _node->nodeOd()->errorObject(objId);
            setError(ERROR_GENERAL_ERROR);
            state = STATE_FREE;
            return;
        }
        state = STATE_ENABLE;
        processMapping();
    }
    if ((objId.index == _objectCommId) && objId.subIndex == 0x01 && (state == STATE_ENABLE))
    {
        state = STATE_ACTIVATE;
        processMapping();
    }
}

void PDO::processMapping()
{
    if (_node->status() == Node::Status::STOPPED || _node->status() == Node::Status::INIT)
    {
        return;
    }
    if (_objectToMap.isEmpty())
    {
        return;
    }

    switch (state)
    {
    case STATE_FREE:
    {
        // Deactivate the PDO
        _node->writeObject(_objectCommList[0].index, _objectCommList[0].subIndex, QVariant(_cobId | COBID_VALID_NOT_VALID));
        break;
    }
    case STATE_DEACTIVATE:
        // Disable the mapping
        if (((_node->nodeOd()->value(_objectCommList[0]).toUInt() & COBID_VALID_NOT_VALID) == COBID_VALID_NOT_VALID))
        {
            _node->writeObject(_objectMappingId, 0x00, QVariant(quint8(0)));
        }
        else
        {
            setError(ERROR_DISABLE_MAPPING);
            state = STATE_FREE;
        }
        break;

    case STATE_DISABLE:
    {
        // Modify the mapping
        quint32 map = 0;
        map = static_cast<quint32>(_objectToMap.at(_iFsm).index << 16);
        map = map | (static_cast<quint16>(_objectToMap.at(_iFsm).subIndex << 8));
        map = map | _objectToMap.at(_iFsm).bitSize();
        _node->writeObject(_objectMappingId, _iFsm + 1, map);
        break;
    }

    case STATE_MODIFY:
        // Enable the mapping
        _node->writeObject(_objectMappingId, 0x00, QVariant(quint8(_objectToMap.size())));
        break;

    case STATE_ENABLE:
        // Activate the PDO
        _cobId = _cobId & ~COBID_VALID_NOT_VALID;
        _node->writeObject(_objectCommId, 0x01, QVariant(_cobId));
        break;

    case STATE_ACTIVATE:
        statusPdo = STATE_NONE;
        state = STATE_FREE;
        _objectToMap.clear();
        _iFsm = 0;
        createListObjectMapped();
        break;
    }
}

bool PDO::sendData()
{
    QCanBusDevice *lcanDevice = canDevice();
    if (!lcanDevice)
    {
        return false;
    }

    QByteArray pdoWriteReqPayload;
    QDataStream request(&pdoWriteReqPayload, QIODevice::WriteOnly);
    request.setByteOrder(QDataStream::LittleEndian);

    for (NodeObjectId object : _objectCurrentMapped)
    {
        quint8 size = static_cast<quint8>(QMetaType::sizeOf(object.dataType));
        if (size > 8)
        {
            setError(ERROR_EXCEED_PDO_LENGTH);
            return false;
        }
        convertQVariantToQDataStream(request, _node->nodeOd()->value(object), _node->nodeOd()->dataType(object));
    }

    QCanBusFrame frame;
    frame.setFrameId(_cobId);
    frame.setPayload(pdoWriteReqPayload);
    lcanDevice->writeFrame(frame);
    return lcanDevice->writeFrame(frame);
}

void PDO::convertQVariantToQDataStream(QDataStream &request, const QVariant &data, QMetaType::Type type)
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

void PDO::setError(ErrorPdo error)
{
    emit errorOccurred(error);
}
