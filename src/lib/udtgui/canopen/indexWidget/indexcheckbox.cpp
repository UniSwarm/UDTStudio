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

#include "indexcheckbox.h"

#include <QContextMenuEvent>
#include <QMenu>

IndexCheckBox::IndexCheckBox(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    setObjId(objId);

    setText(tr("Enabled"));

    _widget = this;
}

void IndexCheckBox::mousePressEvent(QMouseEvent *event)
{
    QCheckBox::mousePressEvent(event);
    indexWidgetMouseClick(event);
}

void IndexCheckBox::mouseMoveEvent(QMouseEvent *event)
{
    QCheckBox::mouseMoveEvent(event);
    indexWidgetMouseMove(event);
}

void IndexCheckBox::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
}

void IndexCheckBox::nextCheckState()
{
    QCheckBox::nextCheckState();
    int64_t oldValue = _lastValue.toLongLong();
    int64_t newValue = 0;
    if (isChecked())
    {
        newValue = oldValue | _bitMask;
    }
    else
    {
        newValue = oldValue & ~_bitMask;
    }
    requestWriteValue(QVariant(static_cast<qlonglong>(newValue)));
}

void IndexCheckBox::setDisplayValue(const QVariant &value, AbstractIndexWidget::DisplayAttribute flags)
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

    setChecked(value.toInt() != 0);
}

bool IndexCheckBox::isEditing() const
{
    return false;
}
