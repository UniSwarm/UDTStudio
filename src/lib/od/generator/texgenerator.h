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
    ~TexGenerator() override;

    // Generator interface
    bool generate(DeviceConfiguration *deviceConfiguration, const QString &filePath) override;
    bool generate(DeviceDescription *deviceDescription, const QString &filePath) override;

private:
    void writeListIndex(const QList<Index *> &indexes, QTextStream *out);
    void writeListIndexComm(const QList<Index *> &indexes, QTextStream *out);
    void writeListIndexManufacturer402(const QList<Index *> &indexes, QTextStream *out);
    void writeVar(Index *index, QTextStream *out, bool generic);
    void writeRecord(Index *index, QTextStream *out, bool generic);
    void writeArray(Index *index, QTextStream *out, bool generic);
    void writeUnit(const SubIndex *subIndex, QTextStream *out) const;
    void writeLimit(const SubIndex *subIndex, QTextStream *out);
    void writeAccessType(const SubIndex *subIndex, QTextStream *out);
    void writeDefaultValue(const SubIndex *subIndex, QTextStream *out);

    QString formatNameIndex(Index *index, bool generic);
    QString formatNameIndexForTex(Index *index, bool generic);
    QString formatNameSubIndexForTex(QString nameSubIndex);
    QString formatIndex(Index *index, bool generic);
    QString toCamelCase(QString &name);

    QString dataTypeStr(SubIndex *subIndex) const;
    QString accessStr(int access) const;
    QString pdoAccessStr(uint8_t accessType) const;

    uint _profile;
};

#endif  // TEXGENERATOR_H
