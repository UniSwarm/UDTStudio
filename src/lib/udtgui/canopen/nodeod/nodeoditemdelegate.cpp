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

#include "nodeoditemdelegate.h"

#include <QDebug>
#include <QEvent>
#include <QLineEdit>

NodeOdItemDelegate::NodeOdItemDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

bool NodeOdItemDelegate::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::FocusOut)
    {
        qobject_cast<QLineEdit *>(watched)->setMaxLength(0);
    }
    bool ok = QItemDelegate::eventFilter(watched, event);
    return ok;
}

void NodeOdItemDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    if (!editor->hasFocus())
    {
        QItemDelegate::setEditorData(editor, index);
    }
}
