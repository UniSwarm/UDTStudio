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

#include "nodeodtreeview.h"

#include <QKeyEvent>

#include "node.h"

NodeOdTreeView::NodeOdTreeView(QWidget *parent)
    : QTreeView(parent)
{
    _odModel = new NodeOdItemModel();
    _odModelSorter = new QSortFilterProxyModel(this);
    _odModelSorter->setSourceModel(_odModel);
    setModel(_odModelSorter);

    //_odModelSorter->setFilterRegularExpression(QRegularExpression("0x64[0-9]{2}"));
    //_odModelSorter->setFilterKeyColumn(NodeOdItemModel::OdIndex);
    _odModelSorter->setDynamicSortFilter(true);

    setSortingEnabled(true);
    sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

NodeOdTreeView::~NodeOdTreeView()
{
}

Node *NodeOdTreeView::node() const
{
    return _odModel->node();
}

void NodeOdTreeView::setNode(Node *node)
{
    _odModel->setNode(node);
}

bool NodeOdTreeView::isEditable() const
{
    return _odModel->isEditable();
}

void NodeOdTreeView::setEditable(bool editable)
{
    _odModel->setEditable(editable);
}

void NodeOdTreeView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F5)
    {
        const QModelIndex &curentIndex = _odModelSorter->mapToSource(selectionModel()->currentIndex());
        NodeSubIndex *nodeSubIndex = _odModel->nodeSubIndex(curentIndex);
        if (nodeSubIndex)
        {
            _odModel->node()->readObject(nodeSubIndex->nodeIndex()->index(), nodeSubIndex->subIndex());
            return;
        }

        NodeIndex *nodeIndex = _odModel->nodeIndex(curentIndex);
        if (nodeIndex)
        {
            for (NodeSubIndex *subIndexN : nodeIndex->subIndexes())
            {
                _odModel->node()->readObject(nodeIndex->index(), subIndexN->subIndex());
            }
            return;
        }
    }
    QTreeView::keyPressEvent(event);
}
