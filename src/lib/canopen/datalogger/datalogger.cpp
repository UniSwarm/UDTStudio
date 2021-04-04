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

#include "datalogger.h"

#include <QDebug>

DataLogger::DataLogger(QObject *parent)
    : QObject(parent)
{
    connect(&_timer, &QTimer::timeout, this, &DataLogger::readData);
}

void DataLogger::addData(const NodeObjectId &objId)
{
    if (_dataMap.contains(objId.key()) || (!objId.isAnIndex() && !objId.isASubIndex()))
    {
        return;
    }

    QList<NodeObjectId> objToAdd;
    if (objId.isAnIndex())
    {
        NodeIndex *nodeIndex = objId.nodeIndex();
        if (!nodeIndex)
        {
            return;
        }
        for (NodeSubIndex *nodeSubIndex : nodeIndex->subIndexes())
        {
            if (nodeSubIndex->subIndex() != 0 && nodeSubIndex->isReadable())
            {
                NodeObjectId objectId = nodeSubIndex->objectId();
                if (!_dataMap.contains(objectId.key()))
                {
                    objToAdd.append(objectId);
                }
            }
        }
    }
    else
    {
        objToAdd.append(objId);
    }

    for (const NodeObjectId &mobjId : objToAdd)
    {
        // TODO optimize emit signal
        emit dataAboutToBeAdded(_dataList.count());
        DLData *dlData = new DLData(mobjId);
        dlData->setColor(QColor::fromHsv((_dataList.count() * 50) % 360, 255, 255));
        _dataMap.insert(dlData->key(), dlData);
        _dataList.append(dlData);
        registerObjId(dlData->objectId());
        emit dataAdded();
    }
}

void DataLogger::removeData(const NodeObjectId &objId)
{
    DLData *dlData = data(objId);
    if (!dlData)
    {
        return;
    }

    emit dataAboutToBeRemoved(_dataList.indexOf(dlData));

    _dataMap.remove(dlData->key());
    _dataList.removeOne(dlData);
    unRegisterObjId(dlData->objectId());
    delete dlData;

    emit dataRemoved();
}

void DataLogger::removeAllData()
{
    for (DLData *dlData : qAsConst(_dataList))
    {
        removeData(dlData->objectId());
    }
}

QList<DLData *> &DataLogger::dataList()
{
    return _dataList;
}

DLData *DataLogger::data(int index) const
{
    return _dataList.at(index);
}

DLData *DataLogger::data(const NodeObjectId &objId) const
{
    return _dataMap.value(objId.key());
}

qreal DataLogger::min() const
{
    qreal min = std::numeric_limits<int>::max();
    for (DLData *dlData : _dataList)
    {
        min = qMin(dlData->min(), min);
    }
    return min;
}

qreal DataLogger::max() const
{
    qreal max = std::numeric_limits<int>::min();
    for (DLData *dlData : _dataList)
    {
        max = qMax(dlData->max(), max);
    }
    return max;
}

void DataLogger::range(qreal &min, qreal &max) const
{
    min = std::numeric_limits<int>::max();
    max = std::numeric_limits<int>::min();
    for (DLData *dlData : _dataList)
    {
        min = qMin(dlData->min(), min);
        max = qMax(dlData->max(), max);
    }
}

QDateTime DataLogger::firstDateTime() const
{
    QDateTime first;
    if (_dataList.isEmpty())
    {
        return QDateTime();
    }
    first = _dataList.first()->firstDateTime();
    for (DLData *dlData : _dataList)
    {
        const QDateTime &dateTime = dlData->firstDateTime();
        if (dateTime.isValid() && first > dateTime)
        {
            first = dateTime;
        }
    }
    return first;
}

QDateTime DataLogger::lastDateTime() const
{
    QDateTime last;
    if (_dataList.isEmpty())
    {
        return QDateTime();
    }
    last = _dataList.first()->lastDateTime();
    for (DLData *dlData : _dataList)
    {
        const QDateTime &dateTime = dlData->lastDateTime();
        if (last < dateTime)
        {
            last = dateTime;
        }
    }
    return last;
}

void DataLogger::addDataValue(DLData *dlData, const QVariant &value, const QDateTime &dateTime)
{
    double valueDouble = value.toDouble();

    if (dlData->isQ1516())
    {
        valueDouble /= 65536.0;
    }

    dlData->appendData(valueDouble, dateTime);

    emit dataChanged(_dataList.indexOf(dlData));
}

void DataLogger::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (!_timer.isActive())
    {
        return;
    }

    DLData *dlData = data(objId);
    if (!dlData || flags == SDO::Error)
    {
        return;
    }

    const QVariant &value = dlData->node()->nodeOd()->value(dlData->objectId());
    const QDateTime &dateTime = dlData->node()->nodeOd()->lastModification(dlData->objectId());
    addDataValue(dlData, value, dateTime);
}

void DataLogger::start(int ms)
{
    _timer.start(ms);
}

void DataLogger::stop()
{
    _timer.stop();
}

void DataLogger::clear()
{
    QMap<quint64, DLData *>::iterator i = _dataMap.begin();
    while (i != _dataMap.end())
    {
        i.value()->clear();
        emit dataChanged(_dataList.indexOf(i.value()));
        ++i;
    }
}

void DataLogger::readData()
{
    for (DLData *dlData : qAsConst(_dataList))
    {
        if (dlData->node())
        {
            dlData->node()->readObject(dlData->objectId());
        }
    }
}
