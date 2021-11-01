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

#include "nodemanagerwidget.h"

#include <QDebug>
#include <QFileInfo>
#include <QFileDialog>
#include <QFormLayout>

#include "canopenbus.h"
#include "services/services.h"
#include "indexdb.h"

#include "canopen/bootloaderWidget/bootloaderwidget.h"

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

    _node = node;

    if (_node)
    {
        connect(_node, &Node::statusChanged, this, &NodeManagerWidget::updateData);
    }
    _groupBox->setEnabled(_node);
    _groupNmt->setEnabled(_node);

    _actionLoadEds->setEnabled(_node);
    _actionReLoadEds->setEnabled(_node);

    _actionRemoveNode->setEnabled(_node);

    _actionUpdateFirmware->setEnabled(_node);

    updateData();
}

void NodeManagerWidget::updateData()
{
    if (_node)
    {
        _nodeNameEdit->setText(_node->name());
        _nodeStatusLabel->setText(_node->statusStr());

        _edsFileNameLabel->setText(QFileInfo(_node->edsFileName()).fileName());
        _edsFileNameLabel->setToolTip(_node->edsFileName());

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

void NodeManagerWidget::removeNode()
{
    if (_node)
    {
        Node *node = _node;
        setNode(nullptr);
        node->bus()->removeNode(node);
    }
}

void NodeManagerWidget::updateNodeFirmware()
{
    if (_node)
    {
        BootloaderWidget bootloaderWidget(_node, this);
        bootloaderWidget.exec();
    }
}

void NodeManagerWidget::loadEds(const QString &edsFileName)
{
    if (_node)
    {
        QString fileName = edsFileName;
        if (fileName.isEmpty())
        {
            fileName = QFileDialog::getOpenFileName(this, tr("Choose eds file"), QString(), tr("Eds file (*.eds)"));
            if (fileName.isEmpty())
            {
                return;
            }
        }
        _node->loadEds(fileName);
        updateData();
    }
}

void NodeManagerWidget::reloadEds()
{
    if (_node)
    {
        _node->loadEds(_node->edsFileName());
    }
}

void NodeManagerWidget::setNodeName()
{
    if (_node)
    {
        _node->setName(_nodeNameEdit->text());
    }
}

void NodeManagerWidget::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    _groupBox = new QGroupBox(tr("Node"));
    QFormLayout *layoutGroupBox = new QFormLayout();
    layoutGroupBox->setSpacing(2);
    layoutGroupBox->setContentsMargins(2, 2, 2, 2);

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

    _toolBar->addActions(_groupNmt->actions());

    // Remove node
    _actionRemoveNode = new QAction(tr("Remove node"));
    _actionRemoveNode->setIcon(QIcon(":/icons/img/icons8-delete.png"));
    _actionRemoveNode->setStatusTip(tr("Remove the current node from the bus"));
    connect(_actionRemoveNode, &QAction::triggered, this, &NodeManagerWidget::removeNode);

    // Update firmware
    _actionUpdateFirmware = new QAction(tr("Update firmware"));
    _actionUpdateFirmware->setIcon(QIcon(":/icons/img/icons8-restore-page.png"));
    _actionUpdateFirmware->setStatusTip(tr("Launch a firmware update to the node"));
    connect(_actionUpdateFirmware, &QAction::triggered, this, &NodeManagerWidget::updateNodeFirmware);

    // EDS actions
    _actionLoadEds = new QAction(tr("Load eds"));
    _actionLoadEds->setIcon(QIcon(":/icons/img/icons8-import-file.png"));
    _actionLoadEds->setStatusTip(tr("Load an eds file as object dictionary description"));
    connect(_actionLoadEds, &QAction::triggered,  [=](){loadEds();});

    _actionReLoadEds = new QAction(tr("Reload eds"));
    _actionReLoadEds->setIcon(QIcon(":/icons/img/icons8-restore-page.png"));
    _actionReLoadEds->setStatusTip(tr("Reload the current eds file"));
    connect(_actionReLoadEds, &QAction::triggered, this, &NodeManagerWidget::reloadEds);

    layoutGroupBox->addRow(_toolBar);
    layoutGroupBox->addItem(new QSpacerItem(0, 2));

    _nodeNameEdit = new QLineEdit();
    layoutGroupBox->addRow(tr("Name:"), _nodeNameEdit);
    connect(_nodeNameEdit, &QLineEdit::returnPressed, this, &NodeManagerWidget::setNodeName);

    _nodeStatusLabel = new QLabel();
    layoutGroupBox->addRow(tr("Status:"), _nodeStatusLabel);

    _edsFileNameLabel = new QLabel();
    _edsFileNameLabel->setTextInteractionFlags(Qt::TextSelectableByMouse);
    layoutGroupBox->addRow(tr("Eds file name:"), _edsFileNameLabel);

    _groupBox->setLayout(layoutGroupBox);
    layout->addWidget(_groupBox);

    layout->setSizeConstraint(QLayout::SetMinimumSize);
    setLayout(layout);
}

QAction *NodeManagerWidget::actionRemoveNode() const
{
    return _actionRemoveNode;
}

QAction *NodeManagerWidget::actionUpdateFirmware() const
{
    return _actionUpdateFirmware;
}

QAction *NodeManagerWidget::actionReLoadEds() const
{
    return _actionReLoadEds;
}

QAction *NodeManagerWidget::actionLoadEds() const
{
    return _actionLoadEds;
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
