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
    : QDialog(parent)
{
    setWindowTitle("UniSwarm UDTStudio - Update Node");
    createWidgets();
    connect(&_progressTimer,
            &QTimer::timeout,
            this,
            [this]()
            {
                _updateProgressBar->setValue(_node->bootloader()->updateProgress());
            });
}

BootloaderWidget::BootloaderWidget(Node *node, QWidget *parent)
    : BootloaderWidget(parent)
{
    setNode(node);
}

Node *BootloaderWidget::node() const
{
    return _node;
}

void BootloaderWidget::setNode(Node *node)
{
    if (node == nullptr)
    {
        return;
    }

    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->setNode(node);
    }

    _node = node;

    connect(_node->bootloader(), &Bootloader::statusEvent, this, &BootloaderWidget::updateStatus);

#ifdef DEBUG_BOOT
    connect(_stopButton, &QPushButton::clicked, _node->bootloader(), &Bootloader::stopProgram);
    connect(_startButton, &QPushButton::clicked, _node->bootloader(), &Bootloader::startProgram);
    connect(_resetButton, &QPushButton::clicked, _node->bootloader(), &Bootloader::resetProgram);
    connect(_clearButton, &QPushButton::clicked, _node->bootloader(), &Bootloader::clearProgram);
    connect(_sendKeyButton, &QPushButton::clicked, this, &BootloaderWidget::sendKeyButton);
#endif

    _infoLabel->setText(tr("Update firmware for Node %1").arg(_node->nodeId()));
    readAll();
}

void BootloaderWidget::readAll()
{
    for (AbstractIndexWidget *indexWidget : qAsConst(_indexWidgets))
    {
        indexWidget->readObject();
    }
}

void BootloaderWidget::sendKeyButton()
{
    _node->bootloader()->sendKey();
}

void BootloaderWidget::updateProgram()
{
    _progressTimer.start(100);
    _node->bootloader()->startUpdate();
}

void BootloaderWidget::updateStatus()
{
    Bootloader::Status status = _node->bootloader()->status();

    if (status >= 0)
    {
        _statusLabel->setStyleSheet("QLabel { color : green;font-weight: bold; }");
        _statusLabel->setText(_node->bootloader()->statusStr(status));
    }
    else
    {
        _statusLabel->setStyleSheet("QLabel { color : red;font-weight: bold; }");
        _statusLabel->setText(_node->bootloader()->statusStr(status) + " (0x" + QString::number(_node->bootloader()->error(), 16) + ")");
    }

    if (status == Bootloader::Status::STATUS_FILE_ANALYZED_OK)
    {
        _deviceTypeUfwLabel->setText("0x" + QString::number(_node->bootloader()->deviceType(), 16));
        _versionSoftwareUfwLabel->setText(_node->bootloader()->versionSoftware());
        _buildDateUfwLabel->setText(_node->bootloader()->buildDate());
    }
    if (status == Bootloader::Status::STATUS_UPDATE_SUCCESSFUL || status == Bootloader::Status::STATUS_ERROR_UPDATE_FAILED)
    {
        readAll();
        _progressTimer.stop();
    }

    if (status > Bootloader::Status::STATUS_CHECK_FILE_AND_DEVICE)
    {
        _updateButton->setEnabled(false);
        _openButton->setEnabled(false);
    }
    else
    {
        _updateButton->setEnabled(true);
        _openButton->setEnabled(true);
    }
}

void BootloaderWidget::openFile()
{
    _fileName = QFileDialog::getOpenFileName(this, tr("Open firmware"), "", tr("Firmware File (*.ufw)"));
    if (_fileName.isEmpty())
    {
        return;
    }

    _fileUfwLabel->setText(_fileName);
    bool ok = _node->bootloader()->openUfw(_fileName);
    _updateButton->setEnabled(ok);
}

void BootloaderWidget::createWidgets()
{
    QVBoxLayout *vLayout = new QVBoxLayout();

    _infoLabel = new QLabel();
    vLayout->addWidget(_infoLabel);

    vLayout->addWidget(informationDeviceWidget());

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setSpacing(0);
    hLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    QLabel *iconLabel = new QLabel();
    iconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    iconLabel->setAlignment(Qt::AlignTop | Qt::AlignCenter);
    iconLabel->setPixmap(QPixmap(":/icons/img/icons8-sort-desc.png"));
    hLayout->addWidget(iconLabel);

    hLayout->addSpacerItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    vLayout->addLayout(hLayout);

    vLayout->addWidget(informationFileWidget());

    QFormLayout *layout = new QFormLayout();
    _statusLabel = new QLabel();
    layout->addRow("Status:", _statusLabel);
    vLayout->addLayout(layout);

    _updateButton = new QPushButton(tr("Start update"));
    _updateButton->setEnabled(false);
    vLayout->addWidget(_updateButton);
    connect(_updateButton, &QPushButton::clicked, this, &BootloaderWidget::updateProgram);

    _updateProgressBar = new QProgressBar();
    vLayout->addWidget(_updateProgressBar);

#ifdef DEBUG_BOOT
    _stopButton = new QPushButton(tr("Stop program"));
    vLayout->addWidget(_stopButton);

    _startButton = new QPushButton(tr("Start program"));
    vLayout->addWidget(_startButton);

    _resetButton = new QPushButton(tr("Reset program"));
    vLayout->addWidget(_resetButton);

    _clearButton = new QPushButton(tr("Clear program"));
    vLayout->addWidget(_clearButton);

    _sendKeyButton = new QPushButton(tr("Send Key"));
    vLayout->addWidget(_sendKeyButton);
#endif

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
    QGroupBox *groupBox = new QGroupBox(tr("File informations"));

    QFormLayout *fileLayout = new QFormLayout();

    _openButton = new QPushButton(tr("Open firmware"));
    connect(_openButton, &QPushButton::clicked, this, &BootloaderWidget::openFile);

    fileLayout->addRow(_openButton);

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
