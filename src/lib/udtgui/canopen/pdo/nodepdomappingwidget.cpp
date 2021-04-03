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

#include "nodepdomappingwidget.h"

#include <QVBoxLayout>

#include "node.h"
#include "services/rpdo.h"
#include "services/tpdo.h"

NodePDOMappingWidget::NodePDOMappingWidget(QWidget *parent)
    : QScrollArea(parent)
{
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
                _tpdoMappingWidgets[tpdo]->setPdo(node->tpdos()[tpdo]);
            }
            else
            {
                _tpdoMappingWidgets[tpdo]->setPdo(nullptr);
            }
        }
    }
}

void NodePDOMappingWidget::readAllMapping()
{
    if (_node)
    {
        for (int rpdo = 0; rpdo < 4; rpdo++)
        {
            if (_node->rpdos().count() >= rpdo)
            {
                _node->rpdos()[rpdo]->readMapping();
            }
        }
        for (int tpdo = 0; tpdo < 4; tpdo++)
        {
            if (_node->tpdos().count() >= tpdo)
            {
                _node->tpdos()[tpdo]->readMapping();
            }
        }
    }
}

void NodePDOMappingWidget::createWidgets()
{
    QWidget *widget = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setContentsMargins(0, 0, 0, 0);

    _toolBar = new QToolBar(tr("PDO commands"));
    _toolBar->setIconSize(QSize(20, 20));

    // read all action
    _actionReadMappings = _toolBar->addAction(tr("Read all"));
    _actionReadMappings->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    _actionReadMappings->setShortcut(QKeySequence("Ctrl+R"));
    _actionReadMappings->setStatusTip(tr("Read all PDO mapping from device"));
    connect(_actionReadMappings, &QAction::triggered, this, &NodePDOMappingWidget::readAllMapping);

    layout->addWidget(_toolBar);

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
