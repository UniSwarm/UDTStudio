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

#include "oditemmodel.h"

#include "oditem.h"

ODItemModel::ODItemModel()
{
    _root = nullptr;
    _editable = false;
}

ODItemModel::~ODItemModel()
{
    delete _root;
}

void ODItemModel::setDeviceModel(DeviceModel *deviceModel)
{
    beginResetModel();
    _root = new ODItem(deviceModel);
    endResetModel();
}

DeviceModel *ODItemModel::deviceModel() const
{
    return _deviceModel;
}

bool ODItemModel::editable() const
{
    return _editable;
}

void ODItemModel::setEditable(bool editable)
{
    _editable = editable;
}

int ODItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant ODItemModel::headerData(int section, Qt::Orientation orientation, int role) const
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

QVariant ODItemModel::data(const QModelIndex &index, int role) const
{
    if (_root == nullptr)
    {
        return QVariant();
    }
    if (!index.isValid())
    {
        return QVariant();
    }

    ODItem *item = static_cast<ODItem *>(index.internalPointer());

    return item->data(index.column(), role);
}

bool ODItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if ((_root == nullptr) || !index.isValid())
    {
        return false;
    }

    ODItem *item = static_cast<ODItem *>(index.internalPointer());
    return item->setData(index.column(), value, role);
}

QModelIndex ODItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (_root == nullptr)
    {
        return QModelIndex();
    }

    ODItem *item;
    if (parent.internalPointer() == nullptr)
    {
        item = _root;
    }
    else
    {
        item = static_cast<ODItem *>(parent.internalPointer());
    }

    ODItem *childItem = item->child(row);
    if (childItem != nullptr)
    {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex ODItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    ODItem *childItem = static_cast<ODItem *>(child.internalPointer());
    ODItem *parentItem = childItem->parent();

    if (parentItem == _root)
    {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int ODItemModel::rowCount(const QModelIndex &parent) const
{
    if (_root == nullptr)
    {
        return 0;
    }

    ODItem *parentItem;
    if (parent.internalPointer() == nullptr)
    {
        parentItem = _root;
    }
    else
    {
        parentItem = static_cast<ODItem *>(parent.internalPointer());
    }

    return parentItem->rowCount();
}

Qt::ItemFlags ODItemModel::flags(const QModelIndex &index) const
{
    if ((_root == nullptr) || !index.isValid())
    {
        return Qt::NoItemFlags;
    }

    ODItem *item = static_cast<ODItem *>(index.internalPointer());
    Qt::ItemFlags flags = Qt::ItemIsEnabled | Qt::ItemIsSelectable;
    if (_editable)
    {
        flags.setFlag(Qt::ItemIsEditable, true);

        if (item->type() == ODItem::Type::TIndex)
        {
            if (item->index()->objectType() != Index::VAR)
            {
                if (index.column() == Value)
                {
                    flags.setFlag(Qt::ItemIsEditable, false);
                }
            }
        }
    }
    return flags;
}
