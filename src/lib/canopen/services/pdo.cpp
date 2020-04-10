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
    : Service(node)
    , _number(number)
{
    _nodeId = node->nodeId();
    _nodeOd = node->nodeOd();

    statusPdo = STATE_NONE;
    state = STATE_FREE;

    _increment = 0;

    _waitingConf.transType = 0;
    _waitingConf.eventTimer = 0;
    _waitingConf.inhibitTime = 0;
    _waitingConf.syncStartValue = 0;
}

void PDO::notifyReadPdo(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId.index == _objectCommId)
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            qDebug() << ">PDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex
                     << ", Error : " << _nodeOd->errorObject(objId);
            _increment++;
        }
        else
        {
            if (objId.subIndex == 2)
            {
                _waitingConf.transType = static_cast<quint8>(_nodeOd->value(objId).toUInt());
            }
            else if (objId.subIndex == 3)
            {
                _waitingConf.eventTimer = static_cast<quint8>(_nodeOd->value(objId).toUInt());
            }
            else if (objId.subIndex == 5)
            {
                _waitingConf.inhibitTime = static_cast<quint8>(_nodeOd->value(objId).toUInt());
            }
            else if (objId.subIndex == 6)
            {
                _waitingConf.syncStartValue = static_cast<quint8>(_nodeOd->value(objId).toUInt());
            }
        }
        _increment++;
        if (_increment >= _objectCommList.size())
        {
            _increment = 0;
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
                     << ", Error : " << _nodeOd->errorObject(objId);
            state = STATE_FREE;
            return;
        }

        NodeObjectId objectComm(_objectMappingId, 0);
        if (_increment > static_cast<quint8>(_nodeOd->value(objectComm).toUInt()))
        {
            _increment = 0;
            state = STATE_FREE;
            createListObjectMapped();
            return;
        }
        readMappingParam();
    }
}

void PDO::refreshPdo()
{
    statusPdo = STATE_READ;
    readCommParam();
}
void PDO::readCommParam()
{
    _node->readObject(_objectCommList[_increment]);
}

void PDO::readMappingParam()
{
    NodeObjectId objectId(_objectMappingId, _increment);
    _node->readObject(objectId);
    _increment++;
}

bool PDO::createListObjectMapped()
{
    if (_nodeOd->value(_objectCommList[0]) == (_cobId | COBID_VALID_NOT_VALID))
    {
        return false;
    }
    _objectCurrentMapped.clear();
    NodeObjectId objectMapping(_objectMappingId, 0);
    quint8 numberEntries = static_cast<quint8>(_nodeOd->value(objectMapping).toUInt());

    for (quint8 i = 1; i <= numberEntries; i++)
    {
        NodeObjectId objectId(_objectMappingId, i);

        quint32 mapping = _nodeOd->value(objectId).toUInt();
        quint8 subIndexMapping = (mapping & PDO_SUBINDEX_MASK) >> 8;
        quint16 indexMapping = mapping >> 16;

        NodeObjectId object(indexMapping, subIndexMapping, _nodeOd->dataType(indexMapping, subIndexMapping));

        _objectCurrentMapped.append(object);
    }
    return true;
}

void PDO::mapObjectList(QList<NodeObjectId> objectList)
{
    _objectToMap.clear();
    _objectToMap = objectList;
}

QList<NodeObjectId> PDO::currentMappedObjectList() const
{
    return _objectCurrentMapped;
}

void PDO::notifyWritePdo(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((objId.index == _objectCommId) && (state == STATE_FREE))
    {
        _increment++;
        if (_increment >= _objectCommList.size())
        {
            state = STATE_DEACTIVATE;
            _increment = 0;
            _increment = 0;
            processMapping();
            return;
        }
        if (!_nodeOd->index(_objectCommList[_increment].index)->subIndexExist(_objectCommList[_increment].subIndex))
        {
            _increment++;
        }
        processMapping();
    }
    if ((objId.index == _objectMappingId) && (objId.subIndex == 0x00) && (state == STATE_DEACTIVATE))
    {
        state = STATE_DISABLE;
        _increment = 0;
        processMapping();
    }
    if ((objId.index == _objectMappingId) && (objId.subIndex != 0x00) && (state == STATE_DISABLE))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            // ERROR so cobId is invalid and mapping is disable
            qDebug() << ">TPDO::odNotify : Index:SubIndex" << QString("0x%1").arg(QString::number(objId.index, 16)) << ":" << objId.subIndex
                     << ", Error : " << _nodeOd->errorObject(objId);
            state = STATE_FREE;
            return;
        }
        _increment++;
        if (_increment == _objectToMap.size())
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
                     << ", Error : " << _nodeOd->errorObject(objId);
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

