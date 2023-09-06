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

#ifndef NODEWIDGET_H
#define NODEWIDGET_H

#include "udtgui_global.h"

#include <QWidget>

#include "canopen/indexWidget/abstractindexwidget.h"

class UDTGUI_EXPORT NodeWidget : public QWidget
{
    Q_OBJECT
public:
    NodeWidget(QWidget *parent = nullptr);

    Node *node() const;
    virtual void setNode(Node *node);

    void readAll();
    void readDynamics();

protected:
    void addIndexWidget(AbstractIndexWidget *indexWidget);
    void adddynamicIndexWidget(AbstractIndexWidget *indexWidget);

    void updateObjects();

private:
    Node *_node;
    QList<AbstractIndexWidget *> _indexWidgets;
    QList<AbstractIndexWidget *> _dynamicIndexWidgets;

    // QWidget interface
protected:
    void showEvent(QShowEvent *event) override;
};

#endif  // NODEWIDGET_H
