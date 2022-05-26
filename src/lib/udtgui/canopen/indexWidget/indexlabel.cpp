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

#include "indexlabel.h"

#include <QContextMenuEvent>
#include <QMenu>

IndexLabel::IndexLabel(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    setObjId(objId);

    _widget = this;
}

void IndexLabel::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    requestReadValue();
}

void IndexLabel::mousePressEvent(QMouseEvent *event)
{
    QLabel::mousePressEvent(event);
    indexWidgetMouseClick(event);
}

void IndexLabel::mouseMoveEvent(QMouseEvent *event)
{
    QLabel::mouseMoveEvent(event);
    indexWidgetMouseMove(event);
}

void IndexLabel::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
}

void IndexLabel::setDisplayValue(const QVariant &value, AbstractIndexWidget::DisplayAttribute flags)
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

    setText(pstringValue(value, _hint));
}

bool IndexLabel::isEditing() const
{
    return false;
}
