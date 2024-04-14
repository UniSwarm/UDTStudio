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

#include "dataloggertreeview.h"

#include <QColorDialog>
#include <QDir>
#include <QFontMetrics>
#include <QHeaderView>
#include <QItemSelectionModel>
#include <QKeyEvent>
#include <QMenu>
#include <QMessageBox>
#include <QStandardPaths>

#include "udtguimanager.h"
#include "utils/headerview.h"

DataLoggerTreeView::DataLoggerTreeView(QWidget *parent)
    : QTreeView(parent)
{
    HeaderView *headerView = new HeaderView(Qt::Horizontal, this);
    headerView->addMandatorySection(DataLoggerModel::NodeName);
    setHeader(headerView);

    _loggerModel = new DataLoggerModel(this);
    _sortProxy = new QSortFilterProxyModel(this);
    _sortProxy->setSourceModel(_loggerModel);
    setModel(_sortProxy);

#if QT_VERSION >= 0x050B00
    int w0 = QFontMetrics(font()).horizontalAdvance(QStringLiteral("0"));
#else
    int w0 = QFontMetrics(font()).width(QStringLiteral("0"));
#endif
    header()->resizeSection(DataLoggerModel::NodeName, 22 * w0);
    header()->resizeSection(DataLoggerModel::Index, 10 * w0);
    header()->resizeSection(DataLoggerModel::Name, 20 * w0);
    header()->resizeSection(DataLoggerModel::Value, 10 * w0);
    header()->resizeSection(DataLoggerModel::Min, 10 * w0);
    header()->resizeSection(DataLoggerModel::Max, 10 * w0);

    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDrop);
    setSortingEnabled(true);
    sortByColumn(0, Qt::AscendingOrder);

    createActions();
    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &DataLoggerTreeView::updateSelect);
}

DataLoggerTreeView::~DataLoggerTreeView()
{
}

DLData *DataLoggerTreeView::currentData() const
{
    if (_loggerModel->dataLogger() == nullptr)
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
    if (_loggerModel->dataLogger() == nullptr)
    {
        return;
    }
    QModelIndexList selection = selectionModel()->selectedRows();
    if (selection.isEmpty())
    {
        return;
    }

    QList<QPersistentModelIndex> pindex;
    for (QModelIndex selected : qAsConst(selection))
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
    for (const QPersistentModelIndex &index : pindex)
    {
        _loggerModel->dataLogger()->removeData(_loggerModel->objId(index));
    }
}

void DataLoggerTreeView::setColorCurrent()
{
    DLData *dlData = currentData();
    if (dlData == nullptr)
    {
        return;
    }
    QColorDialog colorDialog(this);
    colorDialog.show();
    if (colorDialog.exec() != 0)
    {
        dlData->setColor(colorDialog.currentColor());
    }
}

void DataLoggerTreeView::exportOneCurrent() const
{
    DLData *dlData = currentData();
    if (dlData == nullptr)
    {
        return;
    }

    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/UDTStudio/";
    QDir().mkdir(path);

    dlData->exportCSVData(path + QStringLiteral("export_%1.csv").arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd_hh:mm:ss.zzz"))));
}

void DataLoggerTreeView::locateODCurrent()
{
    DLData *dlData = currentData();
    if (dlData == nullptr)
    {
        return;
    }

    UdtGuiManager::locateInOdTreeView(dlData->objectId());
}

void DataLoggerTreeView::updateSelect(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    bool selectionEmpty = selectionModel()->selectedRows().isEmpty();
    _removeAction->setEnabled(!selectionEmpty);
    _setColorAction->setEnabled(!selectionEmpty);
    _exportOneAction->setEnabled(!selectionEmpty);
    _locadeODAction->setEnabled(!selectionEmpty);
}

void DataLoggerTreeView::createActions()
{
    _removeAction = new QAction(this);
    _removeAction->setText(tr("&Remove"));
    _removeAction->setShortcut(QKeySequence::Delete);
    _removeAction->setShortcutContext(Qt::WidgetShortcut);
    _removeAction->setIcon(QIcon(QStringLiteral(":/icons/img/icons8-delete.png")));
    _removeAction->setEnabled(false);
#if QT_VERSION >= 0x050A00
    _removeAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_removeAction, &QAction::triggered, this, &DataLoggerTreeView::removeCurrent);
    addAction(_removeAction);

    _setColorAction = new QAction(this);
    _setColorAction->setText(tr("&Set color"));
    _setColorAction->setIcon(QIcon(QStringLiteral(":/icons/img/icons8-paint-palette.png")));
    _setColorAction->setEnabled(false);
    connect(_setColorAction, &QAction::triggered, this, &DataLoggerTreeView::setColorCurrent);
    addAction(_setColorAction);

    _exportOneAction = new QAction(this);
    _exportOneAction->setText(tr("&Export"));
    _exportOneAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_S));
    _exportOneAction->setShortcutContext(Qt::WidgetShortcut);
    _exportOneAction->setIcon(QIcon(QStringLiteral(":/icons/img/icons8-export.png")));
    _exportOneAction->setStatusTip(
        tr("Exports CSV data in '%1' directory").arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/UDTStudio/"));
    _exportOneAction->setEnabled(false);
#if QT_VERSION >= 0x050A00
    _exportOneAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_exportOneAction, &QAction::triggered, this, &DataLoggerTreeView::exportOneCurrent);
    addAction(_exportOneAction);

    _locadeODAction = new QAction(this);
    _locadeODAction->setText(tr("&Locate in OD"));
    _locadeODAction->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_L));
    _locadeODAction->setShortcutContext(Qt::WidgetShortcut);
    _locadeODAction->setEnabled(false);
#if QT_VERSION >= 0x050A00
    _locadeODAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_locadeODAction, &QAction::triggered, this, &DataLoggerTreeView::locateODCurrent);
    addAction(_locadeODAction);
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
    menu.addAction(_locadeODAction);
    menu.exec(event->globalPos());
}
