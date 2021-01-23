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

#ifndef NODESCREENPDO_H
#define NODESCREENPDO_H

#include "udtgui_global.h"

#include "nodescreen.h"

#include <canopen/nodeod/nodeodwidget.h>
#include <canopen/pdo/nodepdomappingwidget.h>

class UDTGUI_EXPORT NodeScreenPDO : public NodeScreen
{
    Q_OBJECT
public:
    NodeScreenPDO(QWidget *parent = nullptr);

protected:
    void createWidgets();
    NodeOdWidget *_nodeOdWidget;
    NodePDOMappingWidget *_nodePdoMappingWidget;

    // NodeScreen interface
public:
    QString title() const override;
    void setNodeInternal(Node *node, uint8_t axis = 0) override;
};

#endif // NODESCREENPDO_H
