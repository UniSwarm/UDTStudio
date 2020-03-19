/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#include "cansettingdialog.h"

#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QCanBus>

canSettingDialog::canSettingDialog(QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(QString("Can settings"));
    createDialog();

    interfaceComboBox->addItems(QCanBus::instance()->plugins());
    interfaceComboBox->setCurrentIndex(interfaceComboBox->findText("socketcan"));
}

canSettingDialog::~canSettingDialog()
{

}

canSettingDialog::Settings canSettingDialog::settings() const
{
    return currentSettings;
}

QCanBusDevice *canSettingDialog::device() const
{
    return canDevice;
}

void canSettingDialog::interfaceChanged(const QString &interface)
{
    deviceComboBox->clear();
    devices = QCanBus::instance()->availableDevices(interface);

    for (const QCanBusDeviceInfo &info : qAsConst(devices))
    {
        deviceComboBox->addItem(info.name());
    }
}

void canSettingDialog::deviceChanged(const QString &device)
{
    Q_UNUSED(device)
}

void canSettingDialog::okButton(void)
{
    saveSettings();
    QString errorString;

    canDevice = QCanBus::instance()->createDevice(currentSettings.interfaceName, currentSettings.deviceName, &errorString);
    if (!canDevice)
    {
        QMessageBox::warning(this, tr("My Application"), tr("Error creating device '%1', reason: '%2'").arg(currentSettings.interfaceName).arg(errorString),
                             QMessageBox::Cancel);
        return;
    }

    accept();
}

void canSettingDialog::cancelButton(void)
{
    restoreSettings();
    reject();
}

void canSettingDialog::restoreSettings()
{
    interfaceComboBox->setCurrentText(currentSettings.interfaceName);
    deviceComboBox->setCurrentText(currentSettings.deviceName);
    bitrateComboBox->setCurrentText(searchParam(QCanBusDevice::BitRateKey));
}

void canSettingDialog::saveSettings()
{
    currentSettings.interfaceName = interfaceComboBox->currentText();
    currentSettings.deviceName = deviceComboBox->currentText();

    int bitrate = bitrateComboBox->currentText().toInt();
    const configParam key(QCanBusDevice::BitRateKey, QVariant(bitrate));
    currentSettings.config.append(key);
}

QString canSettingDialog::searchParam(QCanBusDevice::ConfigurationKey key)
{
    QVariant result;

    for (const configParam &keys : qAsConst(currentSettings.config))
    {
        if (keys.first == key)
        {
            result = keys.second;
            break;
        }
    }

    return result.toString();
}


void canSettingDialog::fillBitrates()
{
    const QList<int> rates = {10000, 20000, 50000, 100000, 125000, 250000, 500000, 800000, 1000000};

    bitrateComboBox->clear();
    for (int rate : rates)
    {
        bitrateComboBox->addItem(QString::number(rate), rate);
    }
    bitrateComboBox->setCurrentIndex(8);
}

void canSettingDialog::createDialog()
{
    QLayout *layout = new QVBoxLayout();

    QLayout *interfaceLayout = new QHBoxLayout();
    QLabel *interfaceLabel = new QLabel("Interface :");
    interfaceComboBox = new QComboBox();
    interfaceLayout->addWidget(interfaceLabel);
    interfaceLayout->addWidget(interfaceComboBox);

    QLayout *deviceLayout = new QHBoxLayout();
    QLabel *deviceLabel = new QLabel("Device :");
    deviceComboBox = new QComboBox();
    deviceLayout->addWidget(deviceLabel);
    deviceLayout->addWidget(deviceComboBox);

    QLayout *bitrateLayout = new QHBoxLayout();
    QLabel *bitrateLabel = new QLabel("Bitrate :");
    bitrateComboBox = new QComboBox();
    bitrateLayout->addWidget(bitrateLabel);
    bitrateLayout->addWidget(bitrateComboBox);

    QLayout *pushButtonLayout = new QHBoxLayout();
    QPushButton *cancelPushButton = new QPushButton("Cancel");
    QPushButton *okPushButton = new QPushButton("OK");
    pushButtonLayout->addWidget(cancelPushButton);
    pushButtonLayout->addWidget(okPushButton);

    layout->addItem(interfaceLayout);
    layout->addItem(deviceLayout);
    layout->addItem(bitrateLayout);
    layout->addItem(pushButtonLayout);
    setLayout(layout);

    fillBitrates();

    connect(interfaceComboBox, &QComboBox::currentTextChanged, this, &canSettingDialog::interfaceChanged);
    connect(deviceComboBox, &QComboBox::currentTextChanged, this, &canSettingDialog::deviceChanged);
    connect(okPushButton, &QPushButton::clicked, this, &canSettingDialog::okButton);
    connect(cancelPushButton, &QPushButton::clicked, this, &canSettingDialog::cancelButton);
}
