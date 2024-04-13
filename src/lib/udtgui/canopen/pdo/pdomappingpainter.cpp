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

#include "pdomappingpainter.h"

#include <QDebug>
#include <QWidget>
#include <QtMath>

const int yMargin = 5;
const int xMargin = 3;

PDOMappingPainter::PDOMappingPainter(QWidget *widget)
    : QPainter(widget),
      _widget(widget)
{
    setRenderHint(QPainter::Antialiasing);
}

int PDOMappingPainter::bitFromX(const QRect &rect, double x)
{
    double bitWidth = (static_cast<double>(rect.width()) - 2 * xMargin) / (8 * 8);
    return qRound((x - xMargin) / bitWidth);
}

void PDOMappingPainter::drawDragPos(const QRect &rect, double pos)
{
    setViewport(rect);

    double bitWidth = (static_cast<double>(rect.width()) - 2 * xMargin) / (8 * 8);
    int height = rect.height();
    int x = static_cast<int>(qRound(pos) * bitWidth) + xMargin;

    setPen(QPen(Qt::darkRed, 1));
    setBrush(QBrush(Qt::darkRed));
    drawLine(x, 0, x, height);
    QPolygon arrow;
    arrow << QPoint(x - (xMargin + 1), 0) << QPoint(x + (xMargin + 1), 0) << QPoint(x, yMargin);
    drawPolygon(arrow);
}

void PDOMappingPainter::drawListMapping(const QRect &rect,
                                        const QList<NodeObjectId> &nodeListMapping,
                                        const QList<QString> &nodeListName,
                                        const QList<QColor> &nodeListColor,
                                        bool enabled)
{
    setViewport(rect);

    int height = rect.height();
    double bitWidth = (static_cast<double>(rect.width()) - 2 * xMargin) / (8 * 8);

    // draw ticks
    setPen(QPen(Qt::gray, 1));
    for (int tick = 0; tick <= 8; tick++)
    {
        int x = static_cast<int>(xMargin + tick * (bitWidth * 8));
        drawLine(x, 0, x, 3);
        drawLine(x, height - 3, x, height);
    }

    // draw mapping
    double x = xMargin;
    for (int i = 0; i < nodeListMapping.count(); i++)
    {
        const NodeObjectId &objId = nodeListMapping.at(i);

        x += xMargin;
        double width = objId.bitSize() * bitWidth - 2 * xMargin;
        QRectF objRect(x, yMargin, width, height - yMargin * 2);

        QString name;
        if (i < nodeListName.count())
        {
            name = nodeListName.at(i);
        }
        QColor color(Qt::blue);
        if (i < nodeListColor.count())
        {
            color = nodeListColor.at(i);
        }
        if (!enabled)
        {
            int h;
            int s;
            int v;
            color.getHsv(&h, &s, &v);
            v /= 4;
            s /= 4;
            color.setHsv(h, s, v);
        }

        drawMapping(objRect.toRect(), objId, name, color);

        x += width + xMargin;
    }
}

void PDOMappingPainter::drawMapping(const QRect &objRect, const NodeObjectId &nodeObjectId, const QString &nodeName, const QColor &nodeColor)
{
    setPen(QPen(nodeColor.darker(), 1));
    QLinearGradient gradient;
    gradient.setStart(0, 50);
    gradient.setFinalStop(0, 150);
    gradient.setSpread(QGradient::ReflectSpread);
    gradient.setColorAt(0, nodeColor.lighter(160));
    gradient.setColorAt(1, nodeColor.lighter(130));
    setBrush(gradient);
    QFontMetrics fontMetrics(font(), _widget);
    if (nodeObjectId.bitSize() == 0)
    {
        return;
    }

    drawRoundedRect(objRect, 5, 5);

    QRectF textRext = objRect.adjusted(1, 1, -1, -1);
    QString objName = nodeName;
    objName = fontMetrics.elidedText(objName, Qt::ElideLeft, static_cast<int>(textRext.width()));

    if (objName.isEmpty() || textRext.height() < fontMetrics.height() * 2)
    {
        drawText(textRext,
                 Qt::AlignCenter,
                 QStringLiteral("0x%1.%2").arg(QString::number(nodeObjectId.index(), 16).toUpper(), QString::number(nodeObjectId.subIndex(), 16).toUpper()));
    }
    else
    {
        drawText(textRext,
                 Qt::AlignCenter,
                 QStringLiteral("0x%1.%2\n%3")
                     .arg(QString::number(nodeObjectId.index(), 16).toUpper(), QString::number(nodeObjectId.subIndex(), 16).toUpper())
                     .arg(objName));
    }
}

int PDOMappingPainter::objIdAtPos(const QRect &rect, const QList<NodeObjectId> &nodeListMapping, const QPoint &pos)
{
    int height = rect.height();
    double bitWidth = (static_cast<double>(rect.width()) - 2 * xMargin) / (8 * 8);

    // draw mapping
    double x = xMargin;
    for (int i = 0; i < nodeListMapping.count(); i++)
    {
        const NodeObjectId &objId = nodeListMapping.at(i);

        x += xMargin;
        double width = objId.bitSize() * bitWidth - 2 * xMargin;
        QRectF objRect(x, yMargin, width, height - yMargin * 2);

        if (objRect.contains(pos))
        {
            return i;
        }

        x += width + xMargin;
    }
    return -1;
}
