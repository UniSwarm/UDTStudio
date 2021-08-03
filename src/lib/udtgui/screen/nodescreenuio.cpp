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

#include "nodescreenuio.h"

#include "canopen/profileWidget/p401/p401widget.h"

#include <QLayout>

NodeScreenUio::NodeScreenUio(QWidget *parent)
    : NodeScreen(parent)
{
    createWidgets();
}

void NodeScreenUio::readAll()
{
    _p401Widget->readAllObject();
}

void NodeScreenUio::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    QToolBar *toolBar = new QToolBar(tr("UIO commands"));
    toolBar->setIconSize(QSize(20, 20));

    // read all action
    QAction *actionReadMappings = toolBar->addAction(tr("Read all"));
    actionReadMappings->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    actionReadMappings->setShortcut(QKeySequence("Ctrl+R"));
    actionReadMappings->setStatusTip(tr("Read all object on window"));
    connect(actionReadMappings, &QAction::triggered, this, &NodeScreenUio::readAll);

    layout->addWidget(toolBar);

    _p401Widget = new P401Widget(8, this);
    layout->addWidget(_p401Widget);

    setLayout(layout);
}

QString NodeScreenUio::title() const
{
    return QString(tr("UIO"));
}

QIcon NodeScreenUio::icon() const
{
    return QIcon(":/uBoards/uio.png");
}

void NodeScreenUio::setNodeInternal(Node *node, uint8_t axis)
{
    if (!node)
    {
        return;
    }

    if ((node->profileNumber() != 0x191))
    {
        return;
    }

    _p401Widget->setNode(node);
}
