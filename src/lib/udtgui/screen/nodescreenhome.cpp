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

#include "nodescreenhome.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QPixmap>

#include <canopen/widget/indexlabel.h>

NodeScreenHome::NodeScreenHome()
{
    createWidgets();
}

void NodeScreenHome::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(createSumaryWidget());

    setLayout(layout);
}

QWidget *NodeScreenHome::createSumaryWidget()
{
    QGroupBox *groupBox = new QGroupBox(tr("Summary"));
    QHBoxLayout *hlayout = new QHBoxLayout();

    _iconLabel = new QLabel();
    _iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    _iconLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);
    hlayout->addWidget(_iconLabel);

    QFormLayout *sumaryLayout = new QFormLayout();

    IndexLabel *indexLabel;
    indexLabel = new IndexLabel(NodeObjectId(0x1008, 0));
    sumaryLayout->addRow(tr("&Device name :"), indexLabel);
    _indexWidgets.append(indexLabel);

    _profileLabel = new QLabel();
    sumaryLayout->addRow(tr("&Profile :"), _profileLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x1009, 0));
    sumaryLayout->addRow(tr("&Hardware version :"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x100A, 0));
    sumaryLayout->addRow(tr("&Software version :"), indexLabel);
    _indexWidgets.append(indexLabel);

    hlayout->addItem(sumaryLayout);
    groupBox->setLayout(hlayout);
    return groupBox;
}

QString NodeScreenHome::title() const
{
    return QString(tr("Node"));
}

void NodeScreenHome::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(axis)
    for (AbstractIndexWidget *indexWidget: _indexWidgets)
    {
        indexWidget->setNode(node);
    }

    if (node)
    {
        _profileLabel->setText(QString("DS%1").arg(node->profileNumber()));
        if (node->manufacturerId() == 0x04A2) // UniSwarm
        {
            switch (node->profileNumber())
            {
            case 401:
                _iconLabel->setPixmap(QPixmap(":/uBoards/uio.png"));
                break;

            case 402:
                _iconLabel->setPixmap(QPixmap(":/uBoards/umc.png"));
                break;
            }
        }
    }
    else
    {
        _profileLabel->clear();
        _iconLabel->clear();
    }
}
