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

#include "headerview.h"

#include <QContextMenuEvent>
#include <QMenu>

HeaderView::HeaderView(Qt::Orientation orientation, QWidget *parent)
    : QHeaderView(orientation, parent)
{
}

void HeaderView::addMandatorySection(int section)
{
    _mandatorySection.insert(section);
}

void HeaderView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    for (int col = 0; col < model()->columnCount(); col++)
    {
        QAction *action = menu.addAction(model()->headerData(col, orientation()).toString());
        action->setCheckable(true);
        action->setChecked(!isSectionHidden(col));
        if (_mandatorySection.contains(col))
        {
            action->setDisabled(true);
        }
        connect(action,
                &QAction::triggered,
                this,
                [=]()
                {
                    if (isSectionHidden(col))
                    {
                        showSection(col);
                    }
                    else
                    {
                        hideSection(col);
                    }
                });
    }

    menu.exec(event->globalPos());
}
