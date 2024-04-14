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

#ifndef ODITEMMODEL_H
#define ODITEMMODEL_H

#include "udtgui_global.h"

#include <QAbstractItemModel>

#include "model/devicemodel.h"

class ODItem;

class UDTGUI_EXPORT ODItemModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    ODItemModel();
    ~ODItemModel() override;

    void setDeviceModel(DeviceModel *deviceModel);
    DeviceModel *deviceModel() const;

    bool editable() const;
    void setEditable(bool editable);

    enum Column
    {
        OdIndex,
        Name,
        Type,
        Value,
        ColumnCount
    };

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

private:
    ODItem *_root;
    DeviceModel *_deviceModel;
    bool _editable;
};

#endif  // ODITEMMODEL_H
