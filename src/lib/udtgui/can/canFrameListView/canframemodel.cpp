/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include <QApplication>
#include <QColor>
#include <QFont>

CanFrameModel::CanFrameModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    _bus = nullptr;
    _frameId = 0;
}

CanFrameModel::~CanFrameModel()
{
}

void CanFrameModel::appendCanFrame(const QCanBusFrame &frame)
{
    beginInsertRows(QModelIndex(), _frames.count(), _frames.count());
    if (_frames.isEmpty())
    {
        _startTime = frame.timeStamp().seconds();
    }
    _frames.append(frame);
    endInsertRows();
}

void CanFrameModel::clear()
{
    emit layoutAboutToBeChanged();
    _frames.clear();
    emit layoutChanged();
}

CanOpenBus *CanFrameModel::bus() const
{
    return _bus;
}

void CanFrameModel::setBus(CanOpenBus *bus)
{
    emit layoutAboutToBeChanged();
    if (_bus != nullptr)
    {
        disconnect(_bus, &CanOpenBus::frameAvailable, this, &CanFrameModel::updateFrames);
    }
    _bus = bus;
    _frameId = _bus->canFramesLog().count();
    connect(bus, &CanOpenBus::frameAvailable, this, &CanFrameModel::updateFrames);
    emit layoutChanged();
}

void CanFrameModel::updateFrames(int id)
{
    beginInsertRows(QModelIndex(), _frameId, id - 1);
    _frameId = id;
    endInsertRows();
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

    if (_bus == nullptr)
    {
        // internal data mode
        if (index.row() >= _frames.count())
        {
            return QVariant();
        }
    }
    else
    {
        // bus data mode
        if (index.row() >= _frameId)
        {
            return QVariant();
        }
    }
    const QCanBusFrame &canFrame = (_bus == nullptr) ? _frames.at(index.row()) : _bus->canFramesLog().at(index.row());

    switch (role)
    {
        case Qt::DisplayRole:
            switch (index.column())
            {
                case Time:
                    return QVariant(QStringLiteral("%1.%2")
                                        .arg(canFrame.timeStamp().seconds() - _startTime)
                                        .arg(QString::number(canFrame.timeStamp().microSeconds() / 1000).rightJustified(3, '0')));

                case CanId:
                    return QVariant(QStringLiteral("0x%1 (%2)").arg(QString::number(canFrame.frameId(), 16)).arg(canFrame.frameId()));

                case Type:
                    switch (canFrame.frameType())
                    {
                        case QCanBusFrame::UnknownFrame:
                            return QVariant(tr("unk"));
                        case QCanBusFrame::DataFrame:
                            return QVariant(tr("Dat(%1)").arg(canFrame.payload().size()));
                        case QCanBusFrame::ErrorFrame:
                            return QVariant(tr("Err"));
                        case QCanBusFrame::RemoteRequestFrame:
                            return QVariant(tr("RTR"));
                        case QCanBusFrame::InvalidFrame:
                            return QVariant(tr("NV"));
                    }
                    return QVariant();

                case DataByte:
                    return QVariant(canFrame.payload().toHex(' ').toUpper());

                default:
                    return QVariant();
            }

        case Qt::TextAlignmentRole:
            switch (index.column())
            {
                case Time:
                    return QVariant(Qt::AlignRight | Qt::AlignVCenter);

                case Type:
                    return QVariant(Qt::AlignHCenter | Qt::AlignVCenter);

                default:
                    return QVariant();
            }

        case Qt::FontRole:
            switch (index.column())
            {
                case DataByte:
                {
                    QFont fontMono = QApplication::font();
                    fontMono.setStyleHint(QFont::Monospace);
                    return QVariant(fontMono);
                }
                default:
                    return QVariant();
            }

        case Qt::BackgroundRole:
            if (canFrame.hasLocalEcho())
            {
                return QVariant();
            }
            else
            {
                return QVariant(QColor(Qt::lightGray));
            }
    }
    return QVariant();
}

QModelIndex CanFrameModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    if (_bus == nullptr)
    {
        // internal data mode
        if (row >= _frames.count())
        {
            return QModelIndex();
        }
    }
    else
    {
        // bus data mode
        if (row >= _frameId)
        {
            return QModelIndex();
        }
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
        if (_bus == nullptr)
        {
            return _frames.count();
        }
        return _frameId;
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
