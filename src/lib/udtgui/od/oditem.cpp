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

#include "oditem.h"

#include "oditemmodel.h"

ODItem::ODItem(DeviceModel *od, ODItem *parent)
{
    _deviceModel = od;
    _index = nullptr;
    _subIndex = nullptr;
    _type = TOD;
    _parent = parent;
    createChildren();
}

ODItem::ODItem(Index *index, ODItem *parent)
{
    if (parent->_deviceModel)
    {
        _deviceModel = parent->_deviceModel;
    }
    else
    {
        _deviceModel = nullptr;
    }
    _index = index;
    _subIndex = nullptr;
    _type = TIndex;
    _parent = parent;
    createChildren();
}

ODItem::ODItem(SubIndex *subIndex, ODItem *parent)
{
    if (parent->_deviceModel)
    {
        _deviceModel = parent->_deviceModel;
    }
    else
    {
        _deviceModel = nullptr;
    }
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

DeviceModel *ODItem::deviceModel() const
{
    return _deviceModel;
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
        return _deviceModel->indexCount();

    case ODItem::TIndex:
        if (_index->objectType() == Index::VAR)
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
                return QVariant(QLatin1String("0x") + QString::number(_index->index(), 16).toUpper());

            case ODItemModel::Name:
                return QVariant(_index->name());

            case ODItemModel::Type:
                if (_index->objectType() == Index::VAR && _index->subIndexesCount() == 1)
                {
                    return QVariant(SubIndex::dataTypeStr(_index->subIndex(0)->dataType()));
                }
                else
                {
                    return QVariant(Index::objectTypeStr(_index->objectType()));
                }

            case ODItemModel::Value:
                if (_index->objectType() == Index::VAR && _index->subIndexesCount() == 1)
                {
                    if (_index->subIndex(0)->hasNodeId() && _deviceModel->type() == DeviceModel::Description)
                    {
                        return QString("$NODEID+%1").arg(_index->subIndex(0)->value().toString());
                    }
                    else
                    {
                        return _index->subIndex(0)->value();
                    }
                }
                else
                {
                    return QVariant(QString("%1 items").arg(_index->subIndexesCount()));
                }
            default:
                return QVariant();
            }
        case Qt::EditRole:
            switch (column)
            {
            case ODItemModel::OdIndex:
                return QVariant(QLatin1String("0x") + QString::number(_index->index(), 16).toUpper());

            case ODItemModel::Name:
                return _index->name();

            case ODItemModel::Type:
                return _index->objectType();

            case ODItemModel::Value:
                if (_index->objectType() == Index::VAR && _index->subIndexesCount() == 1)
                {
                    return _index->subIndex(0)->value();
                }
                else
                {
                    return QVariant();
                }

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
                return QVariant(QLatin1String("0x") + QString::number(_subIndex->subIndex(), 16).toUpper());

            case ODItemModel::Name:
                return QVariant(_subIndex->name());

            case ODItemModel::Type:
                return QVariant(SubIndex::dataTypeStr(_subIndex->dataType()));

            case ODItemModel::Value:
                if (_subIndex->hasNodeId() && _deviceModel->type() == DeviceModel::Description)
                {
                    return QString("$NODEID+%1").arg(_subIndex->value().toString());
                }
                else
                {
                    return _subIndex->value();
                }

            default:
                return QVariant();
            }
        case Qt::EditRole:
            switch (column)
            {
            case ODItemModel::OdIndex:
                return _subIndex->subIndex();

            case ODItemModel::Name:
                return _subIndex->name();

            case ODItemModel::Type:
                return _subIndex->dataType();

            case ODItemModel::Value:
                return _subIndex->value();

            default:
                return QVariant();
            }
        }
        break;
    }
    return QVariant();
}

bool ODItem::setData(int column, const QVariant &value, int role)
{
    switch (_type)
    {
    case ODItem::TOD:
        break;

    case ODItem::TIndex:
        switch (role)
        {
        case Qt::EditRole:
            switch (column)
            {
            case ODItemModel::OdIndex:
                if (value.toString().startsWith("0x", Qt::CaseInsensitive))
                {
                    bool ok;
                    index()->setIndex(static_cast<uint16_t>(value.toString().toInt(&ok, 16)));
                    if (!ok)
                    {
                        return false;
                    }
                }
                else
                {
                    bool ok;
                    index()->setIndex(static_cast<uint16_t>(value.toString().toInt(&ok, 10)));
                    if (!ok)
                    {
                        return false;
                    }
                }
                return true;

            case ODItemModel::Name:
                index()->setName(value.toString());
                return true;

            case ODItemModel::Type:
                if (_index->objectType() == Index::VAR && index()->subIndexesCount() == 1)
                {
                    index()->subIndex(0)->setDataType(static_cast<SubIndex::DataType>(value.toInt()));
                }
                else
                {
                    index()->setObjectType(value.toInt());
                }
                return true;

            case ODItemModel::Value:
                if (_index->objectType() == Index::VAR && index()->subIndexesCount() == 1)
                {
                    index()->subIndex(0)->setValue(value);
                    return true;
                }
                else
                {
                    return false;
                }

            default:
                return false;
            }
        }
        break;

    case ODItem::TSubIndex:
        switch (role)
        {
        case Qt::EditRole:
            switch (column)
            {
            case ODItemModel::OdIndex:
                _subIndex->setSubIndex(value.toInt());
                return true;

            case ODItemModel::Name:
                _subIndex->setName(value.toString());
                return true;

            case ODItemModel::Type:
                _subIndex->setDataType(static_cast<SubIndex::DataType>(value.toInt()));
                return true;

            case ODItemModel::Value:
                _subIndex->setValue(value);
                return true;

            default:
                return false;
            }
        }
        break;
    }
    return false;
}

ODItem *ODItem::parent() const
{
    return _parent;
}

ODItem *ODItem::child(int id) const
{
    ODItem *child;
    if (id < 0 || id >= _children.count())
    {
        return nullptr;
    }
    child = _children.at(id);
    return child;
}

int ODItem::row() const
{
    if (!_parent)
    {
        return 0;
    }
    return _parent->_children.indexOf(const_cast<ODItem *>(this));
}

void ODItem::createChildren()
{
    for (int i = 0; i < rowCount(); i++)
    {
        switch (_type)
        {
        case ODItem::TOD:
            _children.append(new ODItem(_deviceModel->indexes().values()[i], this));
            break;

        case ODItem::TIndex:
            _children.append(new ODItem(_index->subIndexes().values()[i], this));
            break;

        default:
            break;
        }
    }
}
