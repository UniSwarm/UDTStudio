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

#ifndef PDOMAPPINGVIEW_H
#define PDOMAPPINGVIEW_H

#include "../../udtgui_global.h"

#include <QWidget>

#include "nodeobjectid.h"

class PDO;

class UDTGUI_EXPORT PDOMappingView : public QWidget
{
    Q_OBJECT
public:
    PDOMappingView(QWidget *parent = nullptr);

    PDO *pdo() const;

    const QList<NodeObjectId> &nodeListMapping() const;
    void setNodeListMapping(const QList<NodeObjectId> &nodeListMapping);

    const QList<QString> &nodeListName() const;
    void setNodeListName(const QList<QString> &nodeListName);

    const QList<QColor> &nodeListColor() const;
    void setNodeListColor(const QList<QColor> &nodeListColor);

    int objIdAtPos(const QPoint &pos);

public slots:
    void setPdo(PDO *pdo);

    // QWidget interface
public:
    int heightForWidth(int width) const override;
    bool hasHeightForWidth() const override;
    QSize minimumSizeHint() const override;

protected slots:
    void updateMapping();
    void updateEnabled(bool enabled);

protected:
    PDO *_pdo;
    bool _enabled;

    QList<NodeObjectId> _nodeListMapping;
    QList<QString> _nodeListName;
    QList<QColor> _nodeListColor;

    int _dragBitPos;
    NodeObjectId _dragObjId;

    // QWidget interface
protected:
    void paintEvent(QPaintEvent *event) override;

    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dragLeaveEvent(QDragLeaveEvent *event) override;

    // QObject interface
public:
    bool event(QEvent *event) override;
};

#endif  // PDOMAPPINGVIEW_H
