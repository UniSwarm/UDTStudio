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
#include <QInputDialog>
#include <QPixmap>
#include <QPushButton>

#include "canopen/widget/indexcombobox.h"
#include "canopen/widget/indexlabel.h"
#include "screen/nodescreenswidget.h"

NodeScreenHome::NodeScreenHome()
{
    createWidgets();
}

void NodeScreenHome::createWidgets()
{
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);
    layout->setSpacing(2);

    layout->addWidget(createSumaryWidget());
    layout->addWidget(createStatusWidget());
    layout->addWidget(createOdWidget());
    setLayout(layout);
}

QWidget *NodeScreenHome::createSumaryWidget()
{
    QGroupBox *groupBox = new QGroupBox(tr("Summary"));
    QHBoxLayout *hlayout = new QHBoxLayout();

    _summaryIconLabel = new QLabel();
    _summaryIconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    _summaryIconLabel->setAlignment(Qt::AlignTop | Qt::AlignRight);
    hlayout->addWidget(_summaryIconLabel);

    QFormLayout *sumaryLayout = new QFormLayout();

    IndexLabel *indexLabel;
    indexLabel = new IndexLabel(NodeObjectId(0x1008, 0));
    sumaryLayout->addRow(tr("&Device name:"), indexLabel);
    _indexWidgets.append(indexLabel);

    _summaryProfileLabel = new QLabel();
    sumaryLayout->addRow(tr("&Profile:"), _summaryProfileLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x1009, 0));
    sumaryLayout->addRow(tr("&Hardware version:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2001, 0));
    sumaryLayout->addRow(tr("Manufacture &date:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x100A, 0));
    sumaryLayout->addRow(tr("&Software version:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2003, 0));
    sumaryLayout->addRow(tr("Software &build:"), indexLabel);
    _indexWidgets.append(indexLabel);

    hlayout->addItem(sumaryLayout);
    groupBox->setLayout(hlayout);
    return groupBox;
}

QWidget *NodeScreenHome::createStatusWidget()
{
    IndexLabel *indexLabel;
    QGroupBox *groupBox = new QGroupBox(tr("Status"));
    QFormLayout *sumaryLayout = new QFormLayout();

    indexLabel = new IndexLabel(NodeObjectId(0x2000, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 100.0);
    indexLabel->setUnit(" V");
    sumaryLayout->addRow(tr("Board voltage:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2020, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 10.0);
    indexLabel->setUnit(" °C");
    sumaryLayout->addRow(tr("CPU temperature:"), indexLabel);
    _indexWidgets.append(indexLabel);

    groupBox->setLayout(sumaryLayout);
    return groupBox;
}

QWidget *NodeScreenHome::createOdWidget()
{
    QGroupBox *groupBox = new QGroupBox(tr("Object dictionary"));
    QHBoxLayout *hlayout = new QHBoxLayout();

    QFormLayout *odLayout = new QFormLayout();
    _odEdsFileLabel = new QLabel();
    _odEdsFileLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _odEdsFileLabel->setCursor(Qt::IBeamCursor);
    odLayout->addRow(tr("&Eds file:"), _odEdsFileLabel);

    _odCountLabel = new QLabel();
    odLayout->addRow(tr("Index &count:"), _odCountLabel);

    _odSubIndexCountLabel = new QLabel();
    odLayout->addRow(tr("&Sub index count:"), _odSubIndexCountLabel);

    hlayout->addItem(odLayout);

    QVBoxLayout *buttonlayout = new QVBoxLayout();
    QPushButton *goODButton = new QPushButton(tr("Go to OD tab"));
    goODButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    connect(goODButton, &QPushButton::released, [=]() { screenWidget()->setActiveTab("od"); });
    buttonlayout->addWidget(goODButton);
    buttonlayout->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    hlayout->addItem(buttonlayout);

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
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->setNode(node);
    }

    if (node)
    {
        _summaryProfileLabel->setText(QString("DS%1").arg(node->profileNumber()));
        if (node->manufacturerId() == 0x04A2) // UniSwarm
        {
            switch (node->profileNumber())
            {
            case 401:
                _summaryIconLabel->setPixmap(QPixmap(":/uBoards/uio.png"));
                break;

            case 402:
                _summaryIconLabel->setPixmap(QPixmap(":/uBoards/umc.png"));
                break;
            }
        }
        _odEdsFileLabel->setText(node->edsFileName());
        _odCountLabel->setNum(node->nodeOd()->indexCount());
        _odSubIndexCountLabel->setNum(node->nodeOd()->subIndexCount());
    }
    else
    {
        _summaryProfileLabel->clear();
        _summaryIconLabel->clear();

        _odEdsFileLabel->clear();
        _odCountLabel->clear();
        _odSubIndexCountLabel->clear();
    }
}
