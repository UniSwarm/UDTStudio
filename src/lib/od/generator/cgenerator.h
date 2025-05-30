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

#ifndef CGENERATOR_H
#define CGENERATOR_H

#include "od_global.h"

#include "generator/generator.h"

#include <QSet>
#include <QString>
#include <QTextStream>

#include "model/deviceconfiguration.h"
#include "model/devicedescription.h"

class OD_EXPORT CGenerator : public Generator
{
public:
    CGenerator();
    ~CGenerator() override;

    bool generate(DeviceConfiguration *deviceConfiguration, const QString &filePath) override;
    bool generate(DeviceDescription *deviceDescription, const QString &filePath) override;
    bool generate(DeviceDescription *deviceDescription, const QString &filePath, uint8_t nodeId);

    bool generateH(DeviceConfiguration *deviceConfiguration, const QString &filePath);
    bool generateC(DeviceConfiguration *deviceConfiguration, const QString &filePath);

    bool generateHStruct(DeviceConfiguration *deviceConfiguration, const QString &filePath, uint16_t min, uint16_t max, const QString &structName);

public:
    static QString typeToString(SubIndex::DataType type);
    static QString varNameToString(const QString &name);
    static QString structNameToString(const QString &name);
    static QString dataToString(const SubIndex *index);
    static QString objectTypeToEnumString(uint16_t objectType);
    static QString dataTypeToEnumString(uint16_t dataType);
    static QString accessToEnumString(uint8_t acces);
    static QString stringNameToString(const SubIndex *subIndex);

private:
    template <typename T>
    static QString toUHex(T value);

    void writeRecordDefinitionH(Index *index, QTextStream &hFile);
    void writeArrayDefinitionH(Index *index, QTextStream &hFile);
    void writeIndexH(Index *index, QTextStream &hFile);
    void writeDefineH(Index *index, QTextStream &hFile);

    int writeRamLineC(Index *index, QTextStream &cFile);
    void writeSubentriesList(Index *index, QTextStream &cFile);
    void writeSubentry(const SubIndex *index, QTextStream &cFile);
    void writeOdCompletionC(Index *index, QTextStream &cFile);
    void writeCharLineC(const SubIndex *subIndex, QTextStream &cFile);

    void writeInitRamC(const QList<Index *> &indexes, QTextStream &cFile);

    void writeSetNodeId(DeviceConfiguration *deviceConfiguration, QTextStream &cFile);

    QSet<QString> _typeSetTable;
};

#endif  // CGENERATOR_H
