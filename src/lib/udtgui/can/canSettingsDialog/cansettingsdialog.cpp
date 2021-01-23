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

#include "cansettingsdialog.h"

#include <QLayout>
#include <QFormLayout>
#include <QPushButton>
#include <QMessageBox>
#include <QCanBus>
#include <QDialogButtonBox>

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

void CanSettingsDialog::accept()
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
    QDialog::accept();
}

void CanSettingsDialog::reject()
{
    restoreSettings();
    QDialog::reject();
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

    QFormLayout *formLayout = new QFormLayout();
    formLayout->setSpacing(8);

    _interfaceComboBox = new QComboBox();
    formLayout->addRow(tr("&Interface :"), _interfaceComboBox);

    _deviceComboBox = new QComboBox();
    formLayout->addRow(tr("&Device :"), _deviceComboBox);

    _bitrateComboBox = new QComboBox();
    formLayout->addRow(tr("&Bitrate :"), _bitrateComboBox);

    QDialogButtonBox *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addItem(formLayout);
    layout->addWidget(buttonBox);
    setLayout(layout);

    fillBitrates();

    connect(_interfaceComboBox, &QComboBox::currentTextChanged, this, &CanSettingsDialog::interfaceChanged);
    connect(_deviceComboBox, &QComboBox::currentTextChanged, this, &CanSettingsDialog::deviceChanged);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}
