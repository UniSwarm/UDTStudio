/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include "pdomappingview.h"

#include "pdomappingpainter.h"

#include "services/pdo.h"

#include <QDebug>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QFontMetrics>
#include <QMimeData>
#include <QToolTip>

PDOMappingView::PDOMappingView(QWidget *parent)
    : QWidget(parent)
{
    _pdo = nullptr;
    _enabled = true;
    _dragBitPos = -1;
    setAcceptDrops(true);
    setMaximumHeight(QFontMetrics(font()).height() * 5);
    setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding));
}

const QList<NodeObjectId> &PDOMappingView::nodeListMapping() const
{
    return _nodeListMapping;
}

void PDOMappingView::setNodeListMapping(const QList<NodeObjectId> &nodeListMapping)
{
    _nodeListMapping = nodeListMapping;
    update();
}

const QList<QString> &PDOMappingView::nodeListName() const
{
    return _nodeListName;
}

void PDOMappingView::setNodeListName(const QList<QString> &nodeListName)
{
    _nodeListName = nodeListName;
    update();
}

const QList<QColor> &PDOMappingView::nodeListColor() const
{
    return _nodeListColor;
}

void PDOMappingView::setNodeListColor(const QList<QColor> &nodeListColor)
{
    _nodeListColor = nodeListColor;
    update();
}

int PDOMappingView::objIdAtPos(const QPoint &pos)
{
    QRect rectPdo = rect().adjusted(1, 1, -1, -1);
    return PDOMappingPainter::objIdAtPos(rectPdo, _nodeListMapping, pos);
}

PDO *PDOMappingView::pdo() const
{
    return _pdo;
}

void PDOMappingView::setPdo(PDO *pdo)
{
    if (_pdo != nullptr)
    {
        disconnect(_pdo, &PDO::mappingChanged, this, &PDOMappingView::updateMapping);
    }
    _pdo = pdo;
    if (_pdo != nullptr)
    {
        connect(_pdo, &PDO::mappingChanged, this, &PDOMappingView::updateMapping);
        connect(_pdo, &PDO::enabledChanged, this, &PDOMappingView::updateEnabled);
        _enabled = _pdo->isEnabled();
        updateMapping();
    }
}

int PDOMappingView::heightForWidth(int width) const
{
    int height = width / 6;
    int maxHeight = QFontMetrics(font()).height() * 4;
    height = qMin(height, maxHeight);
    return height;
}

bool PDOMappingView::hasHeightForWidth() const
{
    return true;
}

QSize PDOMappingView::minimumSizeHint() const
{
    return QSize(200, 40);
}

void PDOMappingView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    PDOMappingPainter painter(this);
    QRect rectPdo = rect().adjusted(1, 1, -1, -1);
    painter.drawListMapping(rectPdo, _nodeListMapping, _nodeListName, _nodeListColor, _enabled);

    if (_dragBitPos >= 0)
    {
        painter.drawDragPos(rectPdo, _dragBitPos);
    }
}

void PDOMappingView::updateMapping()
{
    if (_pdo != nullptr)
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
            if (nodeSubIndex != nullptr)
            {
                objName = nodeSubIndex->fullName();
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

void PDOMappingView::updateEnabled(bool enabled)
{
    _enabled = enabled;
    update();
}

void PDOMappingView::dropEvent(QDropEvent *event)
{
    QRect rectPdo = rect().adjusted(1, 1, -1, -1);
    _dragBitPos = PDOMappingPainter::bitFromX(rectPdo, event->posF().x());
    _dragBitPos = (_dragBitPos & 0xF8);

    if (_pdo->canInsertObjectAtBitPos(_nodeListMapping, _dragObjId, _dragBitPos))
    {
        int insertIndex = PDO::indexAtBitPos(_nodeListMapping, _dragBitPos);
        if (insertIndex <= _nodeListMapping.count())
        {
            QString objName;
            QColor color = Qt::blue;
            NodeSubIndex *nodeSubIndex = _dragObjId.nodeSubIndex();
            if (nodeSubIndex != nullptr)
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
            _pdo->writeMapping(_nodeListMapping);
        }
    }

    _dragBitPos = -1;
    update();
}

void PDOMappingView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QStringLiteral("index/subindex")) && (_pdo != nullptr))
    {
#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
        const QStringList &stringListObjId = QString(event->mimeData()->data(QStringLiteral("index/subindex"))).split(':', QString::SkipEmptyParts);
#else
        const QStringList &stringListObjId = QString(event->mimeData()->data(QStringLiteral("index/subindex"))).split(':', Qt::SkipEmptyParts);
#endif
        for (const QString &stringObjId : stringListObjId)
        {
            _dragObjId = NodeObjectId::fromMimeData(stringObjId);
            NodeSubIndex *nodeSubIndex = _dragObjId.nodeSubIndex();
            if (nodeSubIndex == nullptr)
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

void PDOMappingView::dragMoveEvent(QDragMoveEvent *event)
{
    QRect rectPdo = rect().adjusted(1, 1, -1, -1);
    _dragBitPos = PDOMappingPainter::bitFromX(rectPdo, event->posF().x());
    _dragBitPos = (_dragBitPos & 0xF8);
    if (!_pdo->canInsertObjectAtBitPos(_nodeListMapping, _dragObjId, _dragBitPos))
    {
        _dragBitPos = -1;
        update();
        return;
    }
    event->acceptProposedAction();
    update();
}

void PDOMappingView::dragLeaveEvent(QDragLeaveEvent *event)
{
    Q_UNUSED(event)
    _dragBitPos = -1;
    update();
}

bool PDOMappingView::event(QEvent *event)
{
    if (event->type() == QEvent::ToolTip)
    {
        QHelpEvent *helpEvent = dynamic_cast<QHelpEvent *>(event);
        int objId = objIdAtPos(helpEvent->pos());
        if (objId == -1)
        {
            return false;
        }
        QString objName;
        if (objId < _nodeListName.count())
        {
            objName = _nodeListName.at(objId);
        }
        NodeObjectId nodeObjectId = _nodeListMapping.at(objId);
        QString toolTipText = QStringLiteral("0x%1.%2\n%3")
                                  .arg(QString::number(nodeObjectId.index(), 16).toUpper(), QString::number(nodeObjectId.subIndex(), 16).toUpper(), objName);
        QToolTip::showText(helpEvent->globalPos(), toolTipText);
        return true;
    }
    return QWidget::event(event);
}
