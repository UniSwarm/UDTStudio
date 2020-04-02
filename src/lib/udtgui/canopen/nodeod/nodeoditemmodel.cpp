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

#include "nodeoditemmodel.h"

#include <QDebug>

#include "node.h"
#include "nodeoditem.h"

NodeOdItemModel::NodeOdItemModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    _root = nullptr;
    _node = nullptr;

    registerFullOd();
}

NodeOdItemModel::~NodeOdItemModel()
{
    delete _root;
}

Node *NodeOdItemModel::node() const
{
    return _node;
}

void NodeOdItemModel::setNode(Node *node)
{
    beginResetModel();
    delete _root;
    _node = node;
    setNodeInterrest(_node);
    if (_node)
    {
        _root = new NodeOdItem(node->nodeOd());
    }
    else
    {
        _root = nullptr;
    }
    endResetModel();
}

bool NodeOdItemModel::isEditable() const
{
    return _editable;
}

void NodeOdItemModel::setEditable(bool editable)
{
    _editable = editable;
}

int NodeOdItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant NodeOdItemModel::headerData(int section, Qt::Orientation orientation, int role) const
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
        case OdIndex:
            return QVariant(tr("Index"));
        case Name:
            return QVariant(tr("Name"));
        case Type:
            return QVariant(tr("Type"));
        case Value:
            return QVariant(tr("Value"));
        }
        break;
    }
    return QVariant();
}

QModelIndex NodeOdItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!_root)
    {
        return QModelIndex();
    }

    NodeOdItem *item;
    if (parent.internalPointer() == nullptr)
    {
        item = _root;
    }
    else
    {
        item = static_cast<NodeOdItem *>(parent.internalPointer());
    }

    NodeOdItem *childItem = item->child(row);
    if (childItem)
    {
        return createIndex(row, column, childItem);
    }
    else
    {
        return QModelIndex();
    }
}

QModelIndex NodeOdItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || child.internalPointer() == nullptr)
    {
        return QModelIndex();
    }

    NodeOdItem *item = static_cast<NodeOdItem *>(child.internalPointer());
    if (item->parent() == nullptr)
    {
        return QModelIndex();
    }
    return createIndex(item->parent()->row(), 0, item->parent());
}

int NodeOdItemModel::rowCount(const QModelIndex &parent) const
{
    if (!_root)
    {
        return 0;
    }
    if (!parent.isValid())
    {
        return _root->rowCount();
    }

    NodeOdItem *item = static_cast<NodeOdItem *>(parent.internalPointer());
    return item->rowCount();
}

QVariant NodeOdItemModel::data(const QModelIndex &index, int role) const
{
    if (!_root)
    {
        return QVariant();
    }
    if (!index.isValid())
    {
        return QVariant();
    }

    NodeOdItem *item = static_cast<NodeOdItem *>(index.internalPointer());

    return item->data(index.column(), role);
}

QModelIndex NodeOdItemModel::indexItem(quint16 index, int col)
{
    if (!_root)
    {
        return QModelIndex();
    }

    NodeOdItem *childIndex = _root->childIndex(index);
    if (!childIndex)
    {
        return QModelIndex();
    }
    return createIndex(childIndex->row(), col, childIndex);
}

QModelIndex NodeOdItemModel::subIndexItem(quint16 index, quint8 subindex, int col)
{
    if (!_root)
    {
        return QModelIndex();
    }

    NodeOdItem *childIndex = _root->childIndex(index);
    if (!childIndex)
    {
        return QModelIndex();
    }
    if (childIndex->rowCount() == 0)
    {
        return createIndex(childIndex->row(), col, childIndex);
    }

    NodeOdItem *childSubIndex = childIndex->childIndex(subindex);
    if (!childSubIndex)
    {
        return QModelIndex();
    }
    return createIndex(childSubIndex->row(), col, childSubIndex);
}

void NodeOdItemModel::odNotify(quint16 index, quint8 subindex, const QVariant &value)
{
    Q_UNUSED(value)
    QModelIndex modelIndex = subIndexItem(index, subindex, 3);
    if (modelIndex.isValid())
    {
        emit dataChanged(modelIndex, modelIndex);
    }
}
