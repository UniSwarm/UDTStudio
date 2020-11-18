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

#ifndef CGENERATOR_H
#define CGENERATOR_H

#include "od_global.h"

#include "generator/generator.h"

#include <QString>
#include <QTextStream>

#include "model/deviceconfiguration.h"
#include "model/devicedescription.h"

class OD_EXPORT CGenerator : public Generator
{
public:
    CGenerator();
    ~CGenerator();

    bool generate(DeviceConfiguration *deviceConfiguration, const QString &filePath);
    bool generate(DeviceDescription *deviceDescription, const QString &filePath);
    void generate(DeviceDescription *deviceDescription, const QString &filePath, uint8_t nodeId);

    void generateH(DeviceConfiguration *deviceConfiguration, const QString &filePath);
    void generateC(DeviceConfiguration *deviceConfiguration, const QString &filePath);

private:
    static QString typeToString(const uint16_t &type);
    static QString varNameToString(const QString &name);
    static QString structNameToString(const QString &name);
    static QString dataToString(const SubIndex *index);
    static QString objectTypeToEnumString(const uint16_t objectType);
    static QString dataTypeToEnumString(const uint16_t dataType);
    static QString accessToEnumString(const uint8_t acces);
    static QString typeObjectToString(Index *index, uint8_t subIndex, bool isInRecord);
    static QString stringNameToString(const SubIndex *subIndex);

    void writeRecordDefinitionH(Index *index, QTextStream &hFile);
    void writeIndexH(Index *index, QTextStream &hFile);
    void writeRamLineC(Index *index, QTextStream &cFile);
    void writeRecordCompletionC(Index *index, QTextStream &cFile);
    void writeOdCompletionC(Index *index, QTextStream &cFile);
    void writeCharLineC(const SubIndex *subIndex, QTextStream &cFile);
    void writeInitRamC(QList<Index *> indexes, QTextStream &cFile);
    void writeDefineH(Index *index, QTextStream &hFile);
    void writeSetNodeId(DeviceConfiguration *deviceConfiguration, QTextStream &cFile);
};

#endif // CGENERATOR_H
