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

#ifndef CSVGENERATOR_H
#define CSVGENERATOR_H

#include "generator/generator.h"
#include "od_global.h"

#include <QString>
#include <QTextStream>

#include "model/deviceconfiguration.h"
#include "model/devicedescription.h"

class OD_EXPORT CsvGenerator : public Generator
{

public:
    CsvGenerator();
    ~CsvGenerator();

    // Generator interface
    bool generate(DeviceConfiguration *deviceConfiguration, const QString &filePath);
    bool generate(DeviceDescription *deviceDescription, const QString &filePath);

private:
    static QString accessToString(int access);
    static QString pdoToString(uint8_t accessType);

    void writeListIndex(const QList<Index *> indexes, QTextStream *out);
    void writeIndex(Index *index, QTextStream *out);
    void writeRecord(Index *index, QTextStream *out);
    void writeArray(Index *index, QTextStream *out);
    void writeLimit(const SubIndex *subIndex, QTextStream *out);
};

#endif  // CSVGENERATOR_H
