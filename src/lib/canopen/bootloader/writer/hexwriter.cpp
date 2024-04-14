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

#include "hexwriter.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

HexWriter::HexWriter()
{
}

int HexWriter::write(const QByteArray &prog, const QString &filePath, Optimization optimization)
{
    QFile data(filePath);
    if (!data.open(QFile::WriteOnly | QFile::Truncate))
    {
        return -1;
    }

    int offset = 1;
    int dataCount = 0x10;
    QTextStream stream(&data);

    for (int index = 0; index < prog.size(); index += dataCount)
    {
        int mod = prog.size() % dataCount;
        if (mod != 0 && index >= (prog.size() - mod))
        {
            QString line(QStringLiteral(":%1").arg(QString::number(mod, 16).rightJustified(2, '0').toUpper()));
            // address
            line.append(QString::number(index & 0xFFFF, 16).rightJustified(4, '0').toUpper());
            // type
            line.append(QStringLiteral("00"));
            // Data
            line.append(prog.mid(prog.size() - mod, mod).toHex().rightJustified(2 * mod, '0').toUpper());
            // Checksum
            line.append(QString::number(checksum(line), 16).rightJustified(2, '0').toUpper());
            stream << line << "\n";
        }
        else
        {
            // Number octet
            QString line(QStringLiteral(":%1").arg(QString::number(dataCount, 16).rightJustified(2, '0').toUpper().toUpper()));
            // address
            line.append(QString::number(index & 0xFFFF, 16).rightJustified(4, '0').toUpper().toUpper());
            // type
            line.append(QStringLiteral("00"));
            // Data
            line.append(prog.mid(index, dataCount).toHex().rightJustified(16, '0').toUpper());
            // Checksum
            line.append(QString::number(checksum(line), 16).rightJustified(2, '0').toUpper());
            if (optimization == ON)
            {
                if (!line.contains(QStringLiteral("FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF")))
                {
                    stream << line << "\n";
                }
            }
            else
            {
                stream << line << "\n";
            }

            if ((index & 0xFFFF) == 0xFFF0)
            {
                QString extended(QStringLiteral(":02000004"));
                extended.append(QString::number(offset, 16).rightJustified(4, '0').toUpper());
                // Checksum
                extended.append(QString::number(checksum(extended), 16).rightJustified(2, '0').toUpper());
                stream << extended << "\n";
                offset++;
            }
        }
    }

    // End of Hex
    stream << ":00000001FF"
           << "\n";

    return 0;
}

int HexWriter::checksum(const QString &data)
{
    int i = 0;
    int checksum = 0;
    bool ok;

    for (i = 1; i < data.size(); i = i + 2)
    {
        checksum += data.mid(i, 2).toInt(&ok, 16);
    }

    return (~(checksum % 256) + 1) & 0xFF;
}
