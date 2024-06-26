/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QToolBar>

#include "canopen/indexWidget/indexconsumerheartbeat.h"
#include "canopen/indexWidget/indexspinbox.h"

NodeScreenNMT::NodeScreenNMT(QWidget *parent)
    : NodeScreen(parent)
{
    createWidgets();
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
    actionReadMappings->setIcon(QIcon(QStringLiteral(":/icons/img/icons8-update.png")));
    actionReadMappings->setShortcut(QKeySequence(QStringLiteral("Ctrl+R")));
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
    layout->addWidget(createConsumerHeartBeatWidget());
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
    addIndexWidget(indexSpinBox);

    QLabel *producerStatusLabel = new QLabel();
    connect(indexSpinBox,
            &IndexSpinBox::valueChanged,
            this,
            [=](double value)
            {
                if (value == 0)
                {
                    producerStatusLabel->setText(tr("Producer heartbeat disabled"));
                }
                else
                {
                    producerStatusLabel->setText(tr("Producer heartbeat emited each %n milisecond(s)", nullptr, static_cast<int>(value)));
                }
            });
    formLayout->addWidget(producerStatusLabel);

    return groupBox;
}

QWidget *NodeScreenNMT::createConsumerHeartBeatWidget()
{
    QGroupBox *groupBox = new QGroupBox(tr("Consumer heartbeat"));
    QGridLayout *gridLayout = new QGridLayout(groupBox);

    gridLayout->addWidget(new QLabel(tr("Node ID")), 0, 1);
    gridLayout->addWidget(new QLabel(tr("Dead time")), 0, 2);

    for (int row = 1; row <= 10; row++)
    {
        gridLayout->addWidget(new QLabel(QStringLiteral("%1").arg(row)), row, 0);

        IndexConsumerHeartBeat *indexConsumerSpinBox = new IndexConsumerHeartBeat(NodeObjectId(0x1016, row));
        gridLayout->addWidget(indexConsumerSpinBox, row, 1, 1, 2);
        addIndexWidget(indexConsumerSpinBox);
    }

    return groupBox;
}

QString NodeScreenNMT::title() const
{
    return tr("NMT");
}

void NodeScreenNMT::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(node)
    Q_UNUSED(axis)
}
