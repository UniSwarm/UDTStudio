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

#include "nodescreenod.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QPushButton>

#include "udtguimanager.h"

NodeScreenOD::NodeScreenOD()
{
    createWidgets();
}

void NodeScreenOD::createWidgets()
{
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(0);

    _nodeOdWidget = new NodeOdWidget();
    layout->addWidget(_nodeOdWidget);

    layout->addWidget(createStoreWidget());

    setLayout(layout);
}

QWidget *NodeScreenOD::createStoreWidget()
{
    _storeRestoreGroupBox = new QGroupBox(tr("Store / restore"));
    QFormLayout *layout = new QFormLayout();
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);

    QHBoxLayout *storeLayout = new QHBoxLayout();
    _storeComboBox = new QComboBox();
    _storeComboBox->addItem(tr("Save all parameters"), QVariant(Node::StoreAll));
    _storeComboBox->addItem(tr("Save communication parameters"), QVariant(Node::StoreCom));
    _storeComboBox->addItem(tr("Save application parameters"), QVariant(Node::StoreApp));
    _storeComboBox->addItem(tr("Save manufacturer parameters"), QVariant(Node::StoreMan));
    storeLayout->addWidget(_storeComboBox);

    QPushButton *storeButon = new QPushButton("Store");
    connect(storeButon, &QPushButton::clicked, this, &NodeScreenOD::store);
    storeLayout->addWidget(storeButon);

    layout->addRow("Store:", storeLayout);

    QHBoxLayout *restoreLayout = new QHBoxLayout();
    _restoreComboBox = new QComboBox();
    _restoreComboBox->addItem(tr("Restore all factory parameters"), QVariant(Node::RestoreFactoryAll));
    _restoreComboBox->addItem(tr("Restore factory communication parameters"), QVariant(Node::RestoreFactoryCom));
    _restoreComboBox->addItem(tr("Restore factory application parameters"), QVariant(Node::RestoreFactoryApp));
    _restoreComboBox->addItem(tr("Restore factory manufacturer parameters"), QVariant(Node::RestoreFactoryMan));
    _restoreComboBox->addItem(tr("Restore all saved parameters"), QVariant(Node::RestoreSavedAll));
    _restoreComboBox->addItem(tr("Restore saved communication parameters"), QVariant(Node::RestoreSavedCom));
    _restoreComboBox->addItem(tr("Restore saved application parameters"), QVariant(Node::RestoreSavedApp));
    _restoreComboBox->addItem(tr("Restore saved manufacturer parameters"), QVariant(Node::RestoreSavedMan));
    restoreLayout->addWidget(_restoreComboBox);

    QPushButton *restoreButon = new QPushButton("Restore");
    connect(restoreButon, &QPushButton::clicked, this, &NodeScreenOD::restore);
    restoreLayout->addWidget(restoreButon);

    layout->addRow("Restore:", restoreLayout);

    _storeRestoreGroupBox->setLayout(layout);
    return _storeRestoreGroupBox;
}

void NodeScreenOD::store()
{
    Node::StoreSegment segment = static_cast<Node::StoreSegment>(_storeComboBox->currentData().toInt());
    node()->store(segment);
}

void NodeScreenOD::restore()
{
    Node::RestoreSegment segment = static_cast<Node::RestoreSegment>(_restoreComboBox->currentData().toInt());
    node()->restore(segment);
}

void NodeScreenOD::updateStatusNode()
{
    if (node() != nullptr)
    {
        if (node()->status() == Node::STARTED || node()->status() == Node::STOPPED)
        {
            _storeRestoreGroupBox->setEnabled(false);
        }
        else
        {
            _storeRestoreGroupBox->setEnabled(true);
        }
    }
}

QString NodeScreenOD::title() const
{
    return QString(tr("OD"));
}

void NodeScreenOD::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(axis)
    _nodeOdWidget->setNode(node);
    connect(node, &Node::statusChanged, this, &NodeScreenOD::updateStatusNode);
    updateStatusNode();
    UdtGuiManager::setNodeOdWidget(node, _nodeOdWidget);
}
