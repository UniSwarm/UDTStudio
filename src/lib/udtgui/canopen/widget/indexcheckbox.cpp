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

#include "indexcheckbox.h"

IndexCheckBox::IndexCheckBox(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    setText(tr("Enabled"));
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

void IndexCheckBox::updateObjId()
{
    setToolTip(QString("0x%1.%2").arg(QString::number(objId().index(), 16).toUpper().rightJustified(4, '0')).arg(QString::number(objId().subIndex()).toUpper().rightJustified(2, '0')));
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
    requestWriteValue(newValue);
}
