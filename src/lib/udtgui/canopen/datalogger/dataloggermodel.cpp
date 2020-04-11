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
#include <QMimeData>

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

    if (dataLogger != _dataLogger)
    {
        connect(dataLogger, &DataLogger::dataListChanged, this, &DataLoggerModel::updateDataLoggerList);
        connect(dataLogger, &DataLogger::dataChanged, this, &DataLoggerModel::updateDlData);
    }
    _dataLogger = dataLogger;
    emit layoutChanged();
}

void DataLoggerModel::updateDataLoggerList()
{
    setDataLogger(_dataLogger);
}

void DataLoggerModel::updateDlData(int id)
{
    QModelIndex modelIndexTL = index(id, Node, QModelIndex());
    QModelIndex modelIndexBR = index(id, Value, QModelIndex());
    if (modelIndexTL.isValid() && modelIndexBR.isValid())
    {
        emit dataChanged(modelIndexTL, modelIndexBR);
    }
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
        case Name:
            return QVariant(tr("Name"));
        case Value:
            return QVariant(tr("Value"));
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
            return QVariant(QString("0x%1").arg(QString::number(dlData->objectId().subIndex, 16)));

        case Name:
            return QVariant(dlData->name());

        case Value:
            return QVariant(dlData->lastValue());

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

bool DataLoggerModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    return false;
}

Qt::ItemFlags DataLoggerModel::flags(const QModelIndex &index) const
{
    if (!_dataLogger)
    {
        return Qt::NoItemFlags;
    }
    if (!index.isValid())
    {
        return Qt::ItemIsDropEnabled;
    }

    Qt::ItemFlags flags;
    flags.setFlag(Qt::ItemIsEditable, false);
    flags.setFlag(Qt::ItemIsSelectable, true);
    flags.setFlag(Qt::ItemIsEnabled, true);
    return flags;
}

QStringList DataLoggerModel::mimeTypes() const
{
    QStringList types;
    types << "index/subindex";
    return types;
}

QMimeData *DataLoggerModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    foreach (QModelIndex index, indexes)
    {
        if (index.isValid() && index.column() == Node)
        {
            const DLData *dlData = _dataLogger->dataList().at(index.row());
            encodedData.append(dlData->objectId().mimeData() + ":");
        }
    }
    mimeData->setData("index/subindex", encodedData);
    return mimeData;
}

bool DataLoggerModel::canDropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action)
    Q_UNUSED(row)
    Q_UNUSED(column)
    Q_UNUSED(parent)
    if (mimeData->hasFormat("index/subindex"))
    {
        return true;
    }
    return false;
}

bool DataLoggerModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row)
    Q_UNUSED(column)
    Q_UNUSED(parent)
    if (action == Qt::IgnoreAction)
    {
        return true;
    }
    if (mimeData->hasFormat("index/subindex"))
    {
        const QStringList &stringListObjId = QString(mimeData->data("index/subindex")).split(':', QString::SkipEmptyParts);
        for (const QString &stringObjId : stringListObjId)
        {
            NodeObjectId objId = NodeObjectId::fromMimeData(stringObjId);
            _dataLogger->addData(objId);
        }
        return true;
    }
    return false;
}

Qt::DropActions DataLoggerModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

Qt::DropActions DataLoggerModel::supportedDragActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}
