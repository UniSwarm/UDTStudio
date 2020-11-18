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

#ifndef TEXGENERATOR_H
#define TEXGENERATOR_H

#include "od_global.h"

#include "generator/generator.h"

#include "model/deviceconfiguration.h"
#include "model/devicedescription.h"

class OD_EXPORT TexGenerator : public Generator
{
public:
    TexGenerator();
    ~TexGenerator();

    // Generator interface
    bool generate(DeviceConfiguration *deviceConfiguration, const QString &filePath);
    bool generate(DeviceDescription *deviceDescription, const QString &filePath);
};

#endif // TEXGENERATOR_H
