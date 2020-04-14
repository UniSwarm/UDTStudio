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

#include "nodeoditem.h"

#include <QDebug>

#include "nodeoditemmodel.h"
#include "node.h"

NodeOdItem::NodeOdItem(NodeOd *od, NodeOdItem *parent)
{
    _type = TOD;
    _od = od;
    _index = nullptr;
    _subIndex = nullptr;
    _parent = parent;
    createChildren();
}

NodeOdItem::NodeOdItem(NodeIndex *index, NodeOdItem *parent)
{
    _type = TIndex;
    _od = nullptr;
    _index = index;
    _subIndex = nullptr;
    _parent = parent;
    createChildren();
}

NodeOdItem::NodeOdItem(NodeSubIndex *subIndex, NodeOdItem *parent)
{
    _type = TSubIndex;
    _od = nullptr;
    _index = nullptr;
    _subIndex = subIndex;
    _parent = parent;
}

NodeOdItem::~NodeOdItem()
{
    qDeleteAll(_children);
}

NodeOdItem::Type NodeOdItem::type() const
{
    return _type;
}

NodeOd *NodeOdItem::od() const
{
    return _od;
}

NodeIndex *NodeOdItem::index() const
{
    return _index;
}

NodeSubIndex *NodeOdItem::subIndex() const
{
    return _subIndex;
}

int NodeOdItem::rowCount() const
{
    switch (_type)
    {
    case NodeOdItem::TOD:
        return _od->indexCount();

    case NodeOdItem::TIndex:
        if (_index->objectType() == NodeIndex::VAR)
        {
            return 0;
        }
        else
        {
            return _index->subIndexes().count();
        }

    default:
        return 0;
    }
}

QVariant NodeOdItem::data(int column, int role) const
{
    switch (_type)
    {
    case NodeOdItem::TOD:
        break;

    case NodeOdItem::TIndex:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case NodeOdItemModel::OdIndex:
                return QVariant(QLatin1String("0x") + QString::number(_index->index(), 16).toUpper().rightJustified(4, '0'));

            case NodeOdItemModel::Name:
                return QVariant(_index->name());

            case NodeOdItemModel::Type:
                if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1)
                {
                    return QVariant(NodeSubIndex::dataTypeStr(_index->subIndex(0)->dataType()));
                }
                else
                {
                    return QVariant(NodeIndex::objectTypeStr(_index->objectType()));
                }

            case NodeOdItemModel::Acces:
                if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                {
                    return QVariant(_index->subIndex(0)->accessString());
                }
                break;

            case NodeOdItemModel::Value:
                if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                {
                    QVariant value = _index->subIndex(0)->value();
                    if (QMetaType::Type(value.type()) == QMetaType::UChar
                        || QMetaType::Type(value.type()) == QMetaType::SChar
                        || QMetaType::Type(value.type()) == QMetaType::QChar)
                    {
                        value.convert(QMetaType::Int);
                    }
                    return value;
                }
                else
                {
                    return QVariant(QString("%1 items").arg(_index->subIndexesCount()));
                }
            }
            break;

        case Qt::EditRole:
            switch (column)
            {
            case NodeOdItemModel::Value:
                if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                {
                    QVariant value = _index->subIndex(0)->value();
                    if (QMetaType::Type(value.type()) == QMetaType::UChar
                        || QMetaType::Type(value.type()) == QMetaType::SChar
                        || QMetaType::Type(value.type()) == QMetaType::QChar)
                    {
                        value.convert(QMetaType::Int);
                    }
                    return value;
                }
                break;
            }
            break;

        case Qt::TextAlignmentRole:
            switch (column)
            {
            case NodeOdItemModel::OdIndex:
                return QVariant(Qt::AlignRight);
            }
            break;
        }
        break;

    case NodeOdItem::TSubIndex:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case NodeOdItemModel::OdIndex:
                return QVariant(QLatin1String("0x") + QString::number(_subIndex->subIndex(), 16).toUpper().rightJustified(2, '0'));

            case NodeOdItemModel::Name:
                return QVariant(_subIndex->name());

            case NodeOdItemModel::Type:
                return QVariant(NodeSubIndex::dataTypeStr(_subIndex->dataType()));

            case NodeOdItemModel::Acces:
                return QVariant(_subIndex->accessString());

            case NodeOdItemModel::Value:
                QVariant value = _subIndex->value();
                if (QMetaType::Type(value.type()) == QMetaType::UChar
                    || QMetaType::Type(value.type()) == QMetaType::SChar
                    || QMetaType::Type(value.type()) == QMetaType::QChar)
                {
                    value.convert(QMetaType::Int);
                }
                return value;
            }
            break;

        case Qt::EditRole:
            switch (column)
            {
            case NodeOdItemModel::Value:
                QVariant value = _subIndex->value();
                if (QMetaType::Type(value.type()) == QMetaType::UChar
                    || QMetaType::Type(value.type()) == QMetaType::SChar
                    || QMetaType::Type(value.type()) == QMetaType::QChar)
                {
                    value.convert(QMetaType::Int);
                }
                return value;
            }
            break;

        case Qt::TextAlignmentRole:
            switch (column)
            {
            case NodeOdItemModel::OdIndex:
                return QVariant(Qt::AlignRight);
            }
            break;
        }
        break;
    }
    return QVariant();
}

