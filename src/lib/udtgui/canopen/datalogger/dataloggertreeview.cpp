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

#include "dataloggertreeview.h"

#include <QColorDialog>
#include <QFontMetrics>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QTextStream>

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

DLData *DataLoggerTreeView::currentData() const
{
    if (!_loggerModel->dataLogger())
    {
        return nullptr;
    }
    QModelIndexList selection = selectionModel()->selectedRows();
    if (selection.isEmpty())
    {
        return nullptr;
    }
    const QModelIndex &indexComponent = _sortProxy->mapToSource(selection.first());
    return _loggerModel->dlData(indexComponent);
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

void DataLoggerTreeView::setColorCurrent()
{
    DLData *dlData = currentData();
    if (!dlData)
    {
        return;
    }
    QColorDialog colorDialog(this);
    colorDialog.show();
    if (colorDialog.exec())
    {
        dlData->setColor(colorDialog.currentColor());
    }
}

void DataLoggerTreeView::exportOneCurrent()
{
    DLData *dlData = currentData();
    if (!dlData)
    {
        return;
    }
    QFile file(QString("%1_export_%2.csv").arg(QDateTime::currentDateTime().toString("yyyy-MM-dd_hh:mm:ss.zzz")).arg(dlData->name()));
    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }
    QTextStream stream(&file);
    int minCount = qMin(dlData->values().count(), dlData->times().count());
    for (int i = 0; i < minCount; i++)
    {
        stream << dlData->times()[i].toString("yyyy-MM-dd_hh:mm:ss.zzz") << ';' << dlData->values()[i] << '\n';
    }
    file.close();
}

void DataLoggerTreeView::updateSelect(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    bool selectionEmpty = selectionModel()->selectedRows().isEmpty();
    _removeAction->setEnabled(!selectionEmpty);
    _setColorAction->setEnabled(!selectionEmpty);
    _exportOneAction->setEnabled(!selectionEmpty);
}

void DataLoggerTreeView::createActions()
{
    _removeAction = new QAction(this);
    _removeAction->setText(tr("&Remove"));
    _removeAction->setShortcut(QKeySequence::Delete);
    _removeAction->setShortcutContext(Qt::WidgetShortcut);
    _removeAction->setIcon(QIcon(":/icons/img/icons8-delete.png"));
    _removeAction->setEnabled(false);
#if QT_VERSION >= 0x050A00
    _removeAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_removeAction, &QAction::triggered, this, &DataLoggerTreeView::removeCurrent);
    addAction(_removeAction);

    _setColorAction = new QAction(this);
    _setColorAction->setText(tr("&Set color"));
    _setColorAction->setIcon(QIcon(":/icons/img/icons8-paint-palette.png"));
    _setColorAction->setEnabled(false);
    connect(_setColorAction, &QAction::triggered, this, &DataLoggerTreeView::setColorCurrent);
    addAction(_setColorAction);

    _exportOneAction = new QAction(this);
    _exportOneAction->setText(tr("&Export"));
    _exportOneAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    _exportOneAction->setShortcutContext(Qt::WidgetShortcut);
    _exportOneAction->setIcon(QIcon(":/icons/img/icons8-export.png"));
    _exportOneAction->setEnabled(false);
#if QT_VERSION >= 0x050A00
    _exportOneAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_exportOneAction, &QAction::triggered, this, &DataLoggerTreeView::exportOneCurrent);
    addAction(_exportOneAction);
}

QAction *DataLoggerTreeView::removeAction() const
{
    return _removeAction;
}

void DataLoggerTreeView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(_removeAction);
    menu.addAction(_setColorAction);
    menu.addAction(_exportOneAction);
    menu.exec(event->globalPos());
}
