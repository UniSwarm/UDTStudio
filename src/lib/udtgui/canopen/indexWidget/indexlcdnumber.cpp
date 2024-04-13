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

#include <QContextMenuEvent>
#include <QHBoxLayout>
#include <QLCDNumber>
#include <QLabel>
#include <QMenu>

IndexLCDNumber::IndexLCDNumber(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    setObjId(objId);

    _widget = this;

    QHBoxLayout *hlayout = new QHBoxLayout();
    hlayout->setContentsMargins(0, 0, 0, 0);
    hlayout->setSpacing(0);

    _lcdNumber = new QLCDNumber();
    _lcdNumber->setStyleSheet(QStringLiteral("margin-right: 0; padding-right: 0;"));
    _label = new QLabel();
    _label->setStyleSheet(QStringLiteral("margin-left: 0; padding-left: 0;font: italic;"));

    hlayout->addWidget(_lcdNumber);
    hlayout->addWidget(_label);

    setLayout(hlayout);
}

void IndexLCDNumber::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    requestReadValue();
}

void IndexLCDNumber::mousePressEvent(QMouseEvent *event)
{
    QWidget::mousePressEvent(event);
    indexWidgetMouseClick(event);
}

void IndexLCDNumber::mouseMoveEvent(QMouseEvent *event)
{
    QWidget::mouseMoveEvent(event);
    indexWidgetMouseMove(event);
}

void IndexLCDNumber::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
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
