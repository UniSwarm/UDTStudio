/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#include "oditemmodel.h"

#include "oditem.h"

ODItemModel::ODItemModel()
{
    _root = nullptr;
}

ODItemModel::~ODItemModel()
{
    delete _root;
}

void ODItemModel::setOd(OD *od)
{
    beginResetModel();
    _root = new ODItem(od);
    endResetModel();
}

int ODItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant ODItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation==Qt::Vertical)
        return QVariant();
    switch (role)
    {
    case Qt::DisplayRole:
        switch (section)
        {
        case OdIndex:
            return QVariant("Index");
        case Name:
            return QVariant("Name");
        case Type:
            return QVariant("Type");
        case Value:
            return QVariant("Value");
        }
        break;
    }
    return QVariant();
}

QVariant ODItemModel::data(const QModelIndex &index, int role) const
{
    if (!_root)
        return QVariant();
    if (!index.isValid())
        return QVariant();

    ODItem *item = static_cast<ODItem*>(index.internalPointer());

    return item->data(index.column(), role);
}

QModelIndex ODItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!_root)
        return QModelIndex();

    ODItem *item;
    if (parent.internalPointer() == nullptr)
        item = _root;
    else
        item = static_cast<ODItem*>(parent.internalPointer());

    ODItem *childItem = item->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex ODItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid() || child.internalPointer() == nullptr)
        return QModelIndex();
    ODItem *item = static_cast<ODItem*>(child.internalPointer());
    if (item->parent() == nullptr)
        return QModelIndex();
    return createIndex(item->row(), child.column(), item->parent());
}

int ODItemModel::rowCount(const QModelIndex &parent) const
{
    if (!_root)
        return 0;
    if (!parent.isValid())
        return _root->rowCount();

    ODItem *item = static_cast<ODItem*>(parent.internalPointer());
    return item->rowCount();
}

Qt::ItemFlags ODItemModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}
