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

#include "hexfile.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>

HexFile::HexFile(const QString &fileName)
{
    _fileName = fileName;
}

bool HexFile::read()
{
    int lineCount = 1;
    int offsetAddr = 0;
    int dataCount, addr, type, checkSum;
    bool ok;

    QFile file(_fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "cannot open";
        return false;
    }

    QTextStream stream(&file);
    _prog.clear();
    _prog.fill(static_cast<char>(0xFF), 0x10000);
    while (!stream.atEnd())
    {
        int index = 0;
        QString line = stream.readLine();

        if (line.size() < 11)
        {
            return false;
        }
        if (line.at(index) != ':')
        {
            return false;
        }
        index++;

        dataCount = line.midRef(index, 2).toInt(&ok, 16);
        if (!ok)
        {
            return false;
        }
        index += 2;

        addr = line.midRef(index, 4).toInt(&ok, 16);
        if (!ok)
        {
            return false;
        }
        index += 4;

        type = line.midRef(index, 2).toInt(&ok, 16);
        if (!ok)
        {
            return false;
        }
        index += 2;

        if (type == 0)
        {
            if (offsetAddr + addr > _prog.size())
            {
                _prog.append(QByteArray(_prog.size() - (offsetAddr + addr + dataCount), static_cast<char>(0xFF)));
                // qDebug() << "outData" << QString::number(offsetAddr + addr, 16) << line.midRef(index, 2 * dataCount);
            }
            // else
            {
                for (int i = 0; i < dataCount; i++)
                {
                    int data = line.midRef(index, 2).toInt(&ok, 16);
                    if (!ok)
                    {
                        return false;
                    }
                    index += 2;
                    _prog[addr + i + offsetAddr] = static_cast<char>(data);
                }
            }
        }
        else if (type == 4)
        {
            offsetAddr = line.midRef(index, 4).toInt(&ok, 16) * 0x10000;
            if (!ok)
            {
                return false;
            }
            index += 4;
            // qDebug() << "offset" << QString::number(offsetAddr, 16);
        }
        else if (type == 1)
        {
            break;
        }
        else
        {
            // qDebug() << "unknow type" << type << "at line" << lineCount;
        }

        checkSum = line.midRef(index, 2).toInt(&ok, 16);
        if (!ok)
        {
            return false;
        }
        index += 2;

        lineCount++;
    }

    _checksum = 0;
    for (int i = 0; i < _prog.size(); i++)
    {
        _checksum += static_cast<unsigned char>(_prog[i]);
    }

    return true;
}

const QByteArray &HexFile::prog() const
{
    return _prog;
}

const unsigned short &HexFile::checksum() const
{
    return _checksum;
}
