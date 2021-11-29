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

#include "indexbar.h"

#include <QDebug>

IndexBar::IndexBar(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{

}

void IndexBar::setDisplayValue(const QVariant &value, DisplayAttribute flags)
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

    setValue(value.toInt());
}

bool IndexBar::isEditing() const
{
    return false;
}

void IndexBar::updateObjId()
{
    setToolTip(QString("0x%1.%2").arg(QString::number(objId().index(), 16).toUpper().rightJustified(4, '0'), QString::number(objId().subIndex()).toUpper().rightJustified(2, '0')));
}

void IndexBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    requestReadValue();
}

QString IndexBar::text() const
{
    return QString("%1 %").arg(AbstractIndexWidget::value().toDouble());
}
