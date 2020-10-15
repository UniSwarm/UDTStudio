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
#include "indexdb.h"

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
        if (((_node->profileNumber()) == 0x192) && (_node->nodeOd()->value(IndexDb::getObjectId(IndexDb::OD_VENDOR_ID)).toUInt() == 0x4A2))
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
    _groupNmt->setEnabled(_node);

    updateData();
}

void NodeManagerWidget::updateData()
{
    if (_node)
    {
        _nodeNameEdit->setText(_node->name());
        _nodeStatusLabel->setText(_node->statusStr());

        // status button update
        _groupNmt->blockSignals(true);
        for (QAction *action : _groupNmt->actions())
        {
            action->setChecked(false);
        }
        int idAction = _node->status() - 1;
        if (idAction < 0)
        {
            idAction = 0;
        }
        QAction *actionStatus = _groupNmt->actions()[static_cast<int>(idAction)];
        actionStatus->setCheckable(true);
        actionStatus->setChecked(true);
        _groupNmt->blockSignals(false);
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
        updateData();
    }
}

void NodeManagerWidget::resetNode()
{
    if (_node)
    {
        _node->sendResetNode();
        updateData();
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
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    _groupBox = new QGroupBox(tr("Node"));
    QFormLayout *layoutGroupBox = new QFormLayout();
    layoutGroupBox->setSpacing(2);
    layoutGroupBox->setContentsMargins(5, 5, 5, 5);

    // toolbar nmt
    _toolBar = new QToolBar(tr("Node commands"));
    _toolBar->setIconSize(QSize(20, 20));

    _groupNmt = new QActionGroup(this);
    _groupNmt->setExclusive(true);

    _actionPreop = _groupNmt->addAction(tr("Pre operationnal"));
    _actionPreop->setCheckable(true);
    _actionPreop->setIcon(QIcon(":/icons/img/icons8-connection-status-on.png"));
    _actionPreop->setStatusTip(tr("Request node to go in preop mode"));
    connect(_actionPreop, &QAction::triggered, this, &NodeManagerWidget::preop);

    _actionStart = _groupNmt->addAction(tr("Start"));
    _actionStart->setCheckable(true);
    _actionStart->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _actionStart->setStatusTip(tr("Request node to go in started mode"));
    connect(_actionStart, &QAction::triggered, this, &NodeManagerWidget::start);

    _actionStop = _groupNmt->addAction(tr("Stop"));
    _actionStop->setCheckable(true);
    _actionStop->setIcon(QIcon(":/icons/img/icons8-stop.png"));
    _actionStop->setStatusTip(tr("Request node to go in stop mode"));
    connect(_actionStop, &QAction::triggered, this, &NodeManagerWidget::stop);

    _actionResetCom = _groupNmt->addAction(tr("Reset communication"));
    _actionResetCom->setCheckable(true);
    _actionResetCom->setIcon(QIcon(":/icons/img/icons8-process.png"));
    _actionResetCom->setStatusTip(tr("Request node to reset com. parameters"));
    connect(_actionResetCom, &QAction::triggered, this, &NodeManagerWidget::resetCom);

    _actionReset = _groupNmt->addAction(tr("Reset node"));
    _actionReset->setCheckable(true);
    _actionReset->setIcon(QIcon(":/icons/img/icons8-reset.png"));
    _actionReset->setStatusTip(tr("Request node to reset all values"));
    connect(_actionReset, &QAction::triggered, this, &NodeManagerWidget::resetNode);

    _action402 = new QAction();
    _action402->setCheckable(true);
    _action402->setIcon(QIcon(":/icons/img/icons8-stepper-motor.png"));
    _action402->setStatusTip(tr("402 Management"));
    connect(_action402, &QAction::triggered, this, &NodeManagerWidget::displayMode);

    _toolBar->addActions(_groupNmt->actions());
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

QAction *NodeManagerWidget::actionReset() const
{
    return _actionReset;
}

QAction *NodeManagerWidget::actionResetCom() const
{
    return _actionResetCom;
}

QAction *NodeManagerWidget::actionStop() const
{
    return _actionStop;
}

QAction *NodeManagerWidget::actionStart() const
{
    return _actionStart;
}

QAction *NodeManagerWidget::actionPreop() const
{
    return _actionPreop;
}

void NodeManagerWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (objId.index() == 0x1000 && objId.subIndex() == 0x00)
    {
        //_index1000Label->setNum(flags);
    }
}
