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

#ifndef DCFWRITER_H
#define DCFWRITER_H

#include "od_global.h"

#include <QTextStream>
#include <QString>
#include <QList>

#include "writer.h"

class OD_EXPORT DcfWriter : public Writer
{
public:
    DcfWriter();

    void write(OD *od, const QString &dir) const;

protected:
    void writeSupportedIndexes(QList<Index *> indexes, QTextStream &file) const;
    void writeListIndex(QList<Index *> indexes, QTextStream &file) const;
    void writeIndex(Index *index, QTextStream &file) const;
    void writeRecord(Index *index, QTextStream &file) const;
    void writeArray(Index *index, QTextStream &file) const;

private:
    QString valueToString(int value, int base = 10) const;
    QString accessToString(int access) const;
    QString dataToString(DataStorage data, int base = 10) const;
};

#endif // DCFWRITER_H
