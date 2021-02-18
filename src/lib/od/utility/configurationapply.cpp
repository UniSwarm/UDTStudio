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

#include "configurationapply.h"

#include <QFileInfo>
#include <QSettings>

#include <QDebug>
#define dbg() qDebug().noquote().nospace()

ConfigurationApply::ConfigurationApply()
{
}

bool ConfigurationApply::apply(DeviceConfiguration *deviceDescription, const QString &fileIniPath)
{
    if (!QFileInfo::exists(fileIniPath))
    {
        dbg() << "File Configuration not exist: " << fileIniPath;
        return false;
    }

    QSettings settings(fileIniPath, QSettings::IniFormat);
    settings.beginGroup("Default");

    const QStringList childKeys = settings.childKeys();
    for (const QString &childKey : childKeys)
    {
        bool ok;
        uint16_t indexId = 0;
        uint8_t subIndexId = 0;

        if (childKey.contains(QRegExp("^[0-9]")))
        {
            // Find by id
            if (childKey.contains("sub"))
            {
                QStringList keys = childKey.split("sub");
                indexId = static_cast<uint16_t>(keys[0].toUInt(&ok, 16));
                subIndexId = static_cast<uint8_t>(keys[1].toUInt(&ok, 16));
            }
            else
            {
                indexId = static_cast<uint16_t>(childKey.toUInt(&ok, 16));
                subIndexId = 0;
            }

            if (deviceDescription->indexExist(indexId))
            {
                QString value = settings.value(childKey).toString();
                Index *index = deviceDescription->index(indexId);
                if (index->subIndexExist(subIndexId))
                {
                    SubIndex *sub = index->subIndex(subIndexId);
                    sub->setValue(readData(sub->dataType(), value));
                }
                else
                {
                    dbg() << "Configuration | SubIndex not exist: " << indexId << subIndexId;
                    return false;
                }
            }
            else
            {
                dbg() << "Configuration | Index not exist: " << indexId;
                return false;
            }
        }
        else
        {
            // Find by String
            QString indexName;
            QString subIndexName;

            if (childKey.contains("."))
            {
                QStringList keys = childKey.split(".");
                indexName = keys[0];
                subIndexName = keys[1];

                if (deviceDescription->indexExist(indexName))
                {
                    QString value = settings.value(childKey).toString();
                    Index *index = deviceDescription->index(indexName);
                    if (index->subIndexExist(subIndexName))
                    {
                        SubIndex *sub = index->subIndex(subIndexName);
                        sub->setValue(readData(sub->dataType(), value));
                    }
                    else
                    {
                        dbg() << "Configuration | SubIndex not exist: " << indexName << subIndexName;
                        return false;
                    }
                }                
                else
                {
                    dbg() << "Configuration | Index not exist: " << indexName;
                    return false;
                }
            }
            else
            {
                indexName = childKey;
                if (deviceDescription->indexExist(indexName))
                {
                    QString value = settings.value(childKey).toString();
                    Index *index = deviceDescription->index(indexName);
                    SubIndex *sub = index->subIndex(0);
                    sub->setValue(readData(sub->dataType(), value));
                }

                else
                {
                    dbg() << "Configuration | Index not exist: " << indexName;
                    return false;
                }
            }
        }
    }
    settings.endGroup();
    return true;
}

/**
 * @brief read data to correct format from dcf or eds file
 * @param dcf or eds file
 * @return data
 */
QVariant ConfigurationApply::readData(SubIndex::DataType dataType, QString stringValue) const
{
    int base = 10;
    if (stringValue.startsWith("0x"))
    {
        base = 16;
    }

    if (stringValue.isEmpty())
    {
        return QVariant();
    }

    bool ok;
    switch (dataType)
    {
    case SubIndex::BOOLEAN:
    case SubIndex::INTEGER8:
    case SubIndex::INTEGER16:
    case SubIndex::INTEGER32:
        return QVariant(stringValue.toInt(&ok, base));

    case SubIndex::INTEGER64:
        return QVariant(stringValue.toLongLong(&ok, base));

    case SubIndex::UNSIGNED8:
    case SubIndex::UNSIGNED16:
    case SubIndex::UNSIGNED32:
        return QVariant(stringValue.toUInt(&ok, base));

    case SubIndex::UNSIGNED64:
        return QVariant(stringValue.toULongLong(&ok, base));

    case SubIndex::REAL32:
        return QVariant(stringValue.toFloat());

    case SubIndex::REAL64:
        return QVariant(stringValue.toDouble());

    case SubIndex::VISIBLE_STRING:
    case SubIndex::OCTET_STRING:
        return QVariant(stringValue);

    default:
        return QVariant();
    }
}
