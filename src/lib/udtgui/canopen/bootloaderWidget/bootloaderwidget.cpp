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

#include "bootloaderwidget.h"

#include "bootloader/bootloader.h"
#include "canopen/indexWidget/indexlabel.h"
#include "indexdb.h"
#include "indexdb402.h"
#include "node.h"

#include <QFileDialog>
#include <QFormLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QPushButton>

BootloaderWidget::BootloaderWidget(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("UniSwarm UDTStudio - Update Node");
    createWidgets();
}

Node *BootloaderWidget::node() const
{
    return _node;
}

void BootloaderWidget::setNode(Node *node)
{
    if (!node)
    {
        return;
    }

    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->setNode(node);
    }

    _node = node;
    _bootloader = _node->bootloader();

    connect(_updateButton, &QPushButton::clicked, this, &BootloaderWidget::updateProgram);
    connect(_bootloader, &Bootloader::status, this, &BootloaderWidget::updateStatusLabel);
    connect(_bootloader, &Bootloader::parserUfwFinished, this, &BootloaderWidget::updateFileInformation);
}

void BootloaderWidget::readAll()
{
    for (AbstractIndexWidget *indexWidget : _indexWidgets)
    {
        indexWidget->readObject();
    }
}

void BootloaderWidget::updateProgram()
{
    _bootloader->startUpdate();
}

void BootloaderWidget::updateStatusLabel(QString string)
{
    _statusLabel->setText(string);
}

void BootloaderWidget::updateFileInformation()
{
    _deviceTypeUfwLabel->setText("0x" + QString::number(_bootloader->deviceType(), 16));
    _versionSoftwareUfwLabel->setText(_bootloader->versionSoftware());
    _buildDateUfwLabel->setText(_bootloader->buildDate());
}

void BootloaderWidget::openFile()
{
    _fileName = QFileDialog::getOpenFileName(this, tr("Open ufw"), "/home/julien/Seafile/myLibrary/2_FW/4_UIO_fw/UIO8AD/build/", tr("Image File (*.ufw)"));
    _fileUfwLabel->setText(_fileName);
    _bootloader->openUfw(_fileName);
}

void BootloaderWidget::createWidgets()
{
    QVBoxLayout *vLayout = new QVBoxLayout();

    vLayout->addWidget(informationDeviceWidget());

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);
    hLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    QLabel *iconLabel = new QLabel();
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    iconLabel->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    iconLabel->setPixmap(QPixmap(":/icons/img/icons8-tri-decroissant-48.png"));
    hLayout->addWidget(iconLabel);

    hLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    vLayout->addLayout(hLayout);

    vLayout->addWidget(informationFileWidget());

    QFormLayout *layout = new QFormLayout();
    _statusLabel = new QLabel();
    layout->addRow("Status:", _statusLabel);
    vLayout->addLayout(layout);

    _updateButton = new QPushButton("Start update");
    vLayout->addWidget(_updateButton);

    setLayout(vLayout);
}

QGroupBox *BootloaderWidget::informationDeviceWidget()
{
    QGroupBox *groupBox = new QGroupBox(tr("Node Information"));
    QHBoxLayout *hlayout = new QHBoxLayout();

    QFormLayout *fileLayout = new QFormLayout();

    IndexLabel *indexLabel;

    indexLabel = new IndexLabel(IndexDb::getObjectId(IndexDb::OD_PRODUCT_CODE));
    indexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    indexLabel->setCursor(Qt::IBeamCursor);
    indexLabel->setDisplayHint(IndexLabel::DisplayHexa);
    fileLayout->addRow(tr("&Product Code:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(IndexDb::getObjectId(IndexDb::OD_MANUFACTURER_SOFTWARE_VERSION));
    indexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    indexLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("&Software version:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(IndexDb402::getObjectId(IndexDb402::OD_MS_FIRMWARE_BUILD_DATE));
    indexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    indexLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("Software &build date:"), indexLabel);
    _indexWidgets.append(indexLabel);

    hlayout->addItem(fileLayout);

    groupBox->setLayout(hlayout);
    return groupBox;
}

QGroupBox *BootloaderWidget::informationFileWidget()
{
    QGroupBox *groupBox = new QGroupBox(tr("File Information"));

    QFormLayout *fileLayout = new QFormLayout();

    QPushButton *openButton = new QPushButton(tr("Open Ufw"));
    connect(openButton, &QPushButton::clicked, this, &BootloaderWidget::openFile);

    fileLayout->addRow(openButton);

    _fileUfwLabel = new QLabel();
    _fileUfwLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _fileUfwLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("&File:"), _fileUfwLabel);

    _deviceTypeUfwLabel = new QLabel();
    _deviceTypeUfwLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _deviceTypeUfwLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("&Product Code:"), _deviceTypeUfwLabel);

    _versionSoftwareUfwLabel = new QLabel();
    _versionSoftwareUfwLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _versionSoftwareUfwLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("&Software version:"), _versionSoftwareUfwLabel);

    _buildDateUfwLabel = new QLabel();
    _buildDateUfwLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _buildDateUfwLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("Software &build date:"), _buildDateUfwLabel);

    groupBox->setLayout(fileLayout);
    return groupBox;
}
