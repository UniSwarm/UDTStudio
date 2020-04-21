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

#include "pdomappingpainter.h"

#include <QPainter>
#include <QDebug>

PDOMappingPainter::PDOMappingPainter(QWidget *widget)
    : QPainter(widget), _widget(widget)
{
}

void PDOMappingPainter::drawMapping(const QRect &rect, const QList<NodeObjectId> &nodeListMapping)
{
    setRenderHint(QPainter::Antialiasing);
    setViewport(rect);

    int yMargin = 5;
    int xMargin = 2;
    int height = rect.height();
    double bitWidth = (static_cast<double>(rect.width()) - 2 * xMargin) / (8 * 8);

    // draw ticks
    setPen(QPen(Qt::gray, 1));
    for (int tick = 1; tick < 8; tick++)
    {
        int x = static_cast<int>(xMargin + tick * (bitWidth * 8));
        drawLine(x, 0, x, 3);
        drawLine(x, height - 3, x, height);
    }

    // draw mapping
    QColor color(Qt::blue);
    setPen(QPen(color.darker(), 1));
    QLinearGradient gradient;
    gradient.setStart(0, 50);
    gradient.setFinalStop(0, 150);
    gradient.setSpread(QGradient::ReflectSpread);
    gradient.setColorAt(0, color.lighter(160));
    gradient.setColorAt(1, color.lighter(130));
    setBrush(gradient);
    double x = xMargin;
    QFontMetrics fontMetrics(font(), _widget);
    for (const NodeObjectId &objId : nodeListMapping)
    {
        if (objId.bitSize() == 0)
        {
            continue;
        }
        x += xMargin;

        double width = objId.bitSize() * bitWidth - 2 * xMargin;
        QRectF objRect(x, yMargin, width, height - yMargin * 2);

        drawRoundedRect(objRect, 5, 5);

        QString objName;
        NodeSubIndex *nodeSubIndex = objId.nodeSubIndex();
        if (nodeSubIndex)
        {
            objName = fontMetrics.elidedText(nodeSubIndex->name(), Qt::ElideRight, static_cast<int>(width));
        }

        if (objName.isEmpty())
        {
            drawText(objRect, Qt::AlignCenter, QString("0x%1.%2")
                                                   .arg(QString::number(objId.index, 16))
                                                   .arg(objId.subIndex));
        }
        else
        {
            drawText(objRect, Qt::AlignCenter, QString("0x%1.%2\n%3")
                                                   .arg(QString::number(objId.index, 16))
                                                   .arg(objId.subIndex)
                                                   .arg(objName));
        }
        x += width + xMargin;
    }
}
