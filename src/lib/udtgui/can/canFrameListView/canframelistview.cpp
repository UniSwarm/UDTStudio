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

#include "canframelistview.h"

#include <QScrollBar>
#include <QHeaderView>
#include <QFontMetrics>
#include <QDebug>
#include <QApplication>
#include <QClipboard>
#include <QContextMenuEvent>
#include <QMenu>
#include <QFontMetrics>

CanFrameListView::CanFrameListView(QWidget *parent)
    : QTableView(parent)
{
    _canModel = new CanFrameModel();
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setModel(_canModel);
    setVerticalScrollMode(ScrollPerPixel);

    connect(selectionModel(), &QItemSelectionModel::selectionChanged, this, &CanFrameListView::updateSelect);

    // columns width
    int w0 = QFontMetrics(font()).horizontalAdvance("0");
    horizontalHeader()->resizeSection(CanFrameModel::Time, 10 * w0);
    horizontalHeader()->resizeSection(CanFrameModel::CanId, 12 * w0);
    horizontalHeader()->resizeSection(CanFrameModel::Type, 8 * w0);

    QFont fontMono = QApplication::font();
    fontMono.setStyleHint(QFont::Monospace);
    int w1 = QFontMetrics(fontMono).horizontalAdvance("00 ");
    horizontalHeader()->resizeSection(CanFrameModel::DataByte, 9 * w1);

    // rows height
    verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    verticalHeader()->setDefaultSectionSize(QFontMetrics(font()).height() * 3 / 2);

    createActions();
}

void CanFrameListView::appendCanFrame(const QCanBusFrame &frame, bool received)
{
    bool needToScroll = (verticalScrollBar()->value() >= verticalScrollBar()->maximum() - rowHeight(0));
    _canModel->appendCanFrame(frame, received);
    if (needToScroll)
    {
        int rowCount = model()->rowCount(rootIndex());
        QModelIndex lastIndex = model()->index(rowCount - 1, 0, rootIndex());
        scrollTo(lastIndex, PositionAtBottom);
    }
}

void CanFrameListView::clear()
{
    _canModel->clear();
}

void CanFrameListView::copy()
{
    QString text;
    if (selectionModel()->selection().isEmpty())
    {
        return;
    }

    QItemSelectionRange range = selectionModel()->selection().first();
    for (int i = range.top(); i <= range.bottom(); ++i)
    {
        QStringList rowContents;
        for (int j = range.left(); j <= range.right(); ++j)
        {
            rowContents << model()->index(i, j).data().toString();
        }
        text += rowContents.join("\t");
        text += "\n";
    }
    QApplication::clipboard()->setText(text);
}

void CanFrameListView::updateSelect(const QItemSelection &selected, const QItemSelection &deselected)
{
    Q_UNUSED(selected)
    Q_UNUSED(deselected)

    bool selectionEmpty = selectionModel()->selectedRows().isEmpty();
    _copyAction->setEnabled(!selectionEmpty);
}

void CanFrameListView::createActions()
{
    _clearAction = new QAction(this);
    _clearAction->setText(tr("Clear &all"));
    _clearAction->setShortcut(QKeySequence::Delete);
    _clearAction->setShortcutContext(Qt::WidgetShortcut);
#if QT_VERSION >= 0x050A00
    _clearAction->setShortcutVisibleInContextMenu(true);
#endif
    connect(_clearAction, &QAction::triggered, this, &CanFrameListView::clear);
    addAction(_clearAction);

    _copyAction = new QAction(this);
    _copyAction->setText(tr("&Copy"));
    _copyAction->setShortcut(QKeySequence::Copy);
    _copyAction->setShortcutContext(Qt::WidgetShortcut);
#if QT_VERSION >= 0x050A00
    _copyAction->setShortcutVisibleInContextMenu(true);
#endif
    _copyAction->setEnabled(false);
    connect(_copyAction, &QAction::triggered, this, &CanFrameListView::copy);
    addAction(_copyAction);
}

QAction *CanFrameListView::copyAction() const
{
    return _copyAction;
}

QAction *CanFrameListView::clearAction() const
{
    return _clearAction;
}

void CanFrameListView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    menu.addAction(_clearAction);
    menu.addAction(_copyAction);
    menu.exec(event->globalPos());
}

CanFrameListView::~CanFrameListView()
{
    delete _canModel;
}
