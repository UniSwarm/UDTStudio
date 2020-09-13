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

#include "nodepdomappingwidget.h"

#include <QVBoxLayout>

#include "node.h"
#include "services/tpdo.h"
#include "services/rpdo.h"

NodePDOMappingWidget::NodePDOMappingWidget(QWidget *parent)
    : QScrollArea(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    setWidgetResizable(true);
    createWidgets();
}

Node *NodePDOMappingWidget::node() const
{
    return _node;
}

void NodePDOMappingWidget::setNode(Node *node)
{
    _node = node;
    if (node)
    {
        for (int rpdo = 0; rpdo < 4; rpdo++)
        {
            if (node->rpdos().count() >= rpdo)
            {
                node->rpdos()[rpdo]->readMapping();
                _rpdoMappingWidgets[rpdo]->setPdo(node->rpdos()[rpdo]);
            }
            else
            {
                _rpdoMappingWidgets[rpdo]->setPdo(nullptr);
            }
        }
        for (int tpdo = 0; tpdo < 4; tpdo++)
        {
            if (node->tpdos().count() >= tpdo)
            {
                node->tpdos()[tpdo]->readMapping();
                _tpdoMappingWidgets[tpdo]->setPdo(node->tpdos()[tpdo]);
            }
            else
            {
                _tpdoMappingWidgets[tpdo]->setPdo(nullptr);
            }
        }
    }
}

void NodePDOMappingWidget::createWidgets()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    for (int rpdo = 0; rpdo < 4; rpdo++)
    {
        PDOMappingWidget *mappingWidget = new PDOMappingWidget();
        _rpdoMappingWidgets.append(mappingWidget);
        layout->addWidget(mappingWidget);
    }

    for (int tpdo = 0; tpdo < 4; tpdo++)
    {
        PDOMappingWidget *mappingWidget = new PDOMappingWidget();
        _tpdoMappingWidgets.append(mappingWidget);
        layout->addWidget(mappingWidget);
    }

    layout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    widget->setLayout(layout);
    setWidget(widget);
}
