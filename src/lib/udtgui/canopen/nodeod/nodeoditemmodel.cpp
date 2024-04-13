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

#include "nodeoditemmodel.h"

#include <QMimeData>

#include "node.h"

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

NodeOdItem::Type NodeOdItemModel::typeIndex(const QModelIndex &index) const
{
    if (_root == nullptr)
    {
        return NodeOdItem::TOD;
    }
    if (!index.isValid())
    {
        return NodeOdItem::TOD;
    }

    NodeOdItem *item = static_cast<NodeOdItem *>(index.internalPointer());
    return item->type();
}

NodeIndex *NodeOdItemModel::nodeIndex(const QModelIndex &index) const
{
    if (_root == nullptr)
    {
        return nullptr;
    }
    if (!index.isValid())
    {
        return nullptr;
    }

    NodeOdItem *item = static_cast<NodeOdItem *>(index.internalPointer());
    if (item->type() == NodeOdItem::TIndex)
    {
        return item->index();
    }
    return nullptr;
}

NodeSubIndex *NodeOdItemModel::nodeSubIndex(const QModelIndex &index) const
{
    if (_root == nullptr)
    {
        return nullptr;
    }
    if (!index.isValid())
    {
        return nullptr;
    }

    NodeOdItem *item = static_cast<NodeOdItem *>(index.internalPointer());
    if (item->type() == NodeOdItem::TSubIndex)
    {
        return item->subIndex();
    }
    if (item->type() == NodeOdItem::TIndex)
    {
        NodeIndex *nodeIndex = item->index();
        if ((nodeIndex != nullptr) && nodeIndex->subIndexExist(0))
        {
            return nodeIndex->subIndex(0);
        }
    }
    return nullptr;
}

QModelIndex NodeOdItemModel::index(const NodeObjectId &objId)
{
    const QModelIndexList &list = match(this->index(0, 0, QModelIndex()), Qt::UserRole, objId.index());
    if (list.isEmpty())
    {
        return QModelIndex();
    }

    const QModelIndex &index = list.first();
    const QModelIndexList &listSub = match(this->index(0, 0, index), Qt::UserRole, objId.subIndex());
    if (listSub.isEmpty())
    {
        return index;
    }

    return listSub.first();
}

void NodeOdItemModel::setNode(Node *node)
{
    if (node == _node)
    {
        return;
    }

    beginResetModel();
    delete _root;

    if (_node != nullptr)
    {
        disconnect(_node, nullptr, this, nullptr);
    }

    _node = node;
    setNodeInterrest(_node);

    if (_node != nullptr)
    {
        _root = new NodeOdItem(_node->nodeOd());
        connect(_node,
                &QObject::destroyed,
                this,
                [=]()
                {
                    setNode(nullptr);
                });
        connect(_node,
                &Node::edsFileChanged,
                this,
                [=]()
                {
                    Node *newNode = node;
                    setNode(nullptr);
                    setNode(newNode);
                });
    }
    else
    {
        _root = nullptr;
    }
    endResetModel();
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

                case Access:
                    return QVariant(tr("Access"));

                case Value:
                    return QVariant(tr("Value"));

                case RawValue:
                    return QVariant(tr("Raw"));

                case HexValue:
                    return QVariant(tr("Hex"));
            }
            break;
    }
    return QVariant();
}

QModelIndex NodeOdItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (_root == nullptr)
    {
        return QModelIndex();
    }

    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    NodeOdItem *parentItem;
    if (parent.internalPointer() == nullptr)
    {
        parentItem = _root;
    }
    else
    {
        parentItem = static_cast<NodeOdItem *>(parent.internalPointer());
    }

    NodeOdItem *childItem = parentItem->child(row);
    if (childItem != nullptr)
    {
        return createIndex(row, column, childItem);
    }
    return QModelIndex();
}

QModelIndex NodeOdItemModel::parent(const QModelIndex &child) const
{
    if (!child.isValid())
    {
        return QModelIndex();
    }

    NodeOdItem *childItem = static_cast<NodeOdItem *>(child.internalPointer());
    NodeOdItem *parentItem = childItem->parent();

    if (parentItem == _root || (parentItem == nullptr))
    {
        return QModelIndex();
    }

    return createIndex(parentItem->row(), 0, parentItem);
}

