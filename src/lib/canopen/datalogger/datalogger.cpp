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
#include <QFile>
#include <QTextStream>

#include "db/odindexdb.h"

DataLogger::DataLogger(QObject *parent)
    : QObject(parent)
{
    _timer.setTimerType(Qt::PreciseTimer);
    connect(&_timer, &QTimer::timeout, this, &DataLogger::readData);

    _timerNotify.setInterval(100);
    connect(&_timerNotify, &QTimer::timeout, this, &DataLogger::notify);
}

DataLogger::~DataLogger()
{
    removeAllData();
}

bool DataLogger::isStarted() const
{
    return _timer.isActive();
}

void DataLogger::addData(const NodeObjectId &objId)
{
    if (_dataMap.contains(objId.key()) || (!objId.isAnIndex() && !objId.isASubIndex()))
    {
        return;
    }

    QList<NodeObjectId> objToAdd;
    NodeIndex *nodeIndex = objId.nodeIndex();
    if (nodeIndex == nullptr)
    {
        return;
    }

    if (nodeIndex->subIndexesCount() == 1)
    {
        addDlData(objId);
        return;
    }
    for (NodeSubIndex *nodeSubIndex : nodeIndex->subIndexes())
    {
        if (!nodeSubIndex->isReadable())
        {
            continue;
        }
        if (nodeSubIndex->subIndex() == 0 && !nodeSubIndex->isWritable())
        {
            continue;
        }

        NodeObjectId objectId = nodeSubIndex->objectId();
        if (!_dataMap.contains(objectId.key()))
        {
            objToAdd.append(objectId);
        }
    }

    for (const NodeObjectId &mobjId : objToAdd)
    {
        addDlData(mobjId);
    }
}

void DataLogger::addData(const QList<NodeObjectId> &objIds)
{
    for (const NodeObjectId &objId : qAsConst(objIds))
    {
        addData(objId);
    }
}

void DataLogger::removeData(const NodeObjectId &objId)
{
    DLData *dlData = data(objId);
    if (dlData == nullptr)
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
    valueDouble *= dlData->scale();

    dlData->appendData(valueDouble, dateTime);

    dlData->setHasChanged(true);
}

void DataLogger::exportCSVData(const QString &fileName)
{
    QList<quint64> timeStamps;
    QVector<QMap<quint64, double>> maps(_dataList.count());
    uint dlDataId = 0;
    for (DLData *dlData : _dataList)
    {
        int minCount = qMin(dlData->values().count(), dlData->times().count());
        for (int i = 0; i < minCount; i++)
        {
            quint64 time = dlData->times()[i].toMSecsSinceEpoch();
            maps[dlDataId].insert(time, dlData->values()[i]);
            timeStamps.append(time);
        }
        dlDataId++;
    }

    // sort timestamps and make it unique
    std::sort(timeStamps.begin(), timeStamps.end());
    auto last = std::unique(timeStamps.begin(), timeStamps.end());
    timeStamps.erase(last, timeStamps.end());

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream stream(&file);
    stream << "Time (s)"
           << ";";
    for (DLData *dlData : _dataList)
    {
        stream << dlData->name() << " (" << dlData->unit() << ")"
               << ";";
    }
    stream << '\n';

    quint64 firstDateTime = timeStamps.first();
    for (quint64 time : timeStamps)
    {
        stream << static_cast<double>(time - firstDateTime) / 1000.0 << ';';
        for (const auto &mapValues : maps)
        {
            auto it = mapValues.constFind(time);
            if (it != mapValues.constEnd())
            {
                stream << QString::number(*it, 'f');
            }
            stream << ";";
        }
        stream << '\n';
    }
    file.close();
}

void DataLogger::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if (!_timer.isActive())
    {
        return;
    }

    DLData *dlData = data(objId);
    if ((dlData == nullptr) || !dlData->isActive() || flags == NodeOd::Error)
    {
        return;
    }

    const QVariant &value = dlData->nodeSubIndex()->value();
    const QDateTime &dateTime = dlData->nodeSubIndex()->lastModification();
    addDataValue(dlData, value, dateTime);
}

void DataLogger::start(int ms)
{
    _timerNotify.start();
    _timer.start(ms);
    emit startChanged(true);
}

void DataLogger::stop()
{
    _timerNotify.stop();
    _timer.stop();
    emit startChanged(false);
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
        if ((dlData->node() != nullptr) && dlData->isActive())
        {
            dlData->node()->readObject(dlData->objectId());
        }
    }
}

void DataLogger::notify()
{
    for (DLData *dlData : qAsConst(_dataList))
    {
        if (dlData->hasChanged())
        {
            dlData->setHasChanged(false);
            emit valueChanged(_dataList.indexOf(dlData));
        }
    }
}

void DataLogger::addDlData(const NodeObjectId &mobjId)
{
    // TODO optimize emit signal
    emit dataAboutToBeAdded(_dataList.count());
    DLData *dlData = new DLData(mobjId);
    dlData->setColor(findFreeColor());
    dlData->setActive(true);
    _dataMap.insert(dlData->key(), dlData);
    _dataList.append(dlData);
    registerObjId(dlData->objectId());
    emit dataAdded();

    connect(dlData->node(),
            &QObject::destroyed,
            this,
            [=]()
            {
                removeData(mobjId);
            });
}

QColor DataLogger::findFreeColor() const
{
    int c = 0;
    QColor color;
    do
    {
        color = QColor::fromHsv((c++ * 48) % 360, 255, 255, 200);
    } while (!isColorFree(color) && c < 24);
    return color;
}

bool DataLogger::isColorFree(const QColor &color) const
{
    for (DLData *dlData : qAsConst(_dataList))
    {
        if (dlData->color() == color)
        {
            return false;
        }
    }
    return true;
}
