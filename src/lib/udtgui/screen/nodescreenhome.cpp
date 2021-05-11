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
    layout->addWidget(createStoreWidget());
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
    sumaryLayout->addRow(tr("&Device name :"), indexLabel);
    _indexWidgets.append(indexLabel);

    _summaryProfileLabel = new QLabel();
    sumaryLayout->addRow(tr("&Profile :"), _summaryProfileLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x1009, 0));
    sumaryLayout->addRow(tr("&Hardware version :"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2001, 0));
    sumaryLayout->addRow(tr("Manufacture &date :"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x100A, 0));
    sumaryLayout->addRow(tr("&Software version :"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2003, 0));
    sumaryLayout->addRow(tr("Software &build :"), indexLabel);
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
    sumaryLayout->addRow(tr("Board voltage :"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2020, 1));
    indexLabel->setDisplayHint(AbstractIndexWidget::DisplayFloat);
    indexLabel->setScale(1.0 / 10.0);
    indexLabel->setUnit(" °C");
    sumaryLayout->addRow(tr("CPU temperature :"), indexLabel);
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
    odLayout->addRow(tr("&Eds file :"), _odEdsFileLabel);

    _odCountLabel = new QLabel();
    odLayout->addRow(tr("Index &count :"), _odCountLabel);

    _odSubIndexCountLabel = new QLabel();
    odLayout->addRow(tr("&Sub index count :"), _odSubIndexCountLabel);

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

QWidget *NodeScreenHome::createStoreWidget()
{
    QGroupBox *groupBox = new QGroupBox(tr("Store/Restore"));
    QFormLayout *layout = new QFormLayout();

    QHBoxLayout *storeLayout = new QHBoxLayout();
    _storeComboBox = new QComboBox();
    _storeComboBox->insertItem(1, "Save all Parameters");
    _storeComboBox->insertItem(2, "Save Communication Parameters");
    _storeComboBox->insertItem(3, "Save Application Parameters");
    _storeComboBox->insertItem(4, "Save Manufacturer Parameters");
    storeLayout->addWidget(_storeComboBox);

    QPushButton *storeButon = new QPushButton("Store");
    connect(storeButon, &QPushButton::clicked, this, &NodeScreenHome::storeClicked);
    storeLayout->addWidget(storeButon);

    layout->addRow("Store :", storeLayout);

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
    connect(restoreButon, &QPushButton::clicked, this, &NodeScreenHome::restoreClicked);
    restoreLayout->addWidget(restoreButon);

    layout->addRow("Restore :", restoreLayout);

    groupBox->setLayout(layout);
    return groupBox;
}

void NodeScreenHome::storeClicked()
{
    quint32 save = 0x65766173;

    bool ok;
    QString text = QInputDialog::getText(this, tr("Store"), tr("Signature:"), QLineEdit::Normal, "0x" + QString::number(save, 16).toUpper(), &ok);
    if (ok && !text.isEmpty())
    {
        quint8 id = static_cast<uint8_t>(_storeComboBox->currentIndex()) + 1;
        _node->writeObject(NodeObjectId(0x1010, id), text.toUInt(&ok, 16));
    }
}

void NodeScreenHome::restoreClicked()
{
    quint32 save = 0x64616F6C;

    bool ok;
    QString text = QInputDialog::getText(this, tr("Restore"), tr("Signature:"), QLineEdit::Normal, "0x" + QString::number(save, 16).toUpper(), &ok);
    if (ok && !text.isEmpty())
    {
        quint8 id = static_cast<uint8_t>(_restoreComboBox->currentIndex()) + 1;
        _node->writeObject(NodeObjectId(0x1010, id), text.toUInt(&ok, 16));
    }
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
