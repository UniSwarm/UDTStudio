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

#define PDO_INDEX_MASK 0xFFFF0000
#define PDO_SUBINDEX_MASK 0x0000FF00
#define PDO_DATASIZE_MASK 0x000000FF

RPDO::RPDO(Node *node, quint8 number)
    : Service(node)
    , _number(number)
{
    _nodeOd = node->nodeOd();
    _busId = node->busId();
    _nodeId = node->nodeId();

    _cobId = 0x200 + 0x100 * _number + node->nodeId();
    _cobIds.append(_cobId);
    _objectCommId = 0x1400 + _number;
    _objectMappingId = 0x1600 + _number;

    _rpdo = new RPDO_();

    NodeObjectId objectMapping(_nodeId, _busId, _objectMappingId, 0);

    registerIndex(_objectMappingId);
    node->readObject(objectMapping);
    createListObjectMapped();
}

QString RPDO::type() const
{
    return QLatin1String("RPDO") + QString::number(_number + 1, 10);
}

void RPDO::parseFrame(const QCanBusFrame &frame)
{
    Q_UNUSED(frame);
}

void RPDO::receiveSync()
{
    saveData();
    sendData(_rpdo->data);
}

quint8 RPDO::number() const
{
    return _number;
}

void RPDO::odNotify(const NodeObjectId &objId, const QVariant &value)
{
    if (objId.index == (_objectMappingId) && objId.subIndex == 0x00)
    {
        createListObjectMapped();
    }
}

void RPDO::saveData()
{
    for (QPair<NodeIndex *, quint8> pair : _objectMapped)
    {
        quint8 size = static_cast<quint8>(QMetaType::sizeOf(_nodeOd->dataType(pair.first->index(), pair.second)));
        if (size > 8)
        {
            // ERROR : CO_SDO_ABORT_CODE_EXCEED_PDO_LENGTH
            return;
        }
        arrangeData(_rpdo->data, _nodeOd->value(pair.first->index(), pair.second));
    }
}

bool RPDO::createListObjectMapped()
{
    _objectMapped.clear();
    NodeObjectId objectMapping(_nodeId, _busId, _objectMappingId, 0);
    quint8 numberEntries = static_cast<quint8>(_nodeOd->value(objectMapping).toUInt());

    for (quint8 i = 1; i <= numberEntries; i++)
    {
        NodeObjectId objectMapping(_nodeId, _busId, _objectMappingId, i);

        quint32 mapping = _nodeOd->value(objectMapping).toUInt();
        quint8 subIndexMapping = (mapping & PDO_SUBINDEX_MASK) >> 8;
        quint16 indexMapping = mapping >> 16;

        _objectMapped.append(qMakePair(_nodeOd->index(indexMapping), subIndexMapping));
    }
    return true;
}

bool RPDO::sendData(const QByteArray data)
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

void RPDO::arrangeData(QByteArray &dataByte, const QVariant &data)
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
