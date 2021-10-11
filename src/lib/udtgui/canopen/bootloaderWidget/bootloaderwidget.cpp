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

    //    connect(_stopButton, &QPushButton::clicked, _bootloader, &Bootloader::stopProgram);
    //    connect(_startButton, &QPushButton::clicked, _bootloader, &Bootloader::startProgram);
    //    connect(_resetButton, &QPushButton::clicked, _bootloader, &Bootloader::resetProgram);
    //    connect(_clearButton, &QPushButton::clicked, _bootloader, &Bootloader::clearProgram);
    //    connect(_updateButton, &QPushButton::clicked, this, &BootloaderWidget::updateProgram);
    //    connect(_sendKeyButton, &QPushButton::clicked, this, &BootloaderWidget::sendKeyButton);
}

void BootloaderWidget::readAll()
{
    for (AbstractIndexWidget *indexWidget : _indexWidgets)
    {
        indexWidget->readObject();
    }
}

void BootloaderWidget::sendKeyButton()
{
    quint16 save = 4001;

    //    bool ok;
    //    QString text = QInputDialog::getText(this, tr("Bootloader"), tr("Key:"), QLineEdit::Normal, "0x" + QString::number(save, 16).toUpper(), &ok);
    //    if (ok && !text.isEmpty())
    //    {
    _bootloader->sendKey(static_cast<uint16_t>(save));
    //    }
}

void BootloaderWidget::updateProgram()
{
    _bootloader->startUpdate();
}

void BootloaderWidget::openFile()
{
    // QString fileName = ("/home/julien/Seafile/myLibrary/2_FW/4_UIO_fw/UIO8AD/build/uio8ad.ufw");
    _fileName = QFileDialog::getOpenFileName(this, tr("Open ufw"), "/home/julien/Seafile/myLibrary/2_FW/4_UIO_fw/UIO8AD/build/", tr("Image File (*.ufw)"));
    _fileUfwLabel->setText(_fileName);

    _bootloader->openUfw(_fileName);

    _vendorIdUfwLabel->setText(QString::number(_bootloader->vendorId()));
    _productCodeUfwLabel->setText(QString::number(_bootloader->productCode()));
    _revisionNumberUfwLabel->setText(QString::number(_bootloader->revisionNumber()));
    _versionSoftwareUfwLabel->setText(QString::number(_bootloader->versionSoftware()));
    _buildDateUfwLabel->setText(_bootloader->buildDate());
}

void BootloaderWidget::createWidgets()
{
    QFormLayout *layout = new QFormLayout();

    layout->addWidget(informationDeviceWidget());
    layout->addWidget(informationFileWidget());

    _updateButton = new QPushButton("Start update");
    layout->addRow(_updateButton);

    //    _stopButton = new QPushButton("Stop program");
    //    layout->addRow(_stopButton);

    //    _startButton = new QPushButton("Start program");
    //    layout->addRow(_startButton);

    //    _resetButton = new QPushButton("Reset program");
    //    layout->addRow(_resetButton);

    //    _clearButton = new QPushButton("Clear program");
    //    layout->addRow(_clearButton);

    //    _sendKeyButton = new QPushButton("Send Key");
    //    layout->addRow(_sendKeyButton);

    setLayout(layout);
}

QWidget *BootloaderWidget::informationDeviceWidget()
{
    QGroupBox *groupBox = new QGroupBox(tr("Node Information"));
    QHBoxLayout *hlayout = new QHBoxLayout();

    QFormLayout *fileLayout = new QFormLayout();

    IndexLabel *indexLabel;
    indexLabel = new IndexLabel(NodeObjectId(0x1018, 1));
    indexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    indexLabel->setCursor(Qt::IBeamCursor);
    indexLabel->setDisplayHint(IndexLabel::DisplayHexa);
    fileLayout->addRow(tr("&Vendor Id:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x1018, 2));
    indexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    indexLabel->setCursor(Qt::IBeamCursor);
    indexLabel->setDisplayHint(IndexLabel::DisplayHexa);
    fileLayout->addRow(tr("&Product Code:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x1018, 3));
    indexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    indexLabel->setCursor(Qt::IBeamCursor);
    indexLabel->setDisplayHint(IndexLabel::DisplayHexa);
    fileLayout->addRow(tr("&Revision number"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x100A, 0));
    indexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    indexLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("&Software version:"), indexLabel);
    _indexWidgets.append(indexLabel);

    indexLabel = new IndexLabel(NodeObjectId(0x2003, 0));
    indexLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    indexLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("Software &build:"), indexLabel);
    _indexWidgets.append(indexLabel);

    hlayout->addItem(fileLayout);

    groupBox->setLayout(hlayout);
    return groupBox;
}

QWidget *BootloaderWidget::informationFileWidget()
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

    _vendorIdUfwLabel = new QLabel();
    _vendorIdUfwLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _vendorIdUfwLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("&Vendor Id:"), _vendorIdUfwLabel);

    _productCodeUfwLabel = new QLabel();
    _productCodeUfwLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _productCodeUfwLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("&Product Code:"), _productCodeUfwLabel);

    _revisionNumberUfwLabel = new QLabel();
    _revisionNumberUfwLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _revisionNumberUfwLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("&Revision number"), _revisionNumberUfwLabel);

    _versionSoftwareUfwLabel = new QLabel();
    _versionSoftwareUfwLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _versionSoftwareUfwLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("&Software version:"), _versionSoftwareUfwLabel);

    _buildDateUfwLabel = new QLabel();
    _buildDateUfwLabel->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    _buildDateUfwLabel->setCursor(Qt::IBeamCursor);
    fileLayout->addRow(tr("Software &build:"), _buildDateUfwLabel);

    groupBox->setLayout(fileLayout);
    return groupBox;
}
