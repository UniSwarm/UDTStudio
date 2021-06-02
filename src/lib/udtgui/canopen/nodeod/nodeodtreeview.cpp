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

#include "nodeodtreeview.h"

#include <QClipboard>
#include <QDebug>
#include <QFontMetrics>
#include <QGuiApplication>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMenu>
#include <QRegExp>

#include "node.h"

NodeOdTreeView::NodeOdTreeView(QWidget *parent)
    : QTreeView(parent)
{
    _odModel = new NodeOdItemModel(this);
    _odModelSorter = new NodeOdFilterProxyModel(this);
    _odModelSorter->setSourceModel(_odModel);
    setModel(_odModelSorter);

    int w0 = QFontMetrics(font()).horizontalAdvance("0");
    header()->resizeSection(0, 12 * w0);
    header()->resizeSection(1, 40 * w0);
    header()->resizeSection(2, 12 * w0);
    header()->resizeSection(3, 10 * w0);
    header()->resizeSection(4, 20 * w0);

    _odModelSorter->setDynamicSortFilter(true);

    setSortingEnabled(true);
    sortByColumn(0, Qt::SortOrder::AscendingOrder);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDrop);

    _delegate = new NodeOdItemDelegate(this);
    setItemDelegate(_delegate);

    createActions();
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &NodeOdTreeView::updateSelect);
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
    QRegularExpression filterRegExp("(?:pdo:(?<pdo>[^ ]*) *|type:(?<type>[^ ]*) *)*(.*)");
    QRegularExpressionMatch match = filterRegExp.match(filterText);

    // pdo
    QString pdoFilter = match.captured("pdo");
    QStringList pdoMatch = {"all", "pdo", "rpdo", "tpdo"};
    int idMatch = pdoMatch.indexOf(pdoFilter);
    if (idMatch != -1)
    {
        _odModelSorter->setPdoFilter(static_cast<NodeOdFilterProxyModel::PDOFilter>(idMatch));
    }
    else
    {
        _odModelSorter->setPdoFilter(NodeOdFilterProxyModel::PDOFILTER_ALL);
    }

    QString textFilter = match.capturedTexts().at(match.lastCapturedIndex());
    if (textFilter.startsWith("0x", Qt::CaseInsensitive))
    {
        _odModelSorter->setFilterKeyColumn(NodeOdItemModel::OdIndex);
    }
    else
    {
        _odModelSorter->setFilterKeyColumn(NodeOdItemModel::Name);
    }
#if QT_VERSION >= 0x050C00
    _odModelSorter->setFilterRegularExpression(QRegularExpression(textFilter, QRegularExpression::CaseInsensitiveOption));
#else
    _odModelSorter->setFilterRegExp(QRegExp(textFilter, Qt::CaseInsensitive));
#endif
}

void NodeOdTreeView::readSelected()
{
    QModelIndexList selectedRows = selectionModel()->selectedRows();
    for (QModelIndex row : qAsConst(selectedRows))
    {
        const QModelIndex &curentIndex = _odModelSorter->mapToSource(row);

        NodeIndex *nodeIndex = _odModel->nodeIndex(curentIndex);
        if (nodeIndex)
        {
            for (NodeSubIndex *subIndexN : nodeIndex->subIndexes())
            {
                _odModel->node()->readObject(nodeIndex->index(), subIndexN->subIndex());
            }
            continue;
        }

        NodeSubIndex *nodeSubIndex = _odModel->nodeSubIndex(curentIndex);
        if (nodeSubIndex)
        {
            _odModel->node()->readObject(nodeSubIndex->index(), nodeSubIndex->subIndex());
            continue;
        }
    }
}

void NodeOdTreeView::readAll()
{
    for (int i = 0; i < _odModelSorter->rowCount(); i++)
    {
        const QModelIndex &firstIndex = _odModelSorter->mapToSource(_odModelSorter->index(i, 0));

        NodeIndex *nodeIndex = _odModel->nodeIndex(firstIndex);
        if (nodeIndex)
        {
            for (NodeSubIndex *subIndexN : nodeIndex->subIndexes())
            {
                if (subIndexN->isReadable())
                {
                    _odModel->node()->readObject(nodeIndex->index(), subIndexN->subIndex());
                }
            }
        }
    }
}

void NodeOdTreeView::copy()
{
    QString copyText;
    QModelIndexList selectedRows = selectionModel()->selectedRows();
    for (QModelIndex row : qAsConst(selectedRows))
    {
        const QModelIndex &curentIndex = _odModelSorter->mapToSource(row);
        NodeSubIndex *nodeSubIndex = _odModel->nodeSubIndex(curentIndex);
        if (nodeSubIndex)
        {
            copyText.append(nodeSubIndex->nodeIndex()->name() + "." + nodeSubIndex->name() + "=" + nodeSubIndex->value().toString() + "\n");
        }
    }
    QGuiApplication::clipboard()->setText(copyText);
}

void NodeOdTreeView::createActions()
{
    _readAction = new QAction(this);
    _readAction->setText(tr("&Read"));
    _readAction->setShortcut(Qt::Key_F5);
    _readAction->setShortcutContext(Qt::WidgetShortcut);
    _readAction->setIcon(QIcon(":/icons/img/icons8-import.png"));
    _readAction->setEnabled(false);
#if QT_VERSION >= 0x050A00
    _readAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_readAction, &QAction::triggered, this, &NodeOdTreeView::readSelected);
    addAction(_readAction);

    _readAllAction = new QAction(this);
    _readAllAction->setText(tr("Read &all"));
    _readAllAction->setShortcut(QKeySequence(Qt::SHIFT, Qt::Key_F5));
    _readAllAction->setShortcutContext(Qt::WidgetShortcut);
    _readAllAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
#if QT_VERSION >= 0x050A00
    _readAllAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_readAllAction, &QAction::triggered, this, &NodeOdTreeView::readAll);
    addAction(_readAllAction);

    _copyAction = new QAction(this);
    _copyAction->setText(tr("&Copy"));
    _copyAction->setShortcut(QKeySequence::Copy);
    _copyAction->setShortcutContext(Qt::WidgetShortcut);
    _copyAction->setIcon(QIcon(":/icons/img/icons8-copy"));
#if QT_VERSION >= 0x050A00
    _copyAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_copyAction, &QAction::triggered, this, &NodeOdTreeView::copy);
    addAction(_copyAction);

    _expandAllAction = new QAction(this);
    _expandAllAction->setText(tr("&Expend all"));
    connect(_expandAllAction, &QAction::triggered, this, &QTreeView::expandAll);
    addAction(_expandAllAction);
}

void NodeOdTreeView::updateSelect(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    bool selectionEmpty = selectionModel()->selectedRows().isEmpty();
    _readAction->setEnabled(!selectionEmpty);
    _copyAction->setEnabled(!selectionEmpty);
}

void NodeOdTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(_readAction);
    menu.addAction(_readAllAction);
    menu.addAction(_copyAction);
    menu.addAction(_expandAllAction);
    menu.exec(event->globalPos());
}
