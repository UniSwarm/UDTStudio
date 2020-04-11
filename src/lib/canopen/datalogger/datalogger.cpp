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

#include "datalogger.h"

#include <QDebug>

DataLogger::DataLogger()
{
    connect(&_timer, &QTimer::timeout, this, &DataLogger::readData);
    start(100);
}

void DataLogger::addData(const NodeObjectId &objId)
{
    if (!objId.isValid() || _dataMap.contains(objId.key()))
    {
        return;
    }

    DLData *data = new DLData(objId);
    _dataMap.insert(data->key(), data);
    _dataList.append(data);
    registerObjId(data->objectId());

    emit dataListChanged();
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

void DataLogger::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    DLData *dlData = data(objId);
    if (!dlData || flags == SDO::Error)
    {
        return;
    }

    const QVariant &value = dlData->node()->nodeOd()->value(dlData->objectId());
    const QDateTime &dateTime = dlData->node()->nodeOd()->lastModification(dlData->objectId());
    dlData->appendData(value.toDouble(), dateTime);

    emit dataChanged(_dataList.indexOf(dlData));
}

void DataLogger::start(int ms)
{
    _timer.start(ms);
}

void DataLogger::stop()
{
    _timer.stop();
}

void DataLogger::readData()
{
    for (DLData *dlData : _dataList)
    {
        dlData->node()->readObject(dlData->objectId());
    }
}
