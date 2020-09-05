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

#include "dataloggertreeview.h"

#include <QKeyEvent>
#include <QHeaderView>
#include <QFontMetrics>
#include <QKeyEvent>
#include <QItemSelectionModel>
#include <QMessageBox>
#include <QMenu>

#include "node.h"

DataLoggerTreeView::DataLoggerTreeView(QWidget *parent)
    : QTreeView(parent)
{
    _loggerModel = new DataLoggerModel(this);
    _sortProxy = new QSortFilterProxyModel(this);
    _sortProxy->setSourceModel(_loggerModel);
    setModel(_sortProxy);

    int w0 = QFontMetrics(font()).horizontalAdvance("0");
    header()->resizeSection(DataLoggerModel::Node, 22 * w0);
    header()->resizeSection(DataLoggerModel::Index, 10 * w0);
    header()->resizeSection(DataLoggerModel::Name, 20 * w0);
    header()->resizeSection(DataLoggerModel::Value, 8 * w0);
    header()->resizeSection(DataLoggerModel::Min, 8 * w0);
    header()->resizeSection(DataLoggerModel::Max, 8 * w0);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setSortingEnabled(true);

    createActions();
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &DataLoggerTreeView::updateSelect);
}

DataLoggerTreeView::~DataLoggerTreeView()
{
}

DataLogger *DataLoggerTreeView::dataLogger() const
{
    return _loggerModel->dataLogger();
}

void DataLoggerTreeView::setDataLogger(DataLogger *dataLogger)
{
    _loggerModel->setDataLogger(dataLogger);
}

void DataLoggerTreeView::removeCurrent()
{
    if (!_loggerModel->dataLogger())
    {
        return;
    }
    QModelIndexList selection = selectionModel()->selectedRows();
    if (selection.isEmpty())
    {
        return;
    }

    if (!selection.isEmpty())
    {
        QList<QPersistentModelIndex> pindex;
        for (QModelIndex selected : selection)
        {
            const QModelIndex &indexComponent = _sortProxy->mapToSource(selected);
            if (!indexComponent.isValid())
            {
                continue;
            }

            pindex.append(indexComponent);
        }
        if (QMessageBox::question(this, tr("Remove log?"), tr("Do you realy want to remove theses %1 logs?").arg(pindex.count())) != QMessageBox::Yes)
        {
            return;
        }
        selectionModel()->clearSelection();
        for (QPersistentModelIndex index : pindex)
        {
            _loggerModel->dataLogger()->removeData(_loggerModel->objId(index));
        }
    }
}

void DataLoggerTreeView::updateSelect(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    bool selectionEmpty = selectionModel()->selectedRows().isEmpty();
    _removeAction->setEnabled(!selectionEmpty);
}

void DataLoggerTreeView::createActions()
{
    _removeAction = new QAction(this);
    _removeAction->setText(tr("&Remove"));
    _removeAction->setShortcut(QKeySequence::Delete);
    _removeAction->setShortcutContext(Qt::WidgetShortcut);
    _removeAction->setIcon(QIcon(":/icons/img/icons8-delete.png"));
#if QT_VERSION >= 0x050A00
    _removeAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_removeAction, &QAction::triggered, this, &DataLoggerTreeView::removeCurrent);
    addAction(_removeAction);
}

QAction *DataLoggerTreeView::removeAction() const
{
    return _removeAction;
}

void DataLoggerTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(_removeAction);
    menu.exec(event->globalPos());
}
