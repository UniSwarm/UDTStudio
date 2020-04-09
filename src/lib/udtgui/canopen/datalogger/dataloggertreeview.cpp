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

#include "node.h"

DataLoggerTreeView::DataLoggerTreeView(QWidget *parent)
    : QTreeView(parent)
{
    _loggerModel = new DataLoggerModel();
    setModel(_loggerModel);

    int w0 = QFontMetrics(font()).horizontalAdvance("0");
    header()->resizeSection(0, 16 * w0);
    header()->resizeSection(1, 8 * w0);
    header()->resizeSection(2, 8 * w0);
    header()->resizeSection(3, 12 * w0);

    setSelectionMode(QAbstractItemView::SingleSelection);
    setDragEnabled(true);
    setDragDropMode(QAbstractItemView::DragDrop);
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
