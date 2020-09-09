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

#include "nodemanagerwidget.h"

#include <QFormLayout>
#include <QDebug>

#include "services/services.h"


NodeManagerWidget::NodeManagerWidget(QWidget *parent)
    : NodeManagerWidget(nullptr, parent)
{
}

NodeManagerWidget::NodeManagerWidget(Node *node, QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
    _widgetDebug = nullptr;
    _action402 = nullptr;
    createWidgets();
    setNode(node);

    registerSubIndex(0x1000, 0x00);
}

Node *NodeManagerWidget::node() const
{
    return _node;
}

void NodeManagerWidget::setNode(Node *node)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &NodeManagerWidget::updateData);
        }
    }

    setNodeInterrest(node);
    _node = node;

    if (_node)
    {
        connect(_node, &Node::statusChanged, this, &NodeManagerWidget::updateData);
        if ((_node->profileNumber()) == 0x192)
        {
            if (!_widgetDebug)
            {
                _widgetDebug = new WidgetDebug(_node);
            }
            else
            {
                _widgetDebug->setNode(_node);
            }

            _action402->setVisible(true);
        }
        else if (_action402)
        {
            _action402->setVisible(false);
        }
    }
    _groupBox->setEnabled(_node);

    updateData();
}

void NodeManagerWidget::updateData()
{
    if (_node)
    {
        _nodeNameEdit->setText(_node->name());
        _nodeStatusLabel->setText(_node->statusStr());
    }
}

void NodeManagerWidget::preop()
{
    if (_node)
    {
        _node->sendPreop();
    }
}

void NodeManagerWidget::start()
{
    if (_node)
    {
        _node->sendStart();
    }
}

void NodeManagerWidget::stop()
{
    if (_node)
    {
        _node->sendStop();
    }
}

void NodeManagerWidget::resetCom()
{
    if (_node)
    {
        _node->sendResetComm();
    }
}

void NodeManagerWidget::resetNode()
{
    if (_node)
    {
        _node->sendResetNode();
    }
}

void NodeManagerWidget::displayMode()
{
    if (_node)
    {
        if (!_widgetDebug)
        {
            _widgetDebug = new WidgetDebug(_node);
        }
        _widgetDebug->show();
    }
}

void NodeManagerWidget::createWidgets()
{
    QAction *action;
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    _groupBox = new QGroupBox(tr("Node"));
    QFormLayout *layoutGroupBox = new QFormLayout();

    // toolbar nmt
    _toolBar = new QToolBar(tr("Node commands"));
    QActionGroup *groupNmt = new QActionGroup(this);
    groupNmt->setExclusive(true);

    action = groupNmt->addAction(tr("Pre operationnal"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-connection-status-on.png"));
    action->setStatusTip(tr("Request node to go in preop mode"));
    connect(action, &QAction::triggered, this, &NodeManagerWidget::preop);

    action = groupNmt->addAction(tr("Start"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-play.png"));
    action->setStatusTip(tr("Request node to go in started mode"));
    connect(action, &QAction::triggered, this, &NodeManagerWidget::start);

    action = groupNmt->addAction(tr("Stop"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-stop.png"));
    action->setStatusTip(tr("Request node to go in stop mode"));
    connect(action, &QAction::triggered, this, &NodeManagerWidget::stop);

    action = groupNmt->addAction(tr("Reset communication"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-process.png"));
    action->setStatusTip(tr("Request node to reset com. parameters"));
    connect(action, &QAction::triggered, this, &NodeManagerWidget::resetCom);

    action = groupNmt->addAction(tr("Reset node"));
    action->setCheckable(true);
    action->setIcon(QIcon(":/icons/img/icons8-reset.png"));
    action->setStatusTip(tr("Request node to reset all values"));
    connect(action, &QAction::triggered, this, &NodeManagerWidget::resetNode);

    _action402 = new QAction();
    _action402->setCheckable(true);
    _action402->setIcon(QIcon(":/icons/img/icons8-stepper-motor.png"));
    _action402->setStatusTip(tr("402 Management"));
    connect(_action402, &QAction::triggered, this, &NodeManagerWidget::displayMode);

    _toolBar->addActions(groupNmt->actions());
    _toolBar->addSeparator();
    _toolBar->addAction(_action402);
    layoutGroupBox->addRow(_toolBar);

    _nodeNameEdit = new QLineEdit();
    layoutGroupBox->addRow(tr("Name :"), _nodeNameEdit);

    _nodeStatusLabel = new QLabel();
    layoutGroupBox->addRow(tr("Status :"), _nodeStatusLabel);

    _groupBox->setLayout(layoutGroupBox);
    layout->addWidget(_groupBox);

    setLayout(layout);
}

void NodeManagerWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId.index == 0x1000 && objId.subIndex == 0x00)
    {
        //_index1000Label->setNum(flags);
    }
}
