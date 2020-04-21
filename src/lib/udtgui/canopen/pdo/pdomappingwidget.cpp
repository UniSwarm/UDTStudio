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

#include "pdomappingwidget.h"

#include "pdomappingpainter.h"

#include <QDebug>

PDOMappingWidget::PDOMappingWidget(QWidget *parent)
    : QWidget(parent)
{
    _pdo = nullptr;
}

QList<NodeObjectId> PDOMappingWidget::nodeListMapping() const
{
    return _nodeListMapping;
}

void PDOMappingWidget::setNodeListMapping(const QList<NodeObjectId> &nodeListMapping)
{
    _nodeListMapping = nodeListMapping;
    repaint();
}

PDO *PDOMappingWidget::pdo() const
{
    return _pdo;
}

void PDOMappingWidget::setPdo(PDO *pdo)
{
    if (_pdo)
    {
        disconnect(_pdo, &PDO::mappingChanged, this, &PDOMappingWidget::updateMapping);
    }
    _pdo = pdo;
    if (_pdo)
    {
        connect(_pdo, &PDO::mappingChanged, this, &PDOMappingWidget::updateMapping);
        setNodeListMapping(_pdo->currentMappind());
    }
}

int PDOMappingWidget::heightForWidth(int width) const
{
    return width / 10;
}

bool PDOMappingWidget::hasHeightForWidth() const
{
    return true;
}

QSize PDOMappingWidget::minimumSizeHint() const
{
    return QSize(200, 40);
}

void PDOMappingWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    PDOMappingPainter painter(this);
    QRect rectPdo = rect().adjusted(1, 1, -1, -1);
    painter.drawMapping(rectPdo, _nodeListMapping);
}

void PDOMappingWidget::updateMapping()
{
    if (_pdo)
    {
        setNodeListMapping(_pdo->currentMappind());
    }
}