void PDO::applyMapping()
{
    statusPdo = STATE_WRITE;
    state = STATE_FREE;
    _objectToMap = _objectCurrentMapped;
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
        quint8 subindex = _objectCommList[_increment].subIndex;
        if ((subindex) == PDO_COMM_COB_ID)
        {
            // Deactivate the PDO
            _node->writeObject(_objectCommList[_increment].index, subindex, QVariant(_cobId | COBID_VALID_NOT_VALID));
        }
        else if ((subindex) == PDO_COMM_TRASMISSION_TYPE)
        {
            _node->writeObject(_objectCommList[_increment].index, subindex, _waitingConf.transType);
        }
        else if ((subindex) == PDO_COMM_INHIBIT_TIME)
        {
            _node->writeObject(_objectCommList[_increment].index, subindex, _waitingConf.inhibitTime);
        }
        else if ((subindex) == PDO_COMM_EVENT_TIMER)
        {
            _node->writeObject(_objectCommList[_increment].index, subindex, _waitingConf.eventTimer);
        }
        else if ((subindex) == PDO_COMM_SYNC_START_VALUE)
        {
            _node->writeObject(_objectCommList[_increment].index, subindex, _waitingConf.syncStartValue);
        }

        break;
    }
    case STATE_DEACTIVATE:
        // Disable the mapping
        if (_nodeOd->value(_objectCommList[0]) == (_cobId | COBID_VALID_NOT_VALID))
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
        quint32 map = static_cast<quint32>(_objectToMap.at(_increment).index << 16);
        map = map | (static_cast<quint16>(_objectToMap.at(_increment).subIndex << 8));
        map = map | static_cast<quint16>(QMetaType::sizeOf(_objectToMap.at(_increment).dataType) * 8);
        _node->writeObject(_objectMappingId, _increment + 1, map);
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
        _increment = 0;
        createListObjectMapped();
        break;
    }
}

bool PDO::sendData(const QByteArray data)
{
    QCanBusDevice *lcanDevice = canDevice();
    if (!lcanDevice)
    {
        return false;
    }

    QByteArray sdoWriteReqPayload;
    QDataStream request(&sdoWriteReqPayload, QIODevice::WriteOnly);
    QCanBusFrame frame;
    request.setByteOrder(QDataStream::LittleEndian);
    request << data;

    frame.setFrameId(_cobId);
    frame.setPayload(sdoWriteReqPayload);

    return lcanDevice->writeFrame(frame);
}

void PDO::arrangeData(QByteArray &dataByte, const QVariant &data)
{
    switch (QMetaType::Type(data.type()))
    {
    case QMetaType::Long:
    case QMetaType::LongLong:
    case QMetaType::Int:
        dataByte.append(QByteArray::number(data.value<int>()));
        break;

    case QMetaType::ULong:
    case QMetaType::ULongLong:
    case QMetaType::UInt:
        dataByte.append(QByteArray::number(data.value<unsigned int>()));
        break;

    case QMetaType::Double:
        dataByte.append(QByteArray::number(data.value<double>()));
        break;

    case QMetaType::Short:
        dataByte.append(QByteArray::number(data.value<short>()));
        break;

    case QMetaType::Char:
        dataByte.append(QByteArray::number(data.value<char>()));
        break;

    case QMetaType::UShort:
        dataByte.append(QByteArray::number(data.value<unsigned short>()));
        break;

    case QMetaType::UChar:
        dataByte.append(QByteArray::number(data.value<unsigned char>()));
        break;

    case QMetaType::Float:
        dataByte.append(QByteArray::number(data.value<float>()));
        break;

    case QMetaType::SChar:
        dataByte.append(QByteArray::number(data.value<signed char>()));
        break;

    case QMetaType::QString:
        dataByte.append(data.toByteArray());
        break;

    case QMetaType::QByteArray:
        dataByte.append(data.toByteArray());
        break;

    default:
        break;
    }
}
