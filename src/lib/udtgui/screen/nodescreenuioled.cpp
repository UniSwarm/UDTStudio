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

#include "nodescreenuioled.h"

#include <QToolBar>
#include <QVBoxLayout>

#include "canopen/profileWidget/p428/p428widget.h"

NodeScreenUIOLed::NodeScreenUIOLed(QWidget *parent)
    : NodeScreen(parent)
{
    createWidgets();
}

void NodeScreenUIOLed::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);

    QToolBar *toolBar = new QToolBar(tr("UIO commands"));
    toolBar->setIconSize(QSize(20, 20));

    // read all action
    QAction *readAllObjectAction = toolBar->addAction(tr("Read all objects"));
    readAllObjectAction->setIcon(QIcon(QStringLiteral(":/icons/img/icons8-update.png")));
    readAllObjectAction->setShortcut(QKeySequence(QStringLiteral("Ctrl+R")));
    readAllObjectAction->setStatusTip(tr("Read all the objects of the current window"));
    toolBar->addAction(readAllObjectAction);

    layout->addWidget(toolBar);

    _p428Widget = new P428Widget(10, this);
    connect(readAllObjectAction, &QAction::triggered, _p428Widget, &P428Widget::readAllObject);
    layout->addWidget(_p428Widget);

    setLayout(layout);
}

QString NodeScreenUIOLed::title() const
{
    return tr("UIO led");
}

QIcon NodeScreenUIOLed::icon() const
{
    return QIcon(":/uBoards/uio.png");
}

void NodeScreenUIOLed::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(axis)

    if (node == nullptr)
    {
        return;
    }

    if ((node->profileNumber() != 428))
    {
        return;
    }

    _p428Widget->setNode(node);
}
