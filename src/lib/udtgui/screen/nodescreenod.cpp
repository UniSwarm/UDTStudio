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
    _storeRestoreGroupBox = new QGroupBox(tr("Store/Restore"));
    QFormLayout *layout = new QFormLayout();
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(5);

    QHBoxLayout *storeLayout = new QHBoxLayout();
    _storeComboBox = new QComboBox();
    _storeComboBox->insertItem(1, "Save all Parameters");
    _storeComboBox->insertItem(2, "Save Communication Parameters");
    _storeComboBox->insertItem(3, "Save Application Parameters");
    _storeComboBox->insertItem(4, "Save Manufacturer Parameters");
    storeLayout->addWidget(_storeComboBox);

    QPushButton *storeButon = new QPushButton("Store");
    connect(storeButon, &QPushButton::clicked, this, &NodeScreenOD::storeClicked);
    storeLayout->addWidget(storeButon);

    layout->addRow("Store:", storeLayout);

    QHBoxLayout *restoreLayout = new QHBoxLayout();
    _restoreComboBox = new QComboBox();
    _restoreComboBox->insertItem(1, "Restore all Factory Parameters");
    _restoreComboBox->insertItem(2, "Restore Factory Communication Parameters");
    _restoreComboBox->insertItem(3, "Restore Factory Application Parameters");
    _restoreComboBox->insertItem(4, "Restore Factory Manufacturer Parameters");
    _restoreComboBox->insertItem(5, "Restore all saved Parameters");
    _restoreComboBox->insertItem(6, "Restore saved Communication Parameters");
    _restoreComboBox->insertItem(7, "Restore saved Application Parameters");
    _restoreComboBox->insertItem(8, "Restore saved Manufacturer Parameters");
    restoreLayout->addWidget(_restoreComboBox);

    QPushButton *restoreButon = new QPushButton("Restore");
    connect(restoreButon, &QPushButton::clicked, this, &NodeScreenOD::restoreClicked);
    restoreLayout->addWidget(restoreButon);

    layout->addRow("Restore:", restoreLayout);

    _storeRestoreGroupBox->setLayout(layout);
    return _storeRestoreGroupBox;
}

void NodeScreenOD::storeClicked()
{
    quint32 save = 0x65766173;

    bool ok;
    QString text = QInputDialog::getText(this, tr("Store"), tr("Signature:"), QLineEdit::Normal, "0x" + QString::number(save, 16).toUpper(), &ok);
    if (ok && !text.isEmpty())
    {
        quint8 id = static_cast<uint8_t>(_storeComboBox->currentIndex()) + 1;
        _node->nodeOd()->store(id, text.toUInt(&ok, 16));
    }
}

void NodeScreenOD::restoreClicked()
{
    quint32 save = 0x64616F6C;

    bool ok;
    QString text = QInputDialog::getText(this, tr("Restore"), tr("Signature:"), QLineEdit::Normal, "0x" + QString::number(save, 16).toUpper(), &ok);
    if (ok && !text.isEmpty())
    {
        quint8 id = static_cast<uint8_t>(_restoreComboBox->currentIndex()) + 1;
        _node->nodeOd()->restore(id, text.toUInt(&ok, 16));
    }
}

void NodeScreenOD::statusNodeChanged()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED ||  _node->status() == Node::STOPPED)
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
    connect(node, &Node::statusChanged, this, &NodeScreenOD::statusNodeChanged);
    statusNodeChanged();
}
