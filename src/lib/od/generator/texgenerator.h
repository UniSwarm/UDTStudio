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

#ifndef TEXGENERATOR_H
#define TEXGENERATOR_H

#include "od_global.h"

#include "generator/generator.h"

#include <QString>
#include <QTextStream>

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

private:
    static QString accessToString(int access);
    static QString pdoToString(uint8_t accessType);

    void writeListIndex(const QList<Index *> indexes, QTextStream *out);
    void writeListIndexComm(const QList<Index *> indexes, QTextStream *out);
    void writeListIndexManufacturer402(const QList<Index *> indexes, QTextStream *out);
    void writeIndex(Index *index, QTextStream *out, bool generic);
    void writeRecord(Index *index, QTextStream *out, bool generic);
    void writeArray(Index *index, QTextStream *out, bool generic);
    void writeLimit(const SubIndex *subIndex, QTextStream *out);
};

#endif  // TEXGENERATOR_H
