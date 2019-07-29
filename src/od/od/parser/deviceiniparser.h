/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#ifndef DEVICEINIPARSER_H
#define DEVICEINIPARSER_H

#include "od_global.h"

#include <QTextStream>
#include <QSettings>

#include "model/devicedescription.h"
#include "model/deviceconfiguration.h"

class DeviceIniParser
{
public:
    DeviceIniParser(QSettings *file);

    void readObjects(DeviceModel *deviceModel) const;
    void readIndexes(DeviceModel *deviceModel) const;
    void readSubIndexes(DeviceModel *deviceModel) const;
    void readIndex(Index *index) const;
    void readSubIndex(SubIndex *subIndex) const;
    QVariant readData(bool *nodeId) const;
    void readFileInfo(DeviceModel *od) const;
    void readDummyUsage(DeviceModel *od) const;
    void readDeviceInfo(DeviceDescription *od) const;
    void readDeviceComissioning(DeviceConfiguration *od) const;
    uint8_t readPdoMapping() const;
    QVariant readLowLimit() const;
    QVariant readHighLimit() const;
    uint16_t readDataType() const;

    QSettings *_file;
};

#endif // DEVICEINIPARSER_H
