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

#ifndef CANSETTINGDIALOG_H
#define CANSETTINGDIALOG_H

#include <QDialog>

#include <QComboBox>

#include <QCanBusDevice>
#include <QCanBusDeviceInfo>

#include "../../udtgui_global.h"

class UDTGUI_EXPORT CanSettingsDialog : public QDialog
{

public:
    CanSettingsDialog(QCanBusDevice *canDevice = nullptr, QWidget *parent = nullptr);
    ~CanSettingsDialog() override;

    typedef QPair<QCanBusDevice::ConfigurationKey, QVariant> ConfigParam;

    struct Settings
    {
        QString interfaceName;
        QString deviceName;
        QList<ConfigParam> config;
    };

    Settings settings() const;
    QCanBusDevice *device() const;
    QString searchParam(QCanBusDevice::ConfigurationKey key);

private:
    void init();
    void createDialog();
    void interfaceChanged(const QString &interface);
    void deviceChanged(const QString &device);
    void saveSettings();
    void restoreSettings();
    void fillBitrates();

    Settings _currentSettings;
    QList<QCanBusDeviceInfo> _devices;
    QCanBusDevice *_canDevice = nullptr;

    QComboBox *_interfaceComboBox;
    QComboBox *_deviceComboBox;
    QComboBox *_bitrateComboBox;

    // QDialog interface
public slots:
    void accept() override;
    void reject() override;
};

#endif // CANSETTINGDIALOG_H
