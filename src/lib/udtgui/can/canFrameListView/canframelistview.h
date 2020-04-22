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

#ifndef CANFRAMEVIEW_H
#define CANFRAMEVIEW_H

#include "../../udtgui_global.h"

#include <QTableView>
#include <QCanBus>
#include <QCanBusFrame>
#include <QAction>

#include "canframemodel.h"

class UDTGUI_EXPORT CanFrameListView : public QTableView
{
    Q_OBJECT
public:
    CanFrameListView(QWidget *parent = nullptr);
    ~CanFrameListView() override;

    QAction *clearAction() const;
    QAction *copyAction() const;

public slots:
    void appendCanFrame(const QCanBusFrame &frame, bool received);
    void clear();
    void copy();

protected slots:
    void updateSelect(const QItemSelection &selected, const QItemSelection &deselected);

protected:
    CanFrameModel *_canModel;

    // Actions
    void createActions();
    QAction *_clearAction;
    QAction *_copyAction;

    // QWidget interface
protected:
    void contextMenuEvent(QContextMenuEvent *event) override;
};

#endif // CANFRAMEVIEW_H
