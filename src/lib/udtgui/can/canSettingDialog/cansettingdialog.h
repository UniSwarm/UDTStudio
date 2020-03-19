/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu julien.vaquer@uniswarm.eu
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
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>

#include <QCanBusDevice>
#include <QCanBusDeviceInfo>

#include "../../udtgui_global.h"

class UDTGUI_EXPORT canSettingDialog : public QDialog
{

  public:
    canSettingDialog(QWidget *parent = Q_NULLPTR);
    ~canSettingDialog();

    typedef QPair<QCanBusDevice::ConfigurationKey, QVariant> configParam;

    struct Settings
    {
        QString interfaceName;
        QString deviceName;
        QList<configParam> config;
    };

    Settings settings() const;
    QCanBusDevice *device() const;
    QString searchParam(QCanBusDevice::ConfigurationKey key);

  private:
    void init();
    void createDialog();
    void interfaceChanged(const QString &interface);
    void deviceChanged(const QString &device);
    void okButton(void);
    void cancelButton(void);
    void saveSettings();
    void restoreSettings();
    void fillBitrates();

    Settings currentSettings;
    QList<QCanBusDeviceInfo> devices;
    QCanBusDevice *canDevice = nullptr;

    QComboBox *interfaceComboBox;
    QComboBox *deviceComboBox;
    QComboBox *bitrateComboBox;
};

#endif // CANSETTINGDIALOG_H
