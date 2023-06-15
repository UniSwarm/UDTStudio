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

#include "nodescreennmt.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QScrollArea>
#include <QToolBar>
#include <QLabel>

#include "canopen/indexWidget/indexspinbox.h"

NodeScreenNMT::NodeScreenNMT()
{
    createWidgets();
}

void NodeScreenNMT::readAll()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->readObject();
    }
}

void NodeScreenNMT::createWidgets()
{
    QLayout *glayout = new QVBoxLayout();
    glayout->setContentsMargins(0, 0, 0, 0);

    QLayout *toolBarLayout = new QVBoxLayout();
    toolBarLayout->setContentsMargins(2, 2, 2, 0);
    QToolBar *toolBar = new QToolBar(tr("Node screen commands"));
    toolBar->setIconSize(QSize(20, 20));

    // read all action
    QAction *actionReadMappings = toolBar->addAction(tr("Read all"));
    actionReadMappings->setIcon(QIcon(":/icons/img/icons8-update.png"));
    actionReadMappings->setShortcut(QKeySequence("Ctrl+R"));
    actionReadMappings->setStatusTip(tr("Read all the objects of the current window"));
    connect(actionReadMappings, &QAction::triggered, this, &NodeScreenNMT::readAll);

    toolBarLayout->addWidget(toolBar);
    glayout->addItem(toolBarLayout);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    QWidget *widget = new QWidget(this);
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    layout->addWidget(createProducerHeartBeatWidget());
    widget->setLayout(layout);
    scrollArea->setWidget(widget);

    glayout->addWidget(scrollArea);
    setLayout(glayout);
}

QWidget *NodeScreenNMT::createProducerHeartBeatWidget()
{
    QGroupBox *groupBox = new QGroupBox(tr("Producer heartbeat"));
    QFormLayout *formLayout = new QFormLayout(groupBox);

    IndexSpinBox *indexSpinBox;
    indexSpinBox = new IndexSpinBox(NodeObjectId(0x1017, 0));
    formLayout->addRow(tr("Delay:"), indexSpinBox);
    _indexWidgets.append(indexSpinBox);

    QLabel *producerStatusLabel = new QLabel();
    connect(indexSpinBox, &IndexSpinBox::valueChanged, this, [=](double value)
    {
        if (value == 0)
        {
            producerStatusLabel->setText(tr("Producer heartbeat disabled"));
        }
        else
        {
            producerStatusLabel->setText(tr("Producer heartbeat emited each %n milisecond(s)", nullptr, value));
        }
    });
    formLayout->addWidget(producerStatusLabel);

    return groupBox;
}

QString NodeScreenNMT::title() const
{
    return tr("NMT");
}

void NodeScreenNMT::setNodeInternal(Node *node, uint8_t axis)
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->setNode(node);
    }
}
