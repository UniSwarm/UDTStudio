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

#include "dldata.h"

#include "canopen.h"

#include "indexdb.h"

DLData::DLData(const NodeObjectId &objectId)
    : _objectId(objectId)
{
    _node = nullptr;
    _active = false;
    _scale = 1.0;
    _q1516 = false;

    CanOpenBus *bus = CanOpen::bus(objectId.busId());
    if (bus != nullptr)
    {
        _node = bus->node(objectId.nodeId());
        NodeSubIndex *nodeSubIndex = objectId.nodeSubIndex();
        if (nodeSubIndex != nullptr)
        {
            _name = nodeSubIndex->nodeIndex()->name() + "." + nodeSubIndex->name();
            _q1516 = nodeSubIndex->isQ1516();
            _scale = nodeSubIndex->scale();
            _unit = nodeSubIndex->unit();
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

bool DLData::isActive() const
{
    return _active;
}

void DLData::setActive(bool active)
{
    _active = active;
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

qreal DLData::scale() const
{
    return _scale;
}

void DLData::setScale(qreal scale)
{
    _scale = scale;
}

QString DLData::unit() const
{
    return _unit;
}

void DLData::setUnit(const QString &unit)
{
    _unit = unit;
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

int DLData::valuesCount() const
{
    return _values.size();
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

bool DLData::isEmpty() const
{
    return _values.isEmpty();
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

bool DLData::isQ1516() const
{
    return _q1516;
}

void DLData::setQ1516(bool q1516)
{
    _q1516 = q1516;
}
