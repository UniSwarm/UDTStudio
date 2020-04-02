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

NodeManagerWidget::NodeManagerWidget(QWidget *parent)
    : NodeManagerWidget(nullptr, parent)
{
}

NodeManagerWidget::NodeManagerWidget(Node *node, QWidget *parent)
    : QWidget(parent)
{
    _node = nullptr;
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

void NodeManagerWidget::test()
{
    readObject(0x1000, 0x00, QMetaType::Int);
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

    action = groupNmt->addAction(tr("preop"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, this, &NodeManagerWidget::preop);

    action = groupNmt->addAction(tr("start"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, this, &NodeManagerWidget::start);

    action = groupNmt->addAction(tr("stop"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, this, &NodeManagerWidget::stop);

    action = groupNmt->addAction(tr("rstCom"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, this, &NodeManagerWidget::resetCom);

    action = groupNmt->addAction(tr("rstNode"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, this, &NodeManagerWidget::resetNode);

    action = groupNmt->addAction(tr("test"));
    action->setCheckable(true);
    connect(action, &QAction::triggered, this, &NodeManagerWidget::test);

    _toolBar->addActions(groupNmt->actions());
    layoutGroupBox->addRow(_toolBar);

    _nodeNameEdit = new QLineEdit();
    layoutGroupBox->addRow(tr("Name :"), _nodeNameEdit);

    _nodeStatusLabel = new QLabel();
    layoutGroupBox->addRow(tr("Status :"), _nodeStatusLabel);

    _index1000Label = new QLabel();
    layoutGroupBox->addRow(tr("0x1000 :"), _index1000Label);

    _groupBox->setLayout(layoutGroupBox);
    layout->addWidget(_groupBox);

    setLayout(layout);
}

void NodeManagerWidget::odNotify(quint16 index, quint8 subindex, const QVariant &value)
{
    if (index == 0x1000 && subindex == 0x00)
    {
        _index1000Label->setNum(value.toInt());
    }
}
