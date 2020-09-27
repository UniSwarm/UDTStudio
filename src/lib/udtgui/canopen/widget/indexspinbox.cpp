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

#include "indexspinbox.h"

#include <QDebug>
#include <QKeyEvent>
#include <QLineEdit>

IndexSpinBox::IndexSpinBox(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    setMaximum(0xFFFF);
}

void IndexSpinBox::setDisplayValue(const QVariant &value, DisplayAttribute flags)
{
    if (flags == DisplayAttribute::Error)
    {
        QFont mfont = font();
        mfont.setItalic(true);
        lineEdit()->setFont(mfont);
    }
    else
    {
        QFont mfont = font();
        mfont.setItalic(false);
        lineEdit()->setFont(mfont);
    }
    QSpinBox::setValue(value.toInt());
}

bool IndexSpinBox::isEditing() const
{
    return lineEdit()->hasFocus();
}

void IndexSpinBox::keyPressEvent(QKeyEvent *event)
{
    QSpinBox::keyPressEvent(event);
    switch (event->key())
    {
    case Qt::Key_Return:
    case Qt::Key_Enter:
        requestWriteValue(QVariant(value()));
        break;

    case Qt::Key_F5:
        requestReadValue();
    }
}

void IndexSpinBox::focusOutEvent(QFocusEvent *event)
{
    cancelEdit();
    QSpinBox::focusOutEvent(event);
}
