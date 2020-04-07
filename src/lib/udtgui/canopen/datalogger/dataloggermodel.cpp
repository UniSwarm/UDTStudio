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

#include <QDebug>

#include "dataloggermodel.h"

DataLoggerModel::DataLoggerModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    _dataLogger = nullptr;
}

DataLoggerModel::~DataLoggerModel()
{
}

DataLogger *DataLoggerModel::dataLogger() const
{
    return _dataLogger;
}

void DataLoggerModel::setDataLogger(DataLogger *dataLogger)
{
    emit layoutAboutToBeChanged();
    _dataLogger = dataLogger;
    emit layoutChanged();
}

int DataLoggerModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant DataLoggerModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Vertical)
    {
        return QVariant();
    }
    switch (role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
        case Node:
            return QVariant(tr("Node"));
        case Index:
            return QVariant(tr("Index"));
        case SubIndex:
            return QVariant(tr("SubIndex"));
        }
        break;
    }
    return QVariant();
}

QVariant DataLoggerModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !_dataLogger)
    {
        return QVariant();
    }
    if (index.row() >= _dataLogger->dataList().count())
    {
        return QVariant();
    }

    const DLData *dlData = _dataLogger->dataList().at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case Node:
            return QVariant(QString("%1.%2 %3").arg(dlData->node()->busId()).arg(dlData->node()->nodeId()).arg(dlData->node()->name()));

        case Index:
            return QVariant(QString("0x%1").arg(QString::number(dlData->objectId().index, 16)));

        case SubIndex:
            return QVariant(QString("0x%1").arg(QString::number(dlData->objectId().index, 16)));

        default:
            return QVariant();
        }
    }
    return QVariant();
}

QModelIndex DataLoggerModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (!_dataLogger)
    {
        return QModelIndex();
    }
    if (row >= _dataLogger->dataList().count())
    {
        return QModelIndex();
    }
    return createIndex(row, column, nullptr);
}

QModelIndex DataLoggerModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

int DataLoggerModel::rowCount(const QModelIndex &parent) const
{
    if (!_dataLogger)
    {
        return 0;
    }
    if (!parent.isValid())
    {
        return _dataLogger->dataList().count();
    }
    return 0;
}

Qt::ItemFlags DataLoggerModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    Qt::ItemFlags flags;
    flags.setFlag(Qt::ItemIsEditable, false);
    flags.setFlag(Qt::ItemIsSelectable, true);
    flags.setFlag(Qt::ItemIsEnabled, true);
    return flags;
}
