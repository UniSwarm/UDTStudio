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
    state = STATE_FREE;

    registerObjId({_objectCommId, 0x01});
    registerObjId({_objectMappingId, 255});
    setNodeInterrest(node);

    createListObjectMapped();

    // connect(node->bus()->sync(), &Sync::syncEmitted, this, &TPDO::receiveSync);
}

QString TPDO::type() const
{
    return QLatin1String("TPDO") + QString::number(_number + 1, 10);
}

void TPDO::parseFrame(const QCanBusFrame &frame)
{
    quint8 offset = 0;

    for (quint8 i = 0; i < _objectMapped.size(); i++)
    {
        QByteArray data = frame.payload().mid(offset, QMetaType::sizeOf(_objectMapped.at(i).dataType));

        QVariant vata = arrangeData(data, _objectMapped.at(i).dataType);
        _nodeOd->index(_objectMapped.at(i).index)->subIndex(_objectMapped.at(i).subIndex)->setValue(vata);

        offset += QMetaType::sizeOf(_objectMapped.at(i).dataType);
    }
}

void TPDO::odNotify(const NodeObjectId &objId, const QVariant &value)
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
        if (value == SDO::FlagsRequest::Error)
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
        if (value == SDO::FlagsRequest::Error)
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

quint8 TPDO::number() const
{
    return _number;
}

void TPDO::receiveSync()
{
}

QVariant TPDO::arrangeData(QByteArray data, QMetaType::Type type)
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
