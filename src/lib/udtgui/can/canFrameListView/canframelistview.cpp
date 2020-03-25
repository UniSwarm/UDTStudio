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
#include <QDebug>

CanFrameListView::CanFrameListView(QWidget *parent)
    : QTableView(parent)
{
    _canModel = new CanFrameModel();
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setModel(_canModel);
    setVerticalScrollMode(ScrollPerPixel);
}

void CanFrameListView::appendCanFrame(const QCanBusFrame &frame)
{
    bool needToScroll = (verticalScrollBar()->value() >= verticalScrollBar()->maximum() - rowHeight(0));
    _canModel->appendCanFrame(frame);
    if (needToScroll)
    {
        int rowCount = model()->rowCount(rootIndex());
        QModelIndex lastIndex = model()->index(rowCount - 1, 0, rootIndex());
        scrollTo(lastIndex, PositionAtBottom);
    }
}

CanFrameListView::~CanFrameListView()
{
    delete _canModel;
}
