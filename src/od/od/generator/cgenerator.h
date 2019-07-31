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

    bool generate(DeviceConfiguration *deviceDescription, const QString &filePath) const;
    bool generate(DeviceDescription *deviceDescription, const QString &filePath) const;
    void generate(DeviceDescription *deviceDescription, const QString &filePath, uint8_t nodeId) const;

    void generateH(DeviceConfiguration *deviceDescription, const QString &filePath) const;
    void generateC(DeviceConfiguration *deviceDescription, const QString &filePath) const;

private:
    QString typeToString(const uint16_t &type) const;
    QString varNameToString(const QString &name) const;
    QString structNameToString(const QString &name) const;
    QString dataToString(const SubIndex *index) const;
    QString typeObjectToString(Index *index, uint8_t subIndex, bool isInRecord) const;
    QString stringNameToString(const SubIndex *subIndex) const;

    void writeRecordDefinitionH(Index *index, QTextStream &hFile) const;
    void writeIndexH(Index *index, QTextStream &hFile) const;
    void writeRamLineC(Index *index, QTextStream &cFile) const;
    void writeRecordCompletionC(Index *index, QTextStream &cFile) const;
    void writeOdCompletionC(Index *index, QTextStream &cFile) const;
    void writeCharLineC(const SubIndex *subIndex, QTextStream &cFile) const;
    void writeInitRamC(QList<Index *> indexes, QTextStream &cFile) const;
    void writeDefineH(Index *index, QTextStream &hFile) const;
    void writeSetNodeId(DeviceConfiguration *deviceConfiguration , QTextStream &cFile) const;
};

#endif // CGENERATOR_H
