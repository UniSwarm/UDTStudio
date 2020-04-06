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
#include <QHeaderView>
#include <QFontMetrics>

#include "node.h"

NodeOdTreeView::NodeOdTreeView(QWidget *parent)
    : QTreeView(parent)
{
    _odModel = new NodeOdItemModel();
    _odModelSorter = new NodeOdFilterProxyModel(this);
    _odModelSorter->setSourceModel(_odModel);
    setModel(_odModelSorter);

    int w0 = QFontMetrics(font()).horizontalAdvance("0");
    header()->resizeSection(0, 12 * w0);
    header()->resizeSection(1, 40 * w0);
    header()->resizeSection(2, 12 * w0);
    header()->resizeSection(3, 20 * w0);

    _odModelSorter->setFilterKeyColumn(NodeOdItemModel::OdIndex);
    _odModelSorter->setDynamicSortFilter(true);

    setSortingEnabled(true);
    sortByColumn(0, Qt::SortOrder::AscendingOrder);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDrop);
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

void NodeOdTreeView::setFilter(const QString filterText)
{
    _odModelSorter->setFilterRegularExpression(QRegularExpression(filterText));
}

void NodeOdTreeView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_F5)
    {
        const QModelIndex &curentIndex = _odModelSorter->mapToSource(selectionModel()->currentIndex());
        NodeSubIndex *nodeSubIndex = _odModel->nodeSubIndex(curentIndex);
        if (nodeSubIndex)
        {
            _odModel->node()->readObject(nodeSubIndex->index(), nodeSubIndex->subIndex());
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
