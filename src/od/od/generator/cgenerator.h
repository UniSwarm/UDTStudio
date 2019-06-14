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

#include "model/od.h"

class OD_EXPORT CGenerator : public Generator
{
public:
    CGenerator();

    void generate(OD *od, const QString &dir) const;

private:
    void generateH(OD *od, const QString &dir) const;
    void generateC(OD *od, const QString &dir) const;

    QString typeToString(const uint16_t &type) const;
    QString varNameToString(const QString &name) const;
    QString structNameToString(const QString &name) const;
    QString dataToString(const SubIndex *index, uint8_t subNumber) const;
    QString typeObjectToString(const SubIndex *subIndex) const;
    QString stringNameToString(const SubIndex *subIndex, uint8_t arrayKey) const;

    void writeRecordDefinitionH(Index *index, QTextStream &hFile) const;
    void writeIndexH(Index *index, QTextStream &hFile) const;
    void writeRamLineC(Index *index, QTextStream &cFile) const;
    void writeRecordCompletionC(Index *index, QTextStream &cFile) const;
    void writeOdCompletionC(Index *index, QTextStream &cFile) const;
    void writeCharLineC(SubIndex *subIndex, QTextStream &cFile, uint8_t arrayKey) const;
};

#endif // CGENERATOR_H