bool NodeOdItem::setData(int column, const QVariant &value, int role, Node *node)
{
    Q_UNUSED(role)
    switch (_type)
    {
    case NodeOdItem::TOD:
        break;

    case NodeOdItem::TIndex:
        switch (column)
        {
        case NodeOdItemModel::Value:
            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
            {
                if (_index->subIndex(0)->isWritable())
                {
                    node->writeObject(_index->index(), _index->subIndex(0)->subIndex(), value);
                }
            }
            break;
        }
        break;

    case NodeOdItem::TSubIndex:
        switch (column)
        {
        case NodeOdItemModel::Value:
            if (_subIndex->isWritable())
            {
                node->writeObject(_subIndex->index(), _subIndex->subIndex(), value);
            }
            break;
        }
        break;
    }
    return false;
}

Qt::ItemFlags NodeOdItem::flags(int column) const
{
    Qt::ItemFlags flags = Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsDragEnabled;

    switch (_type)
    {
    case NodeOdItem::TOD:
        break;

    case NodeOdItem::TIndex:
        switch (column)
        {
        case NodeOdItemModel::Value:
            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
            {
                if (_index->subIndex(0)->isWritable())
                {
                    flags.setFlag(Qt::ItemIsEditable);
                }
            }
            break;
        }
        break;

    case NodeOdItem::TSubIndex:
        switch (column)
        {
        case NodeOdItemModel::Value:
            if (_subIndex->isWritable())
            {
                flags.setFlag(Qt::ItemIsEditable);
            }
            break;
        }
        break;
    }
    return flags;
}

NodeOdItem *NodeOdItem::parent() const
{
    return _parent;
}

NodeOdItem *NodeOdItem::child(int row) const
{
    NodeOdItem *child;
    if (row < 0 || row >= _children.count())
    {
        return nullptr;
    }
    child = _children.at(row);
    return child;
}

NodeOdItem *NodeOdItem::childIndex(quint16 index) const
{
    return _childrenMap.value(index);
}

int NodeOdItem::row() const
{
    if (!_parent)
    {
        return 0;
    }
    return _parent->_children.indexOf(const_cast<NodeOdItem *>(this));
}

NodeObjectId NodeOdItem::objectId() const
{
    switch (_type)
    {
    case NodeOdItem::TOD:
        return NodeObjectId(_od->node()->busId(), _od->node()->nodeId(), 0xFFFF, 0xFF);

    case NodeOdItem::TIndex:
        return _index->objectId();

    case NodeOdItem::TSubIndex:
        return _subIndex->objectId();
    }
    return NodeObjectId();
}

QString NodeOdItem::mimeData() const
{
    switch (_type)
    {
    case NodeOdItem::TOD:
        return QString();

    case NodeOdItem::TIndex:
        if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
        {
            return _index->subIndex(0)->objectId().mimeData();
        }
        else
        {
            return _index->objectId().mimeData();
        }

    case NodeOdItem::TSubIndex:
        return _subIndex->objectId().mimeData();
    }
    return QString();
}

void NodeOdItem::addChild(quint16 index, NodeOdItem *child)
{
    _children.append(child);
    _childrenMap.insert(index, child);
}

const QList<NodeOdItem *> &NodeOdItem::children() const
{
    return _children;
}

void NodeOdItem::createChildren()
{
    switch (_type)
    {
    case NodeOdItem::TOD:
        for (NodeIndex *index : _od->indexes())
        {
            addChild(index->index(), new NodeOdItem(index, this));
        }
        break;

    case NodeOdItem::TIndex:
        for (NodeSubIndex *subIndex : _index->subIndexes())
        {
            addChild(subIndex->subIndex(), new NodeOdItem(subIndex, this));
        }
        break;

    default:
        break;
    }
}
