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

#ifndef PDOMAPPINGPAINTER_H
#define PDOMAPPINGPAINTER_H

#include "../../udtgui_global.h"

#include <QPainter>

#include "services/pdo.h"

class UDTGUI_EXPORT PDOMappingPainter : public QPainter
{
public:
    PDOMappingPainter(QWidget *widget);

    static int bitFromX(const QRect &rect, double x);

    void drawDragPos(const QRect &rect, double pos);

    void drawListMapping(const QRect &rect,
                         const QList<NodeObjectId> &nodeListMapping,
                         const QList<QString> &nodeListName = QList<QString>(),
                         const QList<QColor> &nodeListColor = QList<QColor>(),
                         bool enabled = true);

    void drawMapping(const QRect &objRect,
                     const NodeObjectId &nodeObjectId,
                     const QString &nodeName,
                     const QColor &nodeColor);

    static int objIdAtPos(const QRect &rect,
                   const QList<NodeObjectId> &nodeListMapping,
                   const QPoint &pos);
protected:
    QWidget *_widget;
};

#endif // PDOMAPPINGPAINTER_H
