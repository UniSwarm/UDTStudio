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

#include "odtreeviewdelegate.h"

#include <QStyledItemDelegate>
#include <QPainter>
#include <QComboBox>

#include "oditemmodel.h"

ODTreeViewDelegate::ODTreeViewDelegate(QObject *parent)
    : QItemDelegate(parent)
{
}

QWidget *ODTreeViewDelegate::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    /*switch (index.column())
    {
    case ODItemModel::Type:
        if ()
        {
            QComboBox *combobox = new QComboBox(parent);
            for (int i=0; i<=Pin::NotConnected; i++)
                combobox->addItem(Pin::electricalTypeDesc(static_cast<Pin::ElectricalType>(i)), i);
            return combobox;
        }
    default:
        return QItemDelegate::createEditor(parent, option, index);
    }*/
    return Q_NULLPTR;
}

void ODTreeViewDelegate::setEditorData(QWidget *editor, const QModelIndex &index) const
{

}

void ODTreeViewDelegate::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{

}
