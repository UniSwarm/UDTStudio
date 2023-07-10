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

#ifndef DATALOGGERTREEVIEW_H
#define DATALOGGERTREEVIEW_H

#include "../../udtgui_global.h"

#include <QAction>
#include <QSortFilterProxyModel>
#include <QTreeView>

#include "dataloggermodel.h"

class UDTGUI_EXPORT DataLoggerTreeView : public QTreeView
{
    Q_OBJECT
public:
    DataLoggerTreeView(QWidget *parent = nullptr);
    ~DataLoggerTreeView() override;

    DLData *currentData() const;

    DataLogger *dataLogger() const;
    void setDataLogger(DataLogger *dataLogger);

    QAction *removeAction() const;

public slots:
    void removeCurrent();
    void setColorCurrent();
    void exportOneCurrent() const;
    void locateODCurrent();

protected slots:
    void updateSelect(const QItemSelection &selected, const QItemSelection &deselected);

protected:
    DataLoggerModel *_loggerModel;
    QSortFilterProxyModel *_sortProxy;

    void createActions();
    QAction *_removeAction;
    QAction *_setColorAction;
    QAction *_exportOneAction;
    QAction *_locadeODAction;

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif  // DATALOGGERTREEVIEW_H
