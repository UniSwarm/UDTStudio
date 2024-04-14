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

#ifndef NODEODITEMDELEGATE_H
#define NODEODITEMDELEGATE_H

#include "../../udtgui_global.h"

#include <QItemDelegate>

class UDTGUI_EXPORT NodeOdItemDelegate : public QItemDelegate
{
public:
    NodeOdItemDelegate(QObject *parent = nullptr);

    // QObject interface
public:
    bool eventFilter(QObject *editor, QEvent *event) override;

    // QAbstractItemDelegate interface
public:
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

private:
    mutable QString _currentEditUnit;
};

#endif  // NODEODITEMDELEGATE_H
