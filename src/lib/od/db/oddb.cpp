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

#include "oddb.h"

#include "parser/edsparser.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QDirIterator>

OdDb::OdDb(QString directory)
    : _directory(directory)
{
    _directoryList.append(_directory);

    _fileName = nullptr;
    _path = nullptr;

    searchFile();
}

void OdDb::setDirectory(QString directory)
{
    _directory = directory;
    searchFile();
}

void OdDb::searchFile()
{
    QDir dir(_directory);
    QStringList list = dir.entryList(QStringList() << "*.eds", QDir::Files | QDir::NoSymLinks);

    for (const QString &file : list)
    {
        EdsParser parser;
        DeviceDescription *deviceDescription = parser.parse(_directory + "/" + file);
        QByteArray byte;
        byte.append(deviceDescription->index(0x1000)->subIndex(0)->value().toByteArray());
        byte.append(deviceDescription->index(0x1018)->subIndex(1)->value().toByteArray());
        byte.append(deviceDescription->index(0x1018)->subIndex(2)->value().toByteArray());
        QByteArray hash = QCryptographicHash::hash(byte, QCryptographicHash::Md4);
        QPair<quint32, QString> pair;
        pair.first = deviceDescription->index(0x1018)->subIndex(3)->value().toUInt();
        pair.second = _directory + "/" + file;
        _mapFile.insert(hash, pair);

        delete deviceDescription;
        byte.clear();
    }
}

QString OdDb::file(quint32 deviceType, quint32 vendorID, quint32 productCode, quint32 revisionNumber)
{
    QByteArray byte;
    byte.append(QVariant(deviceType).toByteArray());
    byte.append(QVariant(vendorID).toByteArray());
    byte.append(QVariant(productCode).toByteArray());
    QByteArray hash = QCryptographicHash::hash(byte, QCryptographicHash::Md4);
    _mapFile.contains(hash);

    QList<QPair<quint32, QString>> values = _mapFile.values(hash);

    for (quint32 j = revisionNumber; j > 0; j--)
    {
        for (int i = 0; i < values.size(); i++)
        {
            if (values.at(i).first == j)
            {
                qDebug() << "deviceType :" << deviceType << ", vendorID :" << vendorID << ", productCode :" << productCode << ", revisionNumber :" << j;
                return values.at(i).second;
            }
        }
    }

    return QString();
}

void OdDb::refreshFile()
{
    _mapFile.clear();
    searchFile();
}
