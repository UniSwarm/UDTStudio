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

#ifndef NODEODITEMMODEL_H
#define NODEODITEMMODEL_H

#include "../../udtgui_global.h"

#include <QAbstractItemModel>
#include "nodeodsubscriber.h"

class Node;
class NodeIndex;
class NodeSubIndex;
class NodeOdItem;

class UDTGUI_EXPORT NodeOdItemModel : public QAbstractItemModel, public NodeOdSubscriber
{
    Q_OBJECT
public:
    NodeOdItemModel(QObject *parent = nullptr);
    ~NodeOdItemModel();

    Node *node() const;
    NodeIndex *nodeIndex(const QModelIndex &index) const;
    NodeSubIndex *nodeSubIndex(const QModelIndex &index) const;

    bool isEditable() const;
    void setEditable(bool editable);

    enum Column {
        OdIndex,
        Name,
        Type,
        Acces,
        Value,
        ColumnCount
    };

public slots:
    void setNode(Node *node);

    // QAbstractItemModel interface
public:
    int columnCount(const QModelIndex &parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

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

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;

protected:
    QModelIndex indexItem(quint16 index, int col);
    QModelIndex subIndexItem(quint16 index, quint8 subindex, int col);

private:
    NodeOdItem *_root;
    Node *_node;
    bool _editable;
};

#endif // NODEODITEMMODEL_H
