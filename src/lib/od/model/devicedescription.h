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

#ifndef DEVICEDESCRIPTION_H
#define DEVICEDESCRIPTION_H

#include "od_global.h"

#include <QMap>

#include "devicemodel.h"

class OD_EXPORT DeviceDescription : public DeviceModel
{
public:
    DeviceDescription();
    virtual ~DeviceDescription();

    Type type();

    const QMap<QString, QString> &deviceInfos() const;
    void setDeviceInfo(const QString &key, const QString &value);

private:
    QMap<QString, QString> _deviceInfos;
};

#endif // DEVICEDESCRIPTION_H
