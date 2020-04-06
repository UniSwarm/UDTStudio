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

#include "nodeodfilterproxymodel.h"

#include "nodeoditemmodel.h"
#include "nodesubindex.h"

NodeOdFilterProxyModel::NodeOdFilterProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
}

NodeOdFilterProxyModel::~NodeOdFilterProxyModel()
{
}

bool NodeOdFilterProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    NodeOdItemModel *smodel = dynamic_cast<NodeOdItemModel *>(sourceModel());
    if (!smodel)
    {
        return false;
    }

    // limit filter to index only and not subindex
    NodeIndex *nodeIndex = smodel->nodeIndex(source_parent);
    if (nodeIndex)
    {
        return true;
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
    NodeOdItemModel *smodel = dynamic_cast<NodeOdItemModel *>(sourceModel());
    if (!smodel)
    {
        return false;
    }

    // limit filter to index only and not subindex
    NodeSubIndex *nodeSubIndexLeft = smodel->nodeSubIndex(source_left);
    NodeSubIndex *nodeSubIndexRight = smodel->nodeSubIndex(source_right);
    if (nodeSubIndexLeft && nodeSubIndexRight)
    {
        return nodeSubIndexLeft->subIndex() < nodeSubIndexRight->subIndex();
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}