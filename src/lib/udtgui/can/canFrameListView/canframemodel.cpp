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

#include "canframemodel.h"
#include "canframelistview.h"

CanFrameModel::CanFrameModel(QObject *parent)
    : QAbstractItemModel(parent)
{
}

CanFrameModel::~CanFrameModel()
{
}

void CanFrameModel::appendCanFrame(const QCanBusFrame &frame)
{
    // TODO use insert system
    emit layoutAboutToBeChanged();
    _frames.append(frame);
    emit layoutChanged();
}

void CanFrameModel::clear()
{
    emit layoutAboutToBeChanged();
    _frames.clear();
    emit layoutChanged();
}

int CanFrameModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant CanFrameModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case Time:
            return QVariant(tr("Time"));
        case CanId:
            return QVariant(tr("CanId"));
        case Type:
            return QVariant(tr("Type"));
        case DLC:
            return QVariant(tr("DLC"));
        case DataByte:
            return QVariant(tr("DataByte"));
        }
        break;
    }
    return QVariant();
}

QVariant CanFrameModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    {
        return QVariant();
    }
    if (index.row() >= _frames.count())
    {
        return QVariant();
    }

    const QCanBusFrame &canFrame = _frames.at(index.row());

    switch (role)
    {
    case Qt::DisplayRole:
        switch (index.column())
        {
        case Time:
            return QVariant(QString("%1:%2").arg(canFrame.timeStamp().seconds()).arg(canFrame.timeStamp().microSeconds() / 10000));
        case CanId:
            return QVariant(QString("0x%1 (%2)").arg(QString::number(canFrame.frameId(), 16)).arg(canFrame.frameId()));
        case Type:
            switch (canFrame.frameType())
            {
            case QCanBusFrame::UnknownFrame:
                return QVariant(tr("unk"));
            case QCanBusFrame::DataFrame:
                return QVariant(tr("Data"));
            case QCanBusFrame::ErrorFrame:
                return QVariant(tr("Err"));
            case QCanBusFrame::RemoteRequestFrame:
                return QVariant(tr("RTR"));
            case QCanBusFrame::InvalidFrame:
                return QVariant(tr("NV"));
            }
            return QVariant();
        case DLC:
            return QVariant(canFrame.payload().count());
        case DataByte:
            return QVariant(canFrame.payload().toHex(' ').toUpper());
        default:
            return QVariant();
        }
    }
    return QVariant();
}

QModelIndex CanFrameModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (row >= _frames.count())
    {
        return QModelIndex();
    }
    return createIndex(row, column, nullptr);
}

QModelIndex CanFrameModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}

int CanFrameModel::rowCount(const QModelIndex &parent) const
{
    if (!parent.isValid())
    {
        return _frames.count();
    }
    return 0;
}

Qt::ItemFlags CanFrameModel::flags(const QModelIndex &index) const
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
