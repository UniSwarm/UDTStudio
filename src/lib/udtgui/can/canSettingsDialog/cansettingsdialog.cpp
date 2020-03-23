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

#include "cansettingsdialog.h"

#include <QLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QCanBus>

CanSettingsDialog::CanSettingsDialog(QCanBusDevice *canDevice, QWidget *parent)
    : QDialog(parent), _canDevice(canDevice)
{
    setWindowTitle(QString("Can settings"));
    createDialog();

    _interfaceComboBox->addItems(QCanBus::instance()->plugins());
    _interfaceComboBox->setCurrentIndex(_interfaceComboBox->findText("socketcan"));
}

CanSettingsDialog::~CanSettingsDialog()
{

}

CanSettingsDialog::Settings CanSettingsDialog::settings() const
{
    return _currentSettings;
}

QCanBusDevice *CanSettingsDialog::device() const
{
    return _canDevice;
}

void CanSettingsDialog::interfaceChanged(const QString &interface)
{
    _deviceComboBox->clear();
    _devices = QCanBus::instance()->availableDevices(interface);

    for (const QCanBusDeviceInfo &info : qAsConst(_devices))
    {
        _deviceComboBox->addItem(info.name());
    }
}

void CanSettingsDialog::deviceChanged(const QString &device)
{
    Q_UNUSED(device)
}

void CanSettingsDialog::okButton(void)
{
    saveSettings();
    QString errorString;

    if (!_canDevice)
    {
         _canDevice = QCanBus::instance()->createDevice(_currentSettings.interfaceName, _currentSettings.deviceName, &errorString);
    }
    else
    {
        _canDevice = QCanBus::instance()->createDevice(_currentSettings.interfaceName, _currentSettings.deviceName, &errorString);
        if (!_canDevice)
        {
            QMessageBox::warning(this, tr("My Application"), tr("Error creating device '%1', reason: '%2'").arg(_currentSettings.interfaceName).arg(errorString),
                                 QMessageBox::Cancel);
        }
    }

    accept();
}

void CanSettingsDialog::cancelButton(void)
{
    restoreSettings();
    reject();
}

void CanSettingsDialog::restoreSettings()
{
    _interfaceComboBox->setCurrentText(_currentSettings.interfaceName);
    _deviceComboBox->setCurrentText(_currentSettings.deviceName);
    _bitrateComboBox->setCurrentText(searchParam(QCanBusDevice::BitRateKey));
}

void CanSettingsDialog::saveSettings()
{
    _currentSettings.interfaceName = _interfaceComboBox->currentText();
    _currentSettings.deviceName = _deviceComboBox->currentText();

    int bitrate = _bitrateComboBox->currentText().toInt();
    const ConfigParam key(QCanBusDevice::BitRateKey, QVariant(bitrate));
    _currentSettings.config.append(key);
}

QString CanSettingsDialog::searchParam(QCanBusDevice::ConfigurationKey key)
{
    QVariant result;

    for (const ConfigParam &keys : qAsConst(_currentSettings.config))
    {
        if (keys.first == key)
        {
            result = keys.second;
            break;
        }
    }

    return result.toString();
}


void CanSettingsDialog::fillBitrates()
{
    const QList<int> rates = {10000, 20000, 50000, 100000, 125000, 250000, 500000, 800000, 1000000};

    _bitrateComboBox->clear();
    for (int rate : rates)
    {
        _bitrateComboBox->addItem(QString::number(rate), rate);
    }
    _bitrateComboBox->setCurrentIndex(8);
}

void CanSettingsDialog::createDialog()
{
    QLayout *layout = new QVBoxLayout();

    QLayout *interfaceLayout = new QHBoxLayout();
    QLabel *interfaceLabel = new QLabel("Interface :");
    _interfaceComboBox = new QComboBox();
    interfaceLayout->addWidget(interfaceLabel);
    interfaceLayout->addWidget(_interfaceComboBox);

    QLayout *deviceLayout = new QHBoxLayout();
    QLabel *deviceLabel = new QLabel("Device :");
    _deviceComboBox = new QComboBox();
    deviceLayout->addWidget(deviceLabel);
    deviceLayout->addWidget(_deviceComboBox);

    QLayout *bitrateLayout = new QHBoxLayout();
    QLabel *bitrateLabel = new QLabel("Bitrate :");
    _bitrateComboBox = new QComboBox();
    bitrateLayout->addWidget(bitrateLabel);
    bitrateLayout->addWidget(_bitrateComboBox);

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

    connect(_interfaceComboBox, &QComboBox::currentTextChanged, this, &CanSettingsDialog::interfaceChanged);
    connect(_deviceComboBox, &QComboBox::currentTextChanged, this, &CanSettingsDialog::deviceChanged);
    connect(okPushButton, &QPushButton::clicked, this, &CanSettingsDialog::okButton);
    connect(cancelPushButton, &QPushButton::clicked, this, &CanSettingsDialog::cancelButton);
}
