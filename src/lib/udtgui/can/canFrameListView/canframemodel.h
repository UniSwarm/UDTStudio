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

#ifndef CANFRAMEMODEL_H
#define CANFRAMEMODEL_H

#include "../../udtgui_global.h"

#include <QAbstractItemModel>

#include "busdriver/qcanbusframe.h"

#include "canopenbus.h"

class UDTGUI_EXPORT CanFrameModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    CanFrameModel(QObject *parent = nullptr);
    ~CanFrameModel() override;

    void appendCanFrame(const QCanBusFrame &frame);
    void clear();

    CanOpenBus *bus() const;
    void setBus(CanOpenBus *bus);

    enum Column
    {
        Time,
        CanId,
        Type,
        DataByte,
        ColumnCount
    };

protected slots:
    void updateFrames(int id);

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    qint64 _startTime;

    QList<QCanBusFrame> _frames;

    int _frameId;
    CanOpenBus *_bus;
};

#endif  // CANFRAMEMODEL_H
