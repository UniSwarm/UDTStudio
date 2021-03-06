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

#include "oddb.h"

#include "parser/edsparser.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QDirIterator>

OdDb::OdDb(QString directory)
{
    addDirectory(directory);
}

void OdDb::addDirectory(const QString &directory)
{
    addDirectory(QStringList(directory));
}

void OdDb::addDirectory(const QStringList &directories)
{
    for (const QString &directory : directories)
    {
        searchFile(directory);
    }
    _directoryList.append(directories);
}

void OdDb::searchFile(const QString &directory)
{
    QDirIterator it(directory, QStringList() << "*.eds", QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        const QString &file = it.next();
        EdsParser parser;

        DeviceDescription *deviceDescription = parser.parse(file);

        QByteArray byte;
        byte.append(deviceDescription->subIndexValue(0x1000, 0, "0").toByteArray());
        byte.append(deviceDescription->subIndexValue(0x1018, 1, "0").toByteArray());
        byte.append(deviceDescription->subIndexValue(0x1018, 2, "0").toByteArray());

        QByteArray hash = QCryptographicHash::hash(byte, QCryptographicHash::Md4);
        QPair<quint32, QString> pair;
        pair.first = deviceDescription->subIndexValue(0x1018, 3).toUInt();
        pair.second = file;
        _mapFile.insert(hash, pair);

        delete deviceDescription;
    }
}

QString OdDb::file(quint32 deviceType, quint32 vendorID, quint32 productCode, quint32 revisionNumber)
{
    QByteArray byte;
    byte.append(QVariant(deviceType).toByteArray());
    byte.append(QVariant(vendorID).toByteArray());
    byte.append(QVariant(productCode).toByteArray());
    QByteArray hash = QCryptographicHash::hash(byte, QCryptographicHash::Md4);

    QList<QPair<quint32, QString>> values = _mapFile.values(hash);

    if (!values.isEmpty())
    {
        quint32 rev = revisionNumber;
        do
        {
            for (int i = 0; i < values.size(); i++)
            {
                if (values.at(i).first == rev)
                {
                    // qDebug() << "deviceType :" << deviceType << ", vendorID :" << vendorID << ", productCode :" << productCode << ", revisionNumber :" << rev;
                    return values.at(i).second;
                }
            }
            if (rev == 0)
            {
                break;
            }
            rev--;
        } while (1);
    }

    return QString();
}

void OdDb::refreshFile()
{
    _mapFile.clear();
    for (const QString &directory : _directoryList)
    {
        searchFile(directory);
    }
}
