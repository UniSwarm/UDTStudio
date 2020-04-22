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
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QFontMetrics>

PDOMappingWidget::PDOMappingWidget(QWidget *parent)
    : QWidget(parent)
{
    _pdo = nullptr;
    _dragBitPos = -1;
    setAcceptDrops(true);
}

const QList<NodeObjectId> &PDOMappingWidget::nodeListMapping() const
{
    return _nodeListMapping;
}

void PDOMappingWidget::setNodeListMapping(const QList<NodeObjectId> &nodeListMapping)
{
    _nodeListMapping = nodeListMapping;
    update();
}

const QList<QString> &PDOMappingWidget::nodeListName() const
{
    return _nodeListName;
}

void PDOMappingWidget::setNodeListName(const QList<QString> &nodeListName)
{
    _nodeListName = nodeListName;
    update();
}

const QList<QColor> &PDOMappingWidget::nodeListColor() const
{
    return _nodeListColor;
}

void PDOMappingWidget::setNodeListColor(const QList<QColor> &nodeListColor)
{
    _nodeListColor = nodeListColor;
    update();
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
        updateMapping();
    }
}

int PDOMappingWidget::heightForWidth(int width) const
{
    int height = width / 10;
    int maxHeight = QFontMetrics(font()).height() * 4;
    height = qMin(height, maxHeight);
    return height;
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
    painter.drawListMapping(rectPdo, _nodeListMapping, _nodeListName, _nodeListColor);

    if (_dragBitPos >= 0)
    {
        painter.drawDragPos(rectPdo, _dragBitPos);
    }
}

void PDOMappingWidget::updateMapping()
{
    if (_pdo)
    {
        const QList<NodeObjectId> &nodeListMapping = _pdo->currentMappind();
        _nodeListName.clear();
        _nodeListColor.clear();
        for (int i = 0; i < nodeListMapping.count(); i++)
        {
            const NodeObjectId &objId = nodeListMapping.at(i);
            QString objName;
            QColor color = Qt::blue;
            NodeSubIndex *nodeSubIndex = objId.nodeSubIndex();
            if (nodeSubIndex)
            {
                objName = nodeSubIndex->name();
                if (nodeSubIndex->isWritable())
                {
                    color = Qt::green;
                }
            }
            _nodeListName.append(objName);
            _nodeListColor.append(color);
        }
        setNodeListMapping(nodeListMapping);
    }
}

void PDOMappingWidget::dropEvent(QDropEvent *event)
{
    QRect rectPdo = rect().adjusted(1, 1, -1, -1);
    _dragBitPos = PDOMappingPainter::bitFromX(rectPdo, event->posF().x());

    if (_pdo->canInsertObjectAtBitPos(_nodeListMapping, _dragObjId, _dragBitPos))
    {
        int insertIndex = PDO::indexAtBitPos(_nodeListMapping, _dragBitPos);
        if (insertIndex <= _nodeListMapping.count())
        {
            QString objName;
            QColor color = Qt::blue;
            NodeSubIndex *nodeSubIndex = _dragObjId.nodeSubIndex();
            if (nodeSubIndex)
            {
                objName = nodeSubIndex->name();
                if (nodeSubIndex->isWritable())
                {
                    color = Qt::green;
                }
            }
            _nodeListName.insert(insertIndex, objName);
            _nodeListColor.insert(insertIndex, color);
            _nodeListMapping.insert(insertIndex, _dragObjId);
        }
    }

    _dragBitPos = -1;
    update();
}

void PDOMappingWidget::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat("index/subindex") && _pdo)
    {
        const QStringList &stringListObjId = QString(event->mimeData()->data("index/subindex")).split(':', QString::SkipEmptyParts);
        for (const QString &stringObjId : stringListObjId)
        {
            _dragObjId = NodeObjectId::fromMimeData(stringObjId);
            NodeSubIndex *nodeSubIndex = _dragObjId.nodeSubIndex();
            if (!nodeSubIndex)
            {
                return;
            }
            if (_pdo->isTPDO() != nodeSubIndex->hasTPDOAccess())
            {
                return;
            }
            if (_pdo->isRPDO() != nodeSubIndex->hasRPDOAccess())
            {
                return;
            }
            if (PDO::mappingBitSize(_nodeListMapping) + nodeSubIndex->bitLength() > _pdo->maxMappingBitSize())
            {
                return;
            }
            _dragObjId = nodeSubIndex->objectId();
        }
        event->acceptProposedAction();
    }
}

void PDOMappingWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QRect rectPdo = rect().adjusted(1, 1, -1, -1);
    _dragBitPos = PDOMappingPainter::bitFromX(rectPdo, event->posF().x());
    if (!_pdo->canInsertObjectAtBitPos(_nodeListMapping, _dragObjId, _dragBitPos))
    {
        _dragBitPos = -1;
        update();
        return;
    }
    event->acceptProposedAction();
    update();
}

void PDOMappingWidget::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)
    _dragBitPos = -1;
    update();
}
