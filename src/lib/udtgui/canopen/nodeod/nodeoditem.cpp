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

#include "nodeoditem.h"

#include <QFont>

#include "node.h"
#include "nodeoditemmodel.h"

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
                case Qt::UserRole:
                    return QVariant(_index->index());

                case Qt::DisplayRole:
                    switch (column)
                    {
                        case NodeOdItemModel::OdIndex:
                            return QVariant(QStringLiteral("0x") + QString::number(_index->index(), 16).toUpper().rightJustified(4, '0'));

                        case NodeOdItemModel::Name:
                            return QVariant(_index->name());

                        case NodeOdItemModel::Type:
                            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1)
                            {
                                if (_index->subIndex(0)->isQ1516())
                                {
                                    return QVariant(QStringLiteral("Q1516"));
                                }
                                return QVariant(NodeSubIndex::dataTypeStr(_index->subIndex(0)->dataType()));
                            }
                            else
                            {
                                return QVariant(
                                    QStringLiteral("%1[%2]").arg(NodeIndex::objectTypeStr(_index->objectType())).arg(_index->subIndexesCount() - 1));
                            }

                        case NodeOdItemModel::Access:
                            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                            {
                                return QVariant(_index->subIndex(0)->accessString());
                            }
                            break;

                        case NodeOdItemModel::Value:
                            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                            {
                                QVariant value = formatValue(_index->subIndex(0), ViewValue);
                                if (_index->subIndex(0)->error() != 0)
                                {
                                    return "*" + value.toString();
                                }
                                return value;
                            }
                            break;

                        case NodeOdItemModel::RawValue:
                            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                            {
                                return formatValue(_index->subIndex(0), ViewRawValue);
                            }
                            break;

                        case NodeOdItemModel::HexValue:
                            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                            {
                                return formatValue(_index->subIndex(0), ViewHex);
                            }
                            break;
                    }
                    break;

                case Qt::EditRole:
                    switch (column)
                    {
                        case NodeOdItemModel::Value:
                            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                            {
                                QVariant value = formatValue(_index->subIndex(0), EditValue);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                                value.convert(QMetaType::QString);
#else
                                value.convert(QMetaType(QMetaType::QString));
#endif
                                return value;
                            }
                            break;

                        case NodeOdItemModel::RawValue:
                            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                            {
                                QVariant value = formatValue(_index->subIndex(0), EditRawValue);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                                value.convert(QMetaType::QString);
#else
                                value.convert(QMetaType(QMetaType::QString));
#endif
                                return value;
                            }
                            break;

                        case NodeOdItemModel::HexValue:
                            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                            {
                                QVariant value = formatValue(_index->subIndex(0), EditHex);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                                value.convert(QMetaType::QString);
#else
                                value.convert(QMetaType(QMetaType::QString));
#endif
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

                        case NodeOdItemModel::Value:
                        case NodeOdItemModel::RawValue:
                            if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                            {
                                if (_index->subIndex(0)->isNumeric())
                                {
                                    return QVariant(Qt::AlignRight | Qt::AlignVCenter);
                                }
                            }
                    }
                    break;

                case Qt::FontRole:
                    if (_index->objectType() == NodeIndex::VAR && _index->subIndexesCount() == 1 && _index->subIndexExist(0))
                    {
                        if (_index->subIndex(0)->error() != 0U)
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
                case Qt::UserRole:
                    return QVariant(_subIndex->subIndex());

                case Qt::DisplayRole:
                    switch (column)
                    {
                        case NodeOdItemModel::OdIndex:
                            return QVariant(QStringLiteral("0x") + QString::number(_subIndex->subIndex(), 16).toUpper().rightJustified(2, '0'));

                        case NodeOdItemModel::Name:
                            return QVariant(_subIndex->name());

                        case NodeOdItemModel::Type:
                            if (_subIndex->isQ1516())
                            {
                                return QVariant("  " + QStringLiteral("Q1516"));
                            }
                            else
                            {
                                return QVariant("  " + NodeSubIndex::dataTypeStr(_subIndex->dataType()));
                            }

                        case NodeOdItemModel::Access:
                            return QVariant(_subIndex->accessString());

                        case NodeOdItemModel::Value:
                        {
                            QVariant value = formatValue(_subIndex, ViewValue);
                            if (_subIndex->error() != 0)
                            {
                                return "*" + value.toString();
                            }
                            return value;
                        }

                        case NodeOdItemModel::RawValue:
                            return formatValue(_subIndex, ViewRawValue);

                        case NodeOdItemModel::HexValue:
                            return formatValue(_subIndex, ViewHex);
                    }
                    break;

                case Qt::EditRole:
                    switch (column)
                    {
                        case NodeOdItemModel::Value:
                        {
                            QVariant value = formatValue(_subIndex, EditValue);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                            value.convert(QMetaType::QString);
#else
                            value.convert(QMetaType(QMetaType::QString));
#endif
                            return value;
                        }

                        case NodeOdItemModel::RawValue:
                        {
                            QVariant value = formatValue(_subIndex, EditRawValue);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                            value.convert(QMetaType::QString);
#else
                            value.convert(QMetaType(QMetaType::QString));
#endif
                            return value;
                        }

                        case NodeOdItemModel::HexValue:
                        {
                            QVariant value = formatValue(_subIndex, EditHex);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
                            value.convert(QMetaType::QString);
#else
                            value.convert(QMetaType(QMetaType::QString));
#endif
                            return value;
                        }
                    }
                    break;

                case Qt::TextAlignmentRole:
                    switch (column)
                    {
                        case NodeOdItemModel::OdIndex:
                            return QVariant(Qt::AlignRight | Qt::AlignVCenter);

                        case NodeOdItemModel::Value:
                        case NodeOdItemModel::RawValue:
                            if (_subIndex->isNumeric())
                            {
                                return QVariant(Qt::AlignRight | Qt::AlignVCenter);
                            }
                            return QVariant();
                    }
                    break;

                case Qt::FontRole:
                    if (_subIndex->error() != 0U)
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
    QVariant valueToWrite;

    switch ((NodeOdItemModel::Column)column)
    {
        case NodeOdItemModel::Value:
            valueToWrite = value;
            break;

        case NodeOdItemModel::RawValue:
            valueToWrite = value;
            break;

        case NodeOdItemModel::HexValue:
            if (!value.toString().startsWith(QStringLiteral("0x")))
            {
                valueToWrite = QVariant("0x" + value.toString());
            }
            else
            {
                valueToWrite = value;
            }
            break;

        default:
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
    if (subIndex == nullptr)
    {
        return false;
    }

    if (valueToWrite.toString().endsWith(subIndex->unit()))
    {
        valueToWrite = valueToWrite.toString().mid(0, valueToWrite.toString().size() - subIndex->unit().size());
    }

    if (subIndex->isNumeric())
    {
        QString valueStr = valueToWrite.toString();
        if (valueStr.startsWith(QStringLiteral("0x"), Qt::CaseInsensitive))
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
        else if (column == NodeOdItemModel::Value)
        {
            double valueDouble = valueToWrite.toDouble();
            if (subIndex->isQ1516())
            {
                valueDouble *= 65536.0;
            }
            if (subIndex->scale() != 0)
            {
                valueDouble /= subIndex->scale();
            }
            valueToWrite.setValue(valueDouble);
        }
        else
        {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
            valueToWrite.convert(QMetaType::QReal);
#else
            valueToWrite.convert(QMetaType(QMetaType::QReal));
#endif
        }
    }

    node->writeObject(subIndex->index(), subIndex->subIndex(), valueToWrite);

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
                case NodeOdItemModel::HexValue:
                case NodeOdItemModel::RawValue:
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
                case NodeOdItemModel::HexValue:
                case NodeOdItemModel::RawValue:
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
    if (_parent == nullptr)
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

QVariant NodeOdItem::formatValue(NodeSubIndex *subIndex, NodeOdItem::ViewType viewType) const
{
    QVariant mvalue = subIndex->value();
    int zero = 0;
    bool sign = true;
    bool floatingPoint = false;
    bool string = false;

    switch (subIndex->dataType())
    {
        case NodeSubIndex::BOOLEAN:
            if (mvalue.toInt() == 1)
            {
                return QVariant(true);
            }
            else
            {
                return QVariant(false);
            }

        case NodeSubIndex::UNSIGNED8:
            sign = false;
            zero = 2;
            break;

        case NodeSubIndex::INTEGER8:
            zero = 2;
            break;

        case NodeSubIndex::UNSIGNED16:
            sign = false;
            zero = 4;
            break;

        case NodeSubIndex::INTEGER16:
            zero = 4;
            break;

        case NodeSubIndex::UNSIGNED24:
            sign = false;
            zero = 6;
            break;

        case NodeSubIndex::INTEGER24:
            zero = 6;
            break;

        case NodeSubIndex::UNSIGNED32:
            sign = false;
            zero = 8;
            break;

        case NodeSubIndex::INTEGER32:
            zero = 8;
            break;

        case NodeSubIndex::UNSIGNED40:
            sign = false;
            zero = 10;
            break;

        case NodeSubIndex::INTEGER40:
            zero = 10;
            break;

        case NodeSubIndex::UNSIGNED48:
            sign = false;
            zero = 12;
            break;

        case NodeSubIndex::INTEGER48:
            zero = 12;
            break;

        case NodeSubIndex::UNSIGNED56:
            sign = false;
            zero = 14;
            break;

        case NodeSubIndex::INTEGER56:
            zero = 14;
            break;

        case NodeSubIndex::UNSIGNED64:
            sign = false;
            zero = 16;
            break;

        case NodeSubIndex::INTEGER64:
            zero = 16;
            break;

        case NodeSubIndex::REAL32:
            zero = 8;
            floatingPoint = true;
            break;

        case NodeSubIndex::REAL64:
            zero = 16;
            floatingPoint = true;
            break;

        case NodeSubIndex::VISIBLE_STRING:
        case NodeSubIndex::OCTET_STRING:
        case NodeSubIndex::UNICODE_STRING:
            string = true;
            break;

        default:
            return mvalue;
    }

    QString rawValueStr;
    QString valueStr;
    QString hexStr;

    if (string)
    {
        QString str = mvalue.toString();
        QByteArray strArray;
        strArray.append(str.toUtf8());
        zero = str.size() * 2;
        rawValueStr = "\"" + str + "\"";
        hexStr = "0x" + strArray.toHex().toUpper();
        valueStr = rawValueStr;
    }
    else if (floatingPoint)
    {
        qreal real = mvalue.toReal();
        rawValueStr = formatDouble(mvalue.toReal() * subIndex->scale());

        valueStr = formatDouble(mvalue.toReal() * subIndex->scale());

        if (zero == 8)
        {
            float fval = mvalue.toReal();
            const quint32 *i = reinterpret_cast<const quint32 *>(&fval);
            hexStr = "0x" + QString::number(*i, 16).rightJustified(zero, '0').toUpper();
        }
        else
        {
            const quint64 *i = reinterpret_cast<const quint64 *>(&real);
            hexStr = "0x" + QString::number(*i, 16).rightJustified(zero, '0').toUpper();
        }
    }
    else
    {
        if (sign)
        {
            rawValueStr = QString::number(mvalue.toInt());
            if (subIndex->isQ1516())
            {
                valueStr = formatDouble(static_cast<double>(mvalue.toInt()) / 65536.0 * subIndex->scale());
                rawValueStr = QString::number(mvalue.toInt());
            }
            else
            {
                if (subIndex->scale() != 1.0 && subIndex->scale() != 0.0)
                {
                    valueStr = formatDouble(mvalue.toReal() * subIndex->scale());
                }
                else
                {
                    valueStr = QString::number(mvalue.toInt());
                }
            }
            hexStr = "0x" + QString::number(mvalue.toInt(), 16).rightJustified(zero, '0').right(zero).toUpper();
        }
        else
        {
            rawValueStr = QString::number(mvalue.toUInt());
            if (subIndex->scale() != 1.0 && subIndex->scale() != 0.0)
            {
                valueStr = formatDouble(mvalue.toReal() * subIndex->scale());
            }
            else
            {
                valueStr = QString::number(mvalue.toUInt());
            }
            hexStr = "0x" + QString::number(mvalue.toUInt(), 16).rightJustified(zero, '0').toUpper();
        }
    }

    switch (viewType)
    {
        case NodeOdItem::ViewValue:
            valueStr += subIndex->unit();
            return QVariant(valueStr);

        case NodeOdItem::EditValue:
            return QVariant(valueStr);

        case NodeOdItem::EditRawValue:
        case NodeOdItem::ViewRawValue:
            return QVariant(rawValueStr);

        case NodeOdItem::EditHex:
        case NodeOdItem::ViewHex:
            return QVariant(hexStr);

        case NodeOdItem::ViewHybrid:
            return QVariant(QStringLiteral("%1 (%2)").arg(valueStr, hexStr));
    }
    return QVariant();
}

QString NodeOdItem::formatDouble(double value) const
{
    QString str = QString::number(value, 'g', 10);
    if (!str.contains('.'))
    {
        str.append(QStringLiteral(".0"));
    }
    return str;
}
