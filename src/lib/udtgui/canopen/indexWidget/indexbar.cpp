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

#include <QContextMenuEvent>
#include <QMenu>
#include <QStyle>
#include <QStyleOptionProgressBar>
#include <QStylePainter>

IndexBar::IndexBar(const NodeObjectId &objId)
    : AbstractIndexWidget(objId)
{
    setObjId(objId);

    _widget = this;
}

void IndexBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QStylePainter paint(this);
    QStyleOptionProgressBar opt;
    initStyleOption(&opt);

    opt.rect = paint.style()->subElementRect(QStyle::SE_ProgressBarGroove, &opt, this);
    paint.drawControl(QStyle::CE_ProgressBarGroove, opt);

    opt.rect = paint.style()->subElementRect(QStyle::SE_ProgressBarContents, &opt, this);
    QRect oldContentRect = opt.rect;
    double offsetScale = 0;
    int range = (opt.maximum - opt.minimum);
    if (opt.minimum < 0)
    {
        offsetScale = -1.0 / (static_cast<double>(range) / opt.minimum);
    }
    if (QProgressBar::value() >= 0)
    {
        opt.minimum = 0;
        opt.invertedAppearance = false;
        opt.rect.adjust(opt.rect.width() * offsetScale, 0, 0, 0);
    }
    else
    {
        opt.maximum = 0;
        opt.progress = opt.minimum - opt.progress;
        opt.invertedAppearance = true;
        opt.rect.adjust(0, 0, -opt.rect.width() * (1.0 - offsetScale), 0);
    }
    paint.drawControl(QStyle::CE_ProgressBarContents, opt);

    opt.rect = oldContentRect;
    opt.rect = paint.style()->subElementRect(QStyle::SE_ProgressBarLabel, &opt, this);
    paint.drawControl(QStyle::CE_ProgressBarLabel, opt);
}

void IndexBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    requestReadValue();
}

void IndexBar::mousePressEvent(QMouseEvent *event)
{
    QProgressBar::mousePressEvent(event);
    indexWidgetMouseClick(event);
}

void IndexBar::mouseMoveEvent(QMouseEvent *event)
{
    QProgressBar::mouseMoveEvent(event);
    indexWidgetMouseMove(event);
}

void IndexBar::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
}

QString IndexBar::text() const
{
    return QStringLiteral("%1%2").arg(AbstractIndexWidget::indexValue().toDouble(), 0, 'g', 2).arg(_unit);
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
