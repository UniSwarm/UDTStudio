/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#ifndef CONFIGURATIONAPPLY_H
#define CONFIGURATIONAPPLY_H

#include "od_global.h"

#include "../model/deviceconfiguration.h"
#include "../model/devicedescription.h"

class OD_EXPORT ConfigurationApply
{
public:
    static bool apply(DeviceModel *deviceModel, const QString &fileIniPath);

private:
    static SubIndex *getSubIndex(DeviceModel *deviceDescription, const QString &childKey);
    static void resizeArray(Index *index, int newSize);
    static QString renameItem(const QString &name, int value);

    static QVariant readData(SubIndex::DataType dataType, const QString &stringValue);

    static QString objectFileDetail(const QString &filePath, const QString &childKey);
};

#endif  // CONFIGURATIONAPPLY_H
