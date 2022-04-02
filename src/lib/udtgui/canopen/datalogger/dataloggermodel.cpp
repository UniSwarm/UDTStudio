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

#include <QDebug>
#include <QMimeData>
#include <QPixmap>

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
        connect(dataLogger, &DataLogger::dataAboutToBeAdded, this, &DataLoggerModel::addDataPrepare);
        connect(dataLogger, &DataLogger::dataAdded, this, &DataLoggerModel::addDataOk);
        connect(dataLogger, &DataLogger::dataAboutToBeRemoved, this, &DataLoggerModel::removeDataPrepare);
        connect(dataLogger, &DataLogger::dataRemoved, this, &DataLoggerModel::removeDataOk);
        connect(dataLogger, &DataLogger::dataChanged, this, &DataLoggerModel::updateDlData);
    }
    _dataLogger = dataLogger;
    emit layoutChanged();
}

NodeObjectId DataLoggerModel::objId(const QModelIndex &index)
{
    const DLData *dlData = this->dlData(index);
    if (dlData == nullptr)
    {
        return NodeObjectId();
    }
    return dlData->objectId();
}

DLData *DataLoggerModel::dlData(const QModelIndex &index)
{
    if (!index.isValid() || (_dataLogger == nullptr))
    {
        return nullptr;
    }
    if (index.row() >= _dataLogger->dataList().count())
    {
        return nullptr;
    }

    return _dataLogger->dataList().at(index.row());
}

void DataLoggerModel::updateDataLoggerList()
{
    setDataLogger(_dataLogger);
}

void DataLoggerModel::updateDlData(int id)
{
    QModelIndex modelIndexTL = index(id, NodeName, QModelIndex());
    QModelIndex modelIndexBR = index(id, ColumnCount - 1, QModelIndex());
    if (modelIndexTL.isValid() && modelIndexBR.isValid())
    {
        emit dataChanged(modelIndexTL, modelIndexBR);
    }
}

void DataLoggerModel::addDataPrepare(int id)
{
    beginInsertRows(QModelIndex(), id, id);
}

void DataLoggerModel::addDataOk()
{
    endInsertRows();
}

void DataLoggerModel::removeDataPrepare(int id)
{
    beginRemoveRows(QModelIndex(), id, id);
}

void DataLoggerModel::removeDataOk()
{
    endRemoveRows();
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
                case NodeName:
                    return QVariant(tr("Node"));
                case Index:
                    return QVariant(tr("Index"));
                case Name:
                    return QVariant(tr("Name"));
                case Value:
                    return QVariant(tr("Value"));
                case Min:
                    return QVariant(tr("Min"));
                case Max:
                    return QVariant(tr("Max"));
            }
            break;
    }
    return QVariant();
}

QVariant DataLoggerModel::data(const QModelIndex &index, int role) const
{
    Node *node;
    if (!index.isValid() || (_dataLogger == nullptr))
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
                case NodeName:
                    node = dlData->node();
                    if (node != nullptr)
                    {
                        return QVariant(QString("%1.%2 %3").arg(node->busId()).arg(node->nodeId()).arg(node->name()));
                    }
                    return QVariant(tr("Unknown"));

                case Index:
                    return QVariant(QString("0x%1.%2").arg(QString::number(dlData->objectId().index(), 16), QString::number(dlData->objectId().subIndex(), 16)));

                case Name:
                    return QVariant(dlData->name());

                case Value:
                    return dlData->isEmpty() ? QVariant("-") : QVariant(QString::number(dlData->lastValue()) + dlData->unit());

                case Min:
                    return dlData->isEmpty() ? QVariant("-") : QVariant(QString::number(dlData->min()) + dlData->unit());

                case Max:
                    return dlData->isEmpty() ? QVariant("-") : QVariant(QString::number(dlData->max()) + dlData->unit());

                default:
                    return QVariant();
            }

        case Qt::DecorationRole:
            if (index.column() == NodeName)
            {
                QPixmap pix(24, 24);
                pix.fill(dlData->color());
                return pix;
            }
            return QVariant();

        case Qt::CheckStateRole:
            if (index.column() == NodeName)
            {
                return QVariant(dlData->isActive() ? Qt::Checked : Qt::Unchecked);
            }
            return QVariant();
    }
    return QVariant();
}

QModelIndex DataLoggerModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (_dataLogger == nullptr)
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
    if (_dataLogger == nullptr)
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
    Q_UNUSED(value);

    if ((_dataLogger == nullptr) || !index.isValid())
    {
        return false;
    }

    DLData *dlData = _dataLogger->dataList().at(index.row());

    if (role == Qt::CheckStateRole && index.column() == NodeName)
    {
        dlData->setActive(!dlData->isActive());
        return true;
    }

    return false;
}

Qt::ItemFlags DataLoggerModel::flags(const QModelIndex &index) const
{
    if (_dataLogger == nullptr)
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

    if (index.column() == NodeName)
    {
        flags.setFlag(Qt::ItemIsUserCheckable, true);
    }

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
        if (index.isValid() && index.column() == NodeName)
        {
            const DLData *dlData = _dataLogger->dataList().at(index.row());
            encodedData.append((dlData->objectId().mimeData() + ":").toUtf8());
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
    return mimeData->hasFormat("index/subindex");
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