int NodeOdItemModel::rowCount(const QModelIndex &parent) const
{
    if (_root == nullptr)
    {
        return 0;
    }

    NodeOdItem *parentItem;
    if (parent.internalPointer() == nullptr)
    {
        parentItem = _root;
    }
    else
    {
        parentItem = static_cast<NodeOdItem *>(parent.internalPointer());
    }

    return parentItem->rowCount();
}

QVariant NodeOdItemModel::data(const QModelIndex &index, int role) const
{
    if (_root == nullptr)
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

bool NodeOdItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (_root == nullptr)
    {
        return false;
    }
    if (!index.isValid())
    {
        return false;
    }
    if (!value.isValid())
    {
        return false;
    }
    if (value.toString().isEmpty())
    {
        return false;
    }

    NodeOdItem *item = static_cast<NodeOdItem *>(index.internalPointer());
    item->setData(index.column(), value, role, _node);
    return false;
}

Qt::ItemFlags NodeOdItemModel::flags(const QModelIndex &index) const
{
    if (_root == nullptr)
    {
        return Qt::NoItemFlags;
    }
    if (!index.isValid())
    {
        return Qt::ItemIsDropEnabled;
    }
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    NodeOdItem *item = static_cast<NodeOdItem *>(index.internalPointer());
    return item->flags(index.column());
}

QModelIndex NodeOdItemModel::indexItem(quint16 index, int col)
{
    if (_root == nullptr)
    {
        return QModelIndex();
    }

    NodeOdItem *childIndex = _root->childIndex(index);
    if (childIndex == nullptr)
    {
        return QModelIndex();
    }
    return createIndex(childIndex->row(), col, childIndex);
}

QModelIndex NodeOdItemModel::subIndexItem(quint16 index, quint8 subindex, int col)
{
    if (_root == nullptr)
    {
        return QModelIndex();
    }

    NodeOdItem *childIndex = _root->childIndex(index);
    if (childIndex == nullptr)
    {
        return QModelIndex();
    }
    if (childIndex->rowCount() == 0)
    {
        return createIndex(childIndex->row(), col, childIndex);
    }

    NodeOdItem *childSubIndex = childIndex->childIndex(subindex);
    if (childSubIndex == nullptr)
    {
        return QModelIndex();
    }
    return createIndex(childSubIndex->row(), col, childSubIndex);
}

void NodeOdItemModel::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    Q_UNUSED(flags)
    QModelIndex modelIndexStart = subIndexItem(objId.index(), objId.subIndex(), Value);
    QModelIndex modelIndexEnd = subIndexItem(objId.index(), objId.subIndex(), ColumnCount - 1);
    if (modelIndexStart.isValid() && modelIndexEnd.isValid())
    {
        emit dataChanged(modelIndexStart, modelIndexEnd);
    }
}

QStringList NodeOdItemModel::mimeTypes() const
{
    QStringList types;
    types << QStringLiteral("index/subindex");
    return types;
}

QMimeData *NodeOdItemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QByteArray encodedData;

    for (QModelIndex index : indexes)
    {
        if (index.isValid() && index.column() == OdIndex)
        {
            NodeOdItem *item = static_cast<NodeOdItem *>(index.internalPointer());
            if (item->type() == NodeOdItem::TIndex || item->type() == NodeOdItem::TSubIndex)
            {
                encodedData.append(QString(item->mimeData() + ":").toUtf8());
            }
        }
    }
    mimeData->setData(QStringLiteral("index/subindex"), encodedData);
    return mimeData;
}

bool NodeOdItemModel::canDropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(action)
    Q_UNUSED(row)
    Q_UNUSED(column)
    Q_UNUSED(parent)
    return mimeData->hasFormat(QStringLiteral("index/subindex"));
}

bool NodeOdItemModel::dropMimeData(const QMimeData *mimeData, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    Q_UNUSED(row)
    Q_UNUSED(column)
    Q_UNUSED(parent)
    if (action == Qt::IgnoreAction)
    {
        return true;
    }
    if (!mimeData->hasFormat(QStringLiteral("index/subindex")))
    {
        return false;
    }

    // TODO
    return true;
}

Qt::DropActions NodeOdItemModel::supportedDropActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}

Qt::DropActions NodeOdItemModel::supportedDragActions() const
{
    return Qt::MoveAction | Qt::CopyAction;
}
