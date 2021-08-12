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

#include "hexmerger.h"

#include <QDebug>
#include <QFile>

HexMerger::HexMerger()
{
}

int HexMerger::merge(const QByteArray &appA, const QByteArray &appB, QStringList addressesA, QStringList addressesB)
{
    int ret = 0;
    ret = append(appA, addressesA);
    if (ret < 0)
    {
        return ret;
    }
    // Save file
    QFile file("appA");
    if (!file.open(QFile::WriteOnly))
    {
        return false;
    }
    else
    {
        file.write(appA);
        file.close();
    }
    ret = append(appB, addressesB);
    if (ret < 0)
    {
        return ret;
    }
    QFile file2("appB");
    if (!file2.open(QFile::WriteOnly))
    {
        return false;
    }
    else
    {
        file2.write(appB);
        file2.close();
    }

    QFile file3("merge");
    if (!file3.open(QFile::WriteOnly))
    {
        return false;
    }
    else
    {
        file3.write(_prog);
        file3.close();
    }

    return 0;
}

const QByteArray &HexMerger::prog() const
{
    return _prog;
}

int HexMerger::append(const QByteArray &app, QStringList addresses)
{
    bool ok;
    int i = 0;

    int error = checkAddresses(addresses);
    if (error < 0)
    {
        qDebug() << "HexMerger:append : Error addresses";
        return error;
    }

    for (i = 0; i < addresses.size(); i++)
    {
        int adrStart = addresses.at(i).split(QLatin1Char(':')).at(0).toInt(&ok, 16);
        int adrEnd = addresses.at(i).split(QLatin1Char(':')).at(1).toInt(&ok, 16);

        if (adrEnd > _prog.size())
        {
            _prog.append(QByteArray(adrEnd - _prog.size(), static_cast<char>(0xFF)));
        }

        _prog.replace(adrStart, adrEnd - adrStart, app.mid(adrStart, adrEnd - adrStart));
    }
    return 0;
}

int HexMerger::checkAddresses(QStringList addresses)
{
    int i = 0;
    for (i = 0; i < addresses.size(); i++)
    {
        if (!addresses.at(i).contains(QLatin1Char(':')))
        {
            return -1;
        }
    }
    return 0;
}
