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

#include "tpdo.h"

#include <QDebug>

#include "canopenbus.h"
#include "sdo.h"
#include <QDataStream>

TPDO::TPDO(Node *node, quint8 number)
    : PDO(node, number)
{
    _cobId = 0x180 + 0x100 * _number + node->nodeId();
    _cobIds.append(_cobId);
    _objectCommId = 0x1800 + _number;
    _objectMappingId = 0x1A00 + _number;

    registerObjId({_objectCommId, 255});
    registerObjId({_objectMappingId, 255});
    registerObjId({0x4000, 255});
    setNodeInterrest(node);

    _objectCommList = {{_objectCommId, 0x1}, {_objectCommId, 0x2}, {_objectCommId, 0x3}, {_objectCommId, 0x5}, {_objectCommId, 0x6}};
}

QString TPDO::type() const
{
    return QLatin1String("TPDO") + QString::number(_number + 1, 10);
}

void TPDO::parseFrame(const QCanBusFrame &frame)
{
    quint8 offset = 0;

    for (quint8 i = 0; i < _objectCurrentMapped.size(); i++)
    {
        QByteArray data = frame.payload().mid(offset, QMetaType::sizeOf(_objectCurrentMapped.at(i).dataType));
        QVariant vata = convertQByteArrayToQVariant(data, _objectCurrentMapped.at(i).dataType);

        _nodeOd->updateObjectFromDevice(_objectCurrentMapped.at(i).index, _objectCurrentMapped.at(i).subIndex, vata, SDO::FlagsRequest::Pdo);
        offset += QMetaType::sizeOf(_objectCurrentMapped.at(i).dataType);
    }
}

void TPDO::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
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

void TPDO::setBus(CanOpenBus *bus)
{
    _bus = bus;
    connect(_bus->sync(), &Sync::syncEmitted, this, &TPDO::receiveSync);
}

quint8 TPDO::number() const
{
    return _number;
}

void TPDO::setCommParam(PDO_conf &conf)
{
    if ((conf.transType <= TPDO_CYCLIC_MAX) || (conf.transType == TPDO_RTR_SYNC) || (conf.transType == TPDO_RTR_EVENT) || (conf.transType == TPDO_EVENT_MS) || (conf.transType == TPDO_EVENT_DP))
    {
        _waitingParam.transType = conf.transType;
    }
    else
    {
        _waitingParam.transType = 0;
    }

    _waitingParam.eventTimer = conf.eventTimer;
    _waitingParam.inhibitTime = conf.inhibitTime;
    _waitingParam.syncStartValue = conf.syncStartValue;
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
