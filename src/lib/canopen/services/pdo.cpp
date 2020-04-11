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
#define COBID_VALID_NOT_VALID 0x80000000

PDO::PDO(Node *node, quint8 number)
    : Service(node), _pdoNumber(number)
{
    statusPdo = STATE_NONE;
    state = STATE_FREE;

    _iFsm = 0;

    _waitingConf.transType = 0;
    _waitingConf.eventTimer = 0;
    _waitingConf.inhibitTime = 0;
    _waitingConf.syncStartValue = 0;
}

quint32 PDO::cobId()
{
    return _cobId;
}

quint8 PDO::pdoNumber()
{
    return _pdoNumber;
}

void  PDO::setInhibitTime(quint32 inhibitTime)
{
    _waitingConf.inhibitTime = inhibitTime;
    _node->writeObject(_objectCommList[2].index, PDO_COMM_INHIBIT_TIME, _waitingConf.inhibitTime);
}

void  PDO::setEventTimer(quint32 eventTimer)
{
    _waitingConf.eventTimer = eventTimer;
    _node->writeObject(_objectCommList[3].index, PDO_COMM_EVENT_TIMER, _waitingConf.eventTimer);
}

void PDO::notifyReadPdo(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId.index == _objectCommId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            qDebug() << ">PDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex
                     << ", Error : " << _node->nodeOd()->errorObject(objId);
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
                _waitingConf.eventTimer = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
            }
            else if (objId.subIndex == PDO_COMM_EVENT_TIMER)
            {
                _waitingConf.inhibitTime = static_cast<quint8>(_node->nodeOd()->value(objId).toUInt());
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
            qDebug() << ">PDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex
                     << ", Error : " << _node->nodeOd()->errorObject(objId);
            state = STATE_FREE;
            return;
        }

        NodeObjectId objectComm(_node->busId(), _node->nodeId(), _objectMappingId, 0);
        if (_iFsm > static_cast<quint8>(_node->nodeOd()->value(objectComm).toUInt()))
        {
            _iFsm = 0;
            state = STATE_FREE;
            createListObjectMapped();
            return;
        }
        readMappingParam();
    }
}

void PDO::readMappingFromDevice()
{
    statusPdo = STATE_READ;
    readCommParam();
}
void PDO::readCommParam()
{
    _node->readObject(_objectCommList[_iFsm]);
}

void PDO::readMappingParam()
{
    NodeObjectId objectId(_node->busId(), _node->nodeId(), _objectMappingId, _iFsm);
    _node->readObject(objectId);
    _iFsm++;
}

bool PDO::createListObjectMapped()
{
    if (_node->nodeOd()->value(_objectCommList[0]) == (_cobId | COBID_VALID_NOT_VALID))
    {
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

const QList<NodeObjectId> &PDO::currentMappind() const
{
    return _objectCurrentMapped;
}

void PDO::notifyWritePdo(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((objId.index == _objectCommId) && (state == STATE_FREE))
    {
        _iFsm++;

        if (!_node->nodeOd()->index(_objectCommList[_iFsm].index)->subIndexExist(_objectCommList[_iFsm].subIndex))
        {
            _iFsm++;
        }
        if (_iFsm >= _objectCommList.size())
        {
            state = STATE_DEACTIVATE;
            _iFsm = 0;
            processMapping();
            return;
        }
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
            qDebug() << ">TPDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex
                     << ", Error : " << _node->nodeOd()->errorObject(objId);
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
            qDebug() << ">TPDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex
                     << ", Error : " << _node->nodeOd()->errorObject(objId);
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

void PDO::applyMapping(const QList<NodeObjectId> &objectList)
{
    _objectToMap = objectList;
    statusPdo = STATE_WRITE;
    state = STATE_FREE;
    processMapping();
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
        quint8 subindex = _objectCommList[_iFsm].subIndex;
        if ((subindex) == PDO_COMM_COB_ID)
        {
            // Deactivate the PDO
            _node->writeObject(_objectCommList[_iFsm].index, subindex, QVariant(_cobId | COBID_VALID_NOT_VALID));
        }
        else if ((subindex) == PDO_COMM_TRASMISSION_TYPE)
        {
            _node->writeObject(_objectCommList[_iFsm].index, subindex, _waitingConf.transType);
        }
        else if ((subindex) == PDO_COMM_INHIBIT_TIME)
        {
            _node->writeObject(_objectCommList[_iFsm].index, subindex, _waitingConf.inhibitTime);
        }
        else if ((subindex) == PDO_COMM_EVENT_TIMER)
        {
            _node->writeObject(_objectCommList[_iFsm].index, subindex, _waitingConf.eventTimer);
        }
        else if ((subindex) == PDO_COMM_SYNC_START_VALUE)
        {
            _node->writeObject(_objectCommList[_iFsm].index, subindex, _waitingConf.syncStartValue);
        }

        break;
    }
    case STATE_DEACTIVATE:
        // Disable the mapping
        if (_node->nodeOd()->value(_objectCommList[0]) == (_cobId | COBID_VALID_NOT_VALID))
        {
            _node->writeObject(_objectMappingId, 0x00, QVariant(quint8(0)));
        }
        else
        {
            state = STATE_FREE;
        }
        break;

    case STATE_DISABLE:
    {
        // Modify the mapping
        quint32 map = 0;
        map = static_cast<quint32>(_objectToMap.at(_iFsm).index << 16);
        map = map | (static_cast<quint16>(_objectToMap.at(_iFsm).subIndex << 8));
        map = map | static_cast<quint16>(QMetaType::sizeOf(_objectToMap.at(_iFsm).dataType) * 8);
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

    QDataStream request(&_data, QIODevice::WriteOnly);

    for (NodeObjectId object : _objectCurrentMapped)
    {
        quint8 size = static_cast<quint8>(QMetaType::sizeOf(object.dataType));
        if (size > 8)
        {
            // ERROR : CO_SDO_ABORT_CODE_EXCEED_PDO_LENGTH
            return false;
        }
        convertQVariantToQDataStream(request, _node->nodeOd()->value(object.index, object.subIndex));
    }

    QCanBusFrame frame;
    frame.setFrameId(_cobId);
    frame.setPayload(_data);
    lcanDevice->writeFrame(frame);
    return lcanDevice->writeFrame(frame);
}

void PDO::convertQVariantToQDataStream(QDataStream &request, const QVariant &data)
{
    qint16 value = static_cast<qint16>(data.toInt());
    request << value;
    switch (QMetaType::Type(data.type()))
    {
    case QMetaType::Long:
    case QMetaType::LongLong:
    case QMetaType::Int:
    {
        qint32 value = static_cast<qint32>(data.toInt());
        request << value;
        break;
    }
    case QMetaType::ULong:
    case QMetaType::ULongLong:
    case QMetaType::UInt:
    {
        quint32 value = static_cast<quint32>(data.toInt());
        request << value;
        break;
    }

    case QMetaType::Double:
        break;

    case QMetaType::Short:
    {
        qint16 value = static_cast<qint16>(data.toInt());
        request << value;
        break;
    }
    case QMetaType::Char:
        break;

    case QMetaType::UShort:
    {
        quint16 value = static_cast<quint16>(data.toInt());
        request << value;
        break;
    }

    case QMetaType::UChar:
        break;

    case QMetaType::Float:
        break;

    case QMetaType::SChar:
        break;

    case QMetaType::QString:
        break;

    case QMetaType::QByteArray:
        break;

    default:
        break;
    }
}
