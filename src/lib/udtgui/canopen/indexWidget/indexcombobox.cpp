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

#include "indexcombobox.h"

#include <QContextMenuEvent>
#include <QMenu>

IndexComboBox::IndexComboBox(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    setObjId(objId);

    _widget = this;
    _internalUpdate = false;

    connect(this,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            [=](int index)
            {
                setInternalIndex(index);
                displayStatus(itemData(index, Qt::StatusTipRole).toString());
            });

    connect(this,
            QOverload<int>::of(&QComboBox::highlighted),
            [=](int index)
            {
                displayStatus(itemData(index, Qt::StatusTipRole).toString());
            });
}

void IndexComboBox::setInternalIndex(int index)
{
    if (!_internalUpdate)
    {
        requestWriteValue(itemData(index));
    }
}

void IndexComboBox::focusInEvent(QFocusEvent *event)
{
    QComboBox::focusInEvent(event);
    displayStatus(itemData(currentIndex(), Qt::StatusTipRole).toString());
}

void IndexComboBox::focusOutEvent(QFocusEvent *event)
{
    QComboBox::focusOutEvent(event);
    clearStatus();
}

void IndexComboBox::mousePressEvent(QMouseEvent *event)
{
    QComboBox::mousePressEvent(event);
    indexWidgetMouseClick(event);
}

void IndexComboBox::mouseMoveEvent(QMouseEvent *event)
{
    QComboBox::mouseMoveEvent(event);
    indexWidgetMouseMove(event);
}

void IndexComboBox::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
}

void IndexComboBox::setDisplayValue(const QVariant &value, AbstractIndexWidget::DisplayAttribute flags)
{
    if (flags == DisplayAttribute::Error)
    {
        QFont mfont = font();
        mfont.setItalic(true);
        setFont(mfont);
    }
    else
    {
        QFont mfont = font();
        mfont.setItalic(false);
        setFont(mfont);
    }

    int index = findData(value);
    if (index == -1)
    {
        QString text = "(" + QString::number(value.toInt()) + ")";
        setEditable(true);
        setCurrentText(text);
    }
    else
    {
        _internalUpdate = true;
        setCurrentIndex(index);
        _internalUpdate = false;
    }
}

bool IndexComboBox::isEditing() const
{
    return false;
}
