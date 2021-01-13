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
#include <QFont>

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
                    return QVariant(QString("%1[%2]").arg(NodeIndex::objectTypeStr(_index->objectType())).arg(_index->subIndexesCount()));
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
                    QVariant value = formatValue(_index->subIndex(0)->value(), _index->subIndex(0)->dataType());
                    if (_index->subIndex(0)->error() != 0)
                    {
                        return "*" + value.toString();
                    }
                    return value;
                }
            }
            break;

        case Qt::EditRole:
            switch (column)
            {
            case NodeOdItemModel::Value:
                if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                {
                    QVariant value = formatEditValue(_index->subIndex(0)->value());
                    value.convert(QMetaType::QString);
                    return value;
                }
                break;
            }
            break;

        case Qt::TextAlignmentRole:
            switch (column)
            {
            case NodeOdItemModel::OdIndex:
                return QVariant(Qt::AlignRight | Qt::AlignVCenter);
            }
            break;

        case Qt::FontRole:
            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
            {
                if (_index->subIndex(0)->error())
                {
                    QFont font;
                    font.setItalic(true);
                    return font;
                }
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
                QVariant value = formatValue(_subIndex->value(), _subIndex->dataType());
                if (_subIndex->error() != 0)
                {
                    return "*" + value.toString();
                }
                return value;
            }
            break;

        case Qt::EditRole:
            switch (column)
            {
            case NodeOdItemModel::Value:
                QVariant value = formatEditValue(_subIndex->value());
                value.convert(QMetaType::QString);
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

        case Qt::FontRole:
            if (_subIndex->error())
            {
                QFont font;
                font.setItalic(true);
                return font;
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

    if (column != NodeOdItemModel::Value)
    {
        return false;
    }

    NodeSubIndex *subIndex = nullptr;

    switch (_type)
    {
    case TOD:
        break;

    case NodeOdItem::TIndex:
        if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
        {
            if (_index->subIndex(0)->isWritable())
            {
                subIndex = _index->subIndex(0);
            }
        }
        break;

    case NodeOdItem::TSubIndex:
        if (_subIndex->isWritable())
        {
            subIndex = _subIndex;
        }
        break;
    }

    if (subIndex)
    {
        QVariant valueToWrite = value;

        if (subIndex->isNumeric())
        {
            QString valueStr = valueToWrite.toString();
            if (valueStr.startsWith("0x", Qt::CaseInsensitive))
            {
                bool ok;
                valueStr = valueStr.mid(2);
                switch (subIndex->dataType())
                {
                case NodeSubIndex::BOOLEAN:
                case NodeSubIndex::INTEGER8:
                case NodeSubIndex::INTEGER16:
                case NodeSubIndex::INTEGER24:
                case NodeSubIndex::INTEGER32:
                case NodeSubIndex::INTEGER40:
                case NodeSubIndex::INTEGER48:
                case NodeSubIndex::INTEGER56:
                case NodeSubIndex::INTEGER64:
                    valueToWrite.setValue(valueStr.toInt(&ok, 16));
                    break;

                case NodeSubIndex::UNSIGNED8:
                case NodeSubIndex::UNSIGNED16:
                case NodeSubIndex::UNSIGNED24:
                case NodeSubIndex::UNSIGNED32:
                case NodeSubIndex::UNSIGNED40:
                case NodeSubIndex::UNSIGNED48:
                case NodeSubIndex::UNSIGNED56:
                case NodeSubIndex::UNSIGNED64:
                    valueToWrite.setValue(valueStr.toUInt(&ok, 16));
                    break;

                default:
                    break;
                }
            }
        }

        node->writeObject(subIndex->index(), subIndex->subIndex(), valueToWrite);
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

QVariant NodeOdItem::formatValue(const QVariant &value, NodeSubIndex::DataType dataType) const
{
    QVariant mvalue = value;
    int zero = 0;
    bool sign = true;
    bool floatingPoint = false;
    switch (dataType)
    {
        case NodeSubIndex::BOOLEAN:
            if (value.toInt() == 1)
            {
                return QVariant(true);
            }
            else
            {
                return QVariant(false);
            }

        case NodeSubIndex::UNSIGNED8:
            sign = false;
        case NodeSubIndex::INTEGER8:
            zero = 2;
            break;

        case NodeSubIndex::UNSIGNED16:
            sign = false;
        case NodeSubIndex::INTEGER16:
            zero = 4;
            break;

        case NodeSubIndex::UNSIGNED24:
            sign = false;
        case NodeSubIndex::INTEGER24:
            zero = 6;
            break;

        case NodeSubIndex::UNSIGNED32:
            sign = false;
        case NodeSubIndex::INTEGER32:
            zero = 8;
            break;

        case NodeSubIndex::UNSIGNED40:
            sign = false;
        case NodeSubIndex::INTEGER40:
            zero = 10;
            break;

        case NodeSubIndex::UNSIGNED48:
            sign = false;
        case NodeSubIndex::INTEGER48:
            zero = 12;
            break;

        case NodeSubIndex::UNSIGNED56:
            sign = false;
        case NodeSubIndex::INTEGER56:
            zero = 14;
            break;

        case NodeSubIndex::UNSIGNED64:
            sign = false;
        case NodeSubIndex::INTEGER64:
            zero = 16;
            break;

        case NodeSubIndex::REAL32:
        case NodeSubIndex::REAL64:
            zero = 16;
            floatingPoint = true;
            break;

    default:
        return value;
    }

    if (floatingPoint)
    {
        return QVariant(QString("%1").arg(mvalue.toReal(), 0, 'f'));
    }
    else
    {
        if (sign)
        {
            return QVariant(QString("%1 (0x%2)").arg(mvalue.toInt()).arg(QString::number(mvalue.toInt(), 16).rightJustified(zero, '0').toUpper()));
        }
        else
        {
            return QVariant(QString("%1 (0x%2)").arg(mvalue.toUInt()).arg(QString::number(mvalue.toUInt(), 16).rightJustified(zero, '0').toUpper()));
        }
    }
}

QVariant NodeOdItem::formatEditValue(const QVariant &value) const
{
    QVariant mvalue = value;
    switch (QMetaType::Type(value.type()))
    {
    case QMetaType::UChar:
    case QMetaType::UShort:
    case QMetaType::UInt:
    case QMetaType::ULongLong:
    case QMetaType::Double:
        return QVariant(QString("%1").arg(mvalue.toUInt()));

    case QMetaType::SChar:
    case QMetaType::Short:
    case QMetaType::Int:
    case QMetaType::LongLong:
        return QVariant(QString("%1").arg(mvalue.toInt()));

    default:
        return value;
    }
}
