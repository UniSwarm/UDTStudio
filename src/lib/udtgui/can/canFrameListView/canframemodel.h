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

#ifndef CANFRAMEMODEL_H
#define CANFRAMEMODEL_H

#include <QAbstractItemModel>
#include <QCanBus>
#include <QCanBusFrame>

class CanFrameModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    CanFrameModel(QObject *parent = nullptr);
    ~CanFrameModel();

    void appendCanFrame(const QCanBusFrame &frame);

    enum Column {
        Time,
        CanId,
        Type,
        DLC,
        DataByte,
        ColumnCount
    };

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QVariant data(const QModelIndex &index, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

private:
    QList<QCanBusFrame> _frames;
};

#endif // CANFRAMEMODEL_H
