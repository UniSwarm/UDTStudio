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

#include <QCollator>
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDirIterator>
#include <QProcessEnvironment>

OdDb *OdDb::_instance = nullptr;

OdDb::OdDb()
{
}

void OdDb::init()
{
    addDirectory(QProcessEnvironment::systemEnvironment().value("EDS_PATH").split(QDir::listSeparator()));

    QString edsPath = QCoreApplication::applicationDirPath() + "/../eds";
    if (QDir(edsPath).exists())
    {
        addDirectory(edsPath);
    }
    edsPath = QCoreApplication::applicationDirPath() + "/../data/eds";
    if (QDir(edsPath).exists())
    {
        addDirectory(edsPath);
    }
}

void OdDb::addDirectory(const QString &directory)
{
    addDirectory(QStringList(directory));
}

void OdDb::addDirectory(const QStringList &directories)
{
    for (const QString &directory : directories)
    {
        instance()->searchFile(directory);
    }
    instance()->_directoryList.append(directories);
}

void OdDb::searchFile(const QString &directory)
{
    QDirIterator it(directory, QStringList() << "*.eds", QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);

    while (it.hasNext())
    {
        const QString &file = it.next();
        EdsParser parser;

        DeviceDescription *deviceDescription = parser.parse(file);

        QByteArray bytesId;
        bytesId.append(deviceDescription->subIndexValue(0x1000, 0, "0").toByteArray());
        bytesId.append(deviceDescription->subIndexValue(0x1018, 1, "0").toByteArray());
        bytesId.append(deviceDescription->subIndexValue(0x1018, 2, "0").toByteArray());
        uint revision = deviceDescription->subIndexValue(0x1018, 3).toUInt();
        QByteArray hash = QCryptographicHash::hash(bytesId, QCryptographicHash::Md4);

        QList<QPair<quint32, QString>> values = instance()->_mapFiles.values(hash);
        bool exists = false;
        if (!values.isEmpty())
        {
            for (const auto &value : values)
            {
                if (value.first == revision)
                {
                    // eds already exists with this revision number
                    exists = true;
                }
            }
        }

        if (!exists)
        {
            // append eds file
            QPair<quint32, QString> pair;
            pair.first = revision;
            pair.second = file;
            _mapFiles.insert(hash, pair);
            _edsFiles.append(file);
        }

        delete deviceDescription;
    }

    QCollator order;
    std::sort(_edsFiles.begin(), _edsFiles.end(), order);
}

QString OdDb::file(quint32 deviceType, quint32 vendorID, quint32 productCode, quint32 revisionNumber)
{
    QByteArray bytesId;
    bytesId.append(QVariant(deviceType).toByteArray());
    bytesId.append(QVariant(vendorID).toByteArray());
    bytesId.append(QVariant(productCode).toByteArray());
    QByteArray hash = QCryptographicHash::hash(bytesId, QCryptographicHash::Md4);

    QList<QPair<quint32, QString>> values = instance()->_mapFiles.values(hash);

    if (!values.isEmpty())
    {
        quint32 rev = revisionNumber;
        do
        {
            for (const auto &value : values)
            {
                if (value.first == rev)
                {
                    return value.second;
                }
            }
            if (rev == 0)
            {
                break;
            }
            rev--;
        } while (true);
    }

    return QString();
}

void OdDb::refreshFile()
{
    instance()->_mapFiles.clear();
    for (const QString &directory : qAsConst(instance()->_directoryList))
    {
        instance()->searchFile(directory);
    }
}

const QList<QString> &OdDb::edsFiles()
{
    return instance()->_edsFiles;
}
