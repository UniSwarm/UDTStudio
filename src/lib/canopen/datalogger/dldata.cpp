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

#include "dldata.h"

#include "canopen.h"

DLData::DLData(const NodeObjectId &objectId)
    :_objectId(objectId)
{
    _node = nullptr;
    CanOpenBus *bus = CanOpen::bus(objectId.busId());
    if (bus)
    {
        _node = bus->node(objectId.nodeId());
        NodeSubIndex *nodeSubIndex = objectId.nodeSubIndex();
        if (nodeSubIndex)
        {
            _name = nodeSubIndex->name();
        }
    }

    resetMinMax();
}

const NodeObjectId &DLData::objectId() const
{
    return _objectId;
}

quint64 DLData::key() const
{
    return _objectId.key();
}

Node *DLData::node() const
{
    return _node;
}

QString DLData::name() const
{
    return _name;
}

void DLData::setName(const QString &name)
{
    _name = name;
}

QColor DLData::color() const
{
    return _color;
}

void DLData::setColor(const QColor &color)
{
    _color = color;
}

double DLData::firstValue() const
{
    if (_values.isEmpty())
    {
        return 0.0;
    }
    return _values.first();
}

double DLData::lastValue() const
{
    if (_values.isEmpty())
    {
        return 0.0;
    }
    return _values.last();
}

QDateTime DLData::firstDateTime() const
{
    if (_values.isEmpty())
    {
        return QDateTime();
    }
    return _times.first();
}

QDateTime DLData::lastDateTime() const
{
    if (_values.isEmpty())
    {
        return QDateTime();
    }
    return _times.last();
}

const QList<qreal> &DLData::values() const
{
    return _values;
}

const QList<QDateTime> &DLData::times() const
{
    return _times;
}

void DLData::appendData(qreal value, const QDateTime &dateTime)
{
    _values.append(value);
    _times.append(dateTime);

    _min = qMin(_min, value);
    _max = qMax(_max, value);
}

void DLData::clear()
{
    _values.clear();
    _times.clear();
    resetMinMax();
}

qreal DLData::min() const
{
    return _min;
}

qreal DLData::max() const
{
    return _max;
}

void DLData::resetMinMax()
{
    _min = std::numeric_limits<int>::max();
    _max = std::numeric_limits<int>::min();
}
