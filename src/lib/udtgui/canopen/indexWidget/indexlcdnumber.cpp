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

#include "indexlcdnumber.h"

#include <QHBoxLayout>

IndexLCDNumber::IndexLCDNumber(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);

    _lcdNumber = new QLCDNumber();
    _lcdNumber->setStyleSheet("margin-right: 0; padding-right: 0;");
    _label = new QLabel();
    _label->setStyleSheet("margin-left: 0; padding-left: 0;font: italic;");

    hlayout->addWidget(_lcdNumber);
    hlayout->addWidget(_label);

    setLayout(hlayout);
}

void IndexLCDNumber::setDisplayValue(const QVariant &value, AbstractIndexWidget::DisplayAttribute flags)
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

    if (_hint == AbstractIndexWidget::DisplayFloat)
    {
        _lcdNumber->display(QString::number(value.toDouble(), 'f', 2));
    }
    else if (_hint == AbstractIndexWidget::DisplayDirectValue)
    {
        _lcdNumber->display(value.toInt());
    }
    else
    {
        return;
    }

    if (!_unit.isEmpty())
    {
        _label->setText(unit());
    }
}

bool IndexLCDNumber::isEditing() const
{
    return false;
}

void IndexLCDNumber::updateObjId()
{
    setToolTip(QString("0x%1.%2").arg(QString::number(objId().index(), 16).toUpper().rightJustified(4, '0'), QString::number(objId().subIndex()).toUpper().rightJustified(2, '0')));
}

void IndexLCDNumber::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    requestReadValue();
}
