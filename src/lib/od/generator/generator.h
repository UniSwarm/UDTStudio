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

#ifndef GENERATOR_H
#define GENERATOR_H

#include "od_global.h"

#include <QString>

#include "model/deviceconfiguration.h"
#include "model/devicedescription.h"

class OD_EXPORT Generator
{
public:
    Generator();
    virtual ~Generator();

    virtual bool generate(DeviceConfiguration *deviceConfiguration, const QString &filePath) = 0;
    virtual bool generate(DeviceDescription *deviceDescription, const QString &filePath) = 0;

    static Generator *getGenerator(const QString &type);

    const QString &errorStr() const;

protected:
    QString _errorStr;
    void appendError(const QString &error);
};

#endif  // GENERATOR_H
