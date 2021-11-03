/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
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

#include "tpdo.h"

#include <QDebug>

#include "canopenbus.h"
#include "sdo.h"
#include <QDataStream>

TPDO::TPDO(Node *node, quint8 number)
    : PDO(node, number)
{
    _cobId = 0x180 + 0x100 * _pdoNumber + node->nodeId();
    _cobIds.append(_cobId);
    _objectCommId = 0x1800 + _pdoNumber;
    _objectMappingId = 0x1A00 + _pdoNumber;

    registerObjId({_objectCommId, 255});
    registerObjId({_objectMappingId, 255});
    setNodeInterrest(node);

    _objectCommList = {{_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_NUMBER},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_COB_ID},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_TRANSMISSION_TYPE},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_INHIBIT_TIME},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_EVENT_TIMER},
                       {_node->busId(), _node->nodeId(), _objectCommId, PDO_COMM_SYNC_START_VALUE}};
}

QString TPDO::type() const
{
    return QLatin1String("TPDO") + QString::number(_pdoNumber + 1, 10);
}

void TPDO::parseFrame(const QCanBusFrame &frame)
{
    if (_objectCurrentMapped.isEmpty())
    {
        return;
    }
    quint8 offset = 0;

    for (quint8 i = 0; i < _objectCurrentMapped.size(); i++)
    {
        QByteArray data = frame.payload().mid(offset, QMetaType::sizeOf(_objectCurrentMapped.at(i).dataType()));
        QVariant vata = convertQByteArrayToQVariant(data, _objectCurrentMapped.at(i).dataType());

        _node->nodeOd()->updateObjectFromDevice(_objectCurrentMapped.at(i).index(), _objectCurrentMapped.at(i).subIndex(), vata, SDO::FlagsRequest::Pdo);
        offset += QMetaType::sizeOf(_objectCurrentMapped.at(i).dataType());
    }
}

void TPDO::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
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

void TPDO::setBus(CanOpenBus *bus)
{
    _bus = bus;
    connect(_bus->sync(), &Sync::syncEmitted, this, &TPDO::receiveSync);
}

bool TPDO::isTPDO() const
{
    return true;
}

bool TPDO::isRPDO() const
{
    return false;
}

bool TPDO::setTransmissionType(quint8 type)
{
    _statusPdo = STATE_NONE;
    if ((type <= TPDO_CYCLIC_MAX) || (type == TPDO_RTR_SYNC) || (type == TPDO_RTR_EVENT) || (type == TPDO_EVENT_MS) || (type == TPDO_EVENT_DP))
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

quint8 TPDO::transmissionType()
{
    NodeObjectId object(_objectCommId, PDO_COMM_TRANSMISSION_TYPE);
    return static_cast<quint8>(_node->nodeOd()->value(object).toUInt());
}

void TPDO::setSyncStartValue(quint8 syncStartValue)
{
    _statusPdo = STATE_NONE;
    _waitingConf.syncStartValue = syncStartValue;
    _node->writeObject(_objectCommList[4].index(), PDO_COMM_SYNC_START_VALUE, _waitingConf.syncStartValue);
}

quint8 TPDO::syncStartValue()
{
    NodeObjectId object(_objectCommId, PDO_COMM_SYNC_START_VALUE);
    return static_cast<quint8>(_node->nodeOd()->value(object).toUInt());
}

void TPDO::receiveSync()
{
}

QVariant TPDO::convertQByteArrayToQVariant(QByteArray data, QMetaType::Type type)
{
    QDataStream dataStream(&data, QIODevice::ReadOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    switch (type)
    {
        case QMetaType::Int:
            int a;
            dataStream >> a;
            return QVariant(a);

        case QMetaType::UInt:
            unsigned int b;
            dataStream >> b;
            return QVariant(b);

        case QMetaType::LongLong:
            qint64 c;
            dataStream >> c;
            return QVariant(c);

        case QMetaType::ULongLong:
            quint64 d;
            dataStream >> d;
            return QVariant(d);

        case QMetaType::Double:
            double e;
            dataStream >> e;
            return QVariant(e);

        case QMetaType::Long:
        {
            long f;
            memcpy(&f, data.constData(), sizeof(long));
            QVariant w;
            w.setValue(f);
            return w;
        }

        case QMetaType::Short:
            short g;
            dataStream >> g;
            return QVariant(g);

        case QMetaType::Char:
            return QVariant(data);

        case QMetaType::ULong:
        {
            unsigned long i;
            memcpy(&i, data.constData(), sizeof(unsigned long));
            QVariant y;
            y.setValue(i);
            return y;
        }

        case QMetaType::UShort:
        {
            unsigned short j;
            memcpy(&j, data.constData(), sizeof(unsigned short));
            QVariant z;
            z.setValue(j);
            return z;
        }

        case QMetaType::UChar:
        {
            unsigned char k;
            memcpy(&k, data.constData(), sizeof(unsigned char));
            QVariant n;
            n.setValue(k);
            return n;
        }

        case QMetaType::Float:
            float l;
            dataStream >> l;
            return QVariant(l);

        case QMetaType::SChar:
        {
            signed char m;
            memcpy(&m, data.constData(), sizeof(signed char));
            QVariant x;
            x.setValue(m);
            return x;
        }

        case QMetaType::QString:
            return QVariant(QString(data));

        case QMetaType::QByteArray:
            return QVariant(data);

        default:
            break;
    }

    return QVariant();
}
