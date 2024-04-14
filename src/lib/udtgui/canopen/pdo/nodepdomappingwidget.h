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

#ifndef NODEPDOMAPPINGWIDGET_H
#define NODEPDOMAPPINGWIDGET_H

#include "../../udtgui_global.h"

#include <QScrollArea>

#include "pdomappingwidget.h"

#include <QToolBar>

class UDTGUI_EXPORT NodePDOMappingWidget : public QScrollArea
{
    Q_OBJECT
public:
    NodePDOMappingWidget(QWidget *parent = nullptr);

    Node *node() const;

public slots:
    void setNode(Node *node);
    void readAllMapping();
    void clearAllMapping();

protected:
    void createWidgets();
    QList<PDOMappingWidget *> _tpdoMappingWidgets;
    QList<PDOMappingWidget *> _rpdoMappingWidgets;

    Node *_node;
    QToolBar *_toolBar;
    QAction *_actionReadMappings;
    QAction *_actionClearMappings;
};

#endif  // NODEPDOMAPPINGWIDGET_H
