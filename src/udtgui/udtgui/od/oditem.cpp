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

#include "oditem.h"

#include "oditemmodel.h"

ODItem::ODItem(OD *od, ODItem *parent)
{
    _od = od;
    _index = nullptr;
    _subIndex = nullptr;
    _type = TOD;
    _parent = parent;
    createChildren();
}

ODItem::ODItem(Index *index, ODItem *parent)
{
    _od = nullptr;
    _index = index;
    _subIndex = nullptr;
    _type = TIndex;
    _parent = parent;
    createChildren();
}

ODItem::ODItem(SubIndex *subIndex, ODItem *parent)
{
    _od = nullptr;
    _index = nullptr;
    _subIndex = subIndex;
    _type = TSubIndex;
    _parent = parent;
    createChildren();
}

ODItem::~ODItem()
{
    qDeleteAll(_children);
}

ODItem::Type ODItem::type() const
{
    return _type;
}

Index *ODItem::index() const
{
    return _index;
}

SubIndex *ODItem::subIndex() const
{
    return _subIndex;
}

int ODItem::rowCount() const
{
    switch (_type)
    {
    case ODItem::TOD:
        return _od->indexCount();

    case ODItem::TIndex:
        if (_index->objectType() == Index::VAR)
            return 0;
        else
            return _index->subIndexes().count();

    default:
        return 0;
    }
}

QVariant ODItem::data(int column, int role) const
{
    switch (_type)
    {
    case ODItem::TOD:
        break;
    case ODItem::TIndex:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case ODItemModel::OdIndex:
                return QVariant(QLatin1String("0x") + QString::number(index()->index(), 16).toUpper());
            case ODItemModel::Name:
                return QVariant(index()->name());
            case ODItemModel::Type:
                if (_index->objectType() == Index::VAR && index()->subIndexesCount() == 1)
                    return QVariant(DataStorage::dataTypeStr(index()->subIndex(0)->data().dataType()));
                else
                    return QVariant(Index::objectTypeStr(index()->objectType()));
            case ODItemModel::Value:
                if (_index->objectType() == Index::VAR && index()->subIndexesCount() == 1)
                    return index()->subIndex(0)->data().value();
                else
                    return QVariant(QString("%1 items").arg(index()->subIndexesCount()));
            default:
                return QVariant();
            }
        }
        break;
    case ODItem::TSubIndex:
        switch (role)
        {
        case Qt::DisplayRole:
            switch (column)
            {
            case ODItemModel::OdIndex:
                return QVariant(QLatin1String("0x") + QString::number(subIndex()->subIndex(), 16).toUpper());
            case ODItemModel::Name:
                return QVariant(subIndex()->name());
            case ODItemModel::Type:
                return QVariant(DataStorage::dataTypeStr(subIndex()->data().dataType()));
            case ODItemModel::Value:
                return subIndex()->data().value();
            default:
                return QVariant();
            }
        }
        break;

    }
    return QVariant();
}

ODItem *ODItem::parent() const
{
    return _parent;
}

ODItem *ODItem::child(int id) const
{
    ODItem *child;
    if (id < 0 || id >= _children.count())
        return nullptr;
    child = _children.at(id);
    return child;
}

int ODItem::row() const
{
    if (!_parent)
        return 0;
    return _parent->_children.indexOf(const_cast<ODItem *>(this));
}

void ODItem::createChildren()
{
    for (int i=0; i<rowCount(); i++)
    {
        switch (_type)
        {
        case ODItem::TOD:
            _children.append(new ODItem(od()->indexes().values()[i], this));
            break;
        case ODItem::TIndex:
            _children.append(new ODItem(index()->subIndexes().values()[i], this));
            break;
        default:
            break;
        }
    }
}

OD *ODItem::od() const
{
    return _od;
}
