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

#include "nodeodfilterproxymodel.h"

#include "nodeoditemmodel.h"
#include "nodesubindex.h"

#include <QCollator>

NodeOdFilterProxyModel::NodeOdFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    _pdoFilter = PDOFILTER_ALL;
}

NodeOdFilterProxyModel::~NodeOdFilterProxyModel()
{
}

Node *NodeOdFilterProxyModel::node() const
{
    NodeOdItemModel *smodel = qobject_cast<NodeOdItemModel *>(sourceModel());
    if (smodel == nullptr)
    {
        return nullptr;
    }
    return smodel->node();
}

NodeIndex *NodeOdFilterProxyModel::nodeIndex(const QModelIndex &index) const
{
    NodeOdItemModel *smodel = qobject_cast<NodeOdItemModel *>(sourceModel());
    if (smodel == nullptr)
    {
        return nullptr;
    }
    return smodel->nodeIndex(mapToSource(index));
}

NodeSubIndex *NodeOdFilterProxyModel::nodeSubIndex(const QModelIndex &index) const
{
    NodeOdItemModel *smodel = qobject_cast<NodeOdItemModel *>(sourceModel());
    if (smodel == nullptr)
    {
        return nullptr;
    }
    return smodel->nodeSubIndex(mapToSource(index));
}

NodeOdFilterProxyModel::PDOFilter NodeOdFilterProxyModel::pdoFilter() const
{
    return _pdoFilter;
}

void NodeOdFilterProxyModel::setPdoFilter(NodeOdFilterProxyModel::PDOFilter pdoFilter)
{
    _pdoFilter = pdoFilter;
    invalidateFilter();
}

bool NodeOdFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    NodeOdItemModel *smodel = qobject_cast<NodeOdItemModel *>(sourceModel());
    if (smodel == nullptr)
    {
        return false;
    }

    // limit filter to index only and not subindex
    NodeIndex *nodeIndex = smodel->nodeIndex(source_parent);
    if (nodeIndex != nullptr)
    {
        return true;
    }

    // PDO filter
    nodeIndex = smodel->nodeIndex(smodel->index(source_row, 0, source_parent));
    bool pdoOk = false;
    switch (_pdoFilter)
    {
        case NodeOdFilterProxyModel::PDOFILTER_ALL:
            pdoOk = true;
            break;

        case NodeOdFilterProxyModel::PDOFILTER_PDO:
            for (NodeSubIndex *subIndex : nodeIndex->subIndexes())
            {
                if (subIndex->hasRPDOAccess() || subIndex->hasTPDOAccess())
                {
                    pdoOk = true;
                }
            }
            break;

        case NodeOdFilterProxyModel::PDOFILTER_RPDO:
            for (NodeSubIndex *subIndex : nodeIndex->subIndexes())
            {
                if (subIndex->hasRPDOAccess())
                {
                    pdoOk = true;
                }
            }
            break;

        case NodeOdFilterProxyModel::PDOFILTER_TPDO:
            for (NodeSubIndex *subIndex : nodeIndex->subIndexes())
            {
                if (subIndex->hasTPDOAccess())
                {
                    pdoOk = true;
                }
            }
            break;
    }
    if (!pdoOk)
    {
        return false;
    }

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

bool NodeOdFilterProxyModel::filterAcceptsColumn(int source_column, const QModelIndex &source_parent) const
{
    Q_UNUSED(source_column)
    Q_UNUSED(source_parent)
    return true;
}

bool NodeOdFilterProxyModel::lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const
{
    NodeOdItemModel *smodel = qobject_cast<NodeOdItemModel *>(sourceModel());
    if (smodel == nullptr)
    {
        return false;
    }

    QCollator collator;
    QVariant l = (source_left.model() != nullptr) ? source_left.model()->data(source_left, sortRole()) : QVariant();
    QVariant r = (source_right.model() != nullptr) ? source_right.model()->data(source_right, sortRole()) : QVariant();
    return collator.compare(l.toString(), r.toString()) < 0;
}
