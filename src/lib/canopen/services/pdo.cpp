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

#define PDO_INDEX_MASK 0xFFFF0000
#define PDO_SUBINDEX_MASK 0x0000FF00
#define PDO_DATASIZE_MASK 0x000000FF
#define COBID_VALID_VALID 0x00000000
#define COBID_VALID_NOT_VALID 0x80000000

PDO::PDO(Node *node, quint8 number)
    : Service(node)
    , _number(number)
{
    _nodeId = node->nodeId();
    _nodeOd = node->nodeOd();
}

bool PDO::createListObjectMapped()
{
    _objectMapped.clear();
    NodeObjectId objectMapping(_objectMappingId, 0);
    quint8 numberEntries = static_cast<quint8>(_nodeOd->value(objectMapping).toUInt());

    for (quint8 i = 1; i <= numberEntries; i++)
    {
        NodeObjectId objectMapping(_objectMappingId, i);

        quint32 mapping = _nodeOd->value(objectMapping).toUInt();
        quint8 subIndexMapping = (mapping & PDO_SUBINDEX_MASK) >> 8;
        quint16 indexMapping = mapping >> 16;
        NodeObjectId object(indexMapping, subIndexMapping);

        _objectMapped.append(object);
    }
    return true;
}

void PDO::mapObjectList(QList<NodeObjectId> objectList)
{
    _objectMap.clear();
    _objectMap = objectList;
}

QList<NodeObjectId> PDO::currentMappedObjectList() const
{
    return _objectMapped;
}

void PDO::applyMapping()
{
    if (_node->status() == Node::Status::STOPPED || _node->status() == Node::Status::INIT)
    {
        return;
    }
    if (_objectMap.isEmpty())
    {
        return;
    }

    switch (state)
    {
    case STATE_FREE:
        // Deactivate the PDO
        _cobId = _cobId & COBID_VALID_NOT_VALID;
        _node->writeObject(_objectCommId, 0x01, QVariant(_cobId));
        break;

    case STATE_DEACTIVATE:
        // Disable the mapping
        _node->writeObject(_objectMappingId, 0x00, QVariant(quint8(0)));
        break;

    case STATE_DISABLE:
    {
        // Modify the mapping
        QByteArray dataByte;
        arrangeData(dataByte, _objectMap.at(_numberObjectCurrent).index);
        arrangeData(dataByte, _objectMap.at(_numberObjectCurrent).subIndex);
        arrangeData(dataByte, QMetaType::sizeOf(_objectMap.at(_numberObjectCurrent).dataType));
        _node->writeObject(_objectMappingId, _numberObjectCurrent + 1, dataByte);
        break;
    }

    case STATE_MODIFY:
        // Enable the mapping
        _node->writeObject(_objectMappingId, 0x00, QVariant(quint8(_objectMap.size())));
        break;

    case STATE_ENABLE:
        // Activate the PDO
        _cobId = _cobId & ~COBID_VALID_NOT_VALID;
        _node->writeObject(_objectCommId, 0x01, QVariant(_cobId));
        break;

    case STATE_ACTIVATE:
        state = STATE_ENABLE;
        _objectMap.clear();
        _numberObjectCurrent = 0;
        createListObjectMapped();
        break;
    }

    // set transmission type
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
