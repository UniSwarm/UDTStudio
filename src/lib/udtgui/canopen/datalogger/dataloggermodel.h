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

#ifndef DATALOGGERMODEL_H
#define DATALOGGERMODEL_H

#include "../../udtgui_global.h"

#include <QAbstractItemModel>

#include "datalogger/datalogger.h"

class UDTGUI_EXPORT DataLoggerModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    DataLoggerModel(QObject *parent = nullptr);
    ~DataLoggerModel();

    DataLogger *dataLogger() const;
    void setDataLogger(DataLogger *dataLogger);

    enum Column {
        Node,
        Index,
        SubIndex,
        Name,
        Value,
        ColumnCount
    };

protected slots:
    void updateDataLoggerList();
    void updateDlData(int id);

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;

    // set data support
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    // drag and drop / mimedata
    QStringList mimeTypes() const override;
    QMimeData *mimeData(const QModelIndexList &indexes) const override;
    bool canDropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent) override;
    Qt::DropActions supportedDropActions() const override;
    Qt::DropActions supportedDragActions() const override;

private:
    DataLogger *_dataLogger;
};

#endif // DATALOGGERMODEL_H
