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
#include <QRegularExpression>
#include <QSettings>

#include <QDebug>
#define dbg() qDebug().noquote().nospace()

bool ConfigurationApply::apply(DeviceModel *deviceModel, const QString &fileIniPath)
{
    if (!QFileInfo::exists(fileIniPath))
    {
        dbg() << "File configuration does not exist: " << fileIniPath;
        return false;
    }

    QSettings settings(fileIniPath, QSettings::IniFormat);
    QStringList groups = settings.childGroups();
    if (!groups.contains("Default"))
    {
        dbg() << "File Configuration : corrupted file (missing [Default] group) " << fileIniPath;
        return false;
    }

    settings.beginGroup("Default");

    const QStringList &childKeys = settings.childKeys();
    for (const QString &childKey : childKeys)
    {
        SubIndex *subIndex = getSubIndex(deviceModel, childKey);
        if (subIndex != nullptr)
        {
            QString strValue = settings.value(childKey).toString();
            QVariant value = readData(subIndex->dataType(), strValue);

            if (subIndex->index()->objectType() == Index::ARRAY && subIndex->subIndex() == 0)
            {
                if (value.canConvert(QMetaType::UInt) && value.toUInt() <= 128 && subIndex->index()->subIndexesCount() >= 2)
                {
                    resizeArray(subIndex->index(), value.toUInt());
                }
                else
                {
                    dbg() << "Invalid ARRAY for resize: " << childKey;
                    return false;
                }
            }
            else
            {
                subIndex->setValue(value);
            }
        }
        else
        {
            dbg() << "Configuration | Object does not exist: " << childKey;
            return false;
        }
    }

    settings.endGroup();
    return true;
}

SubIndex *ConfigurationApply::getSubIndex(DeviceModel *deviceDescription, const QString &childKey)
{
    bool ok = false;
    if (childKey.contains(QRegularExpression("^[0-9]")))
    {
        uint16_t indexId = 0;
        uint8_t subIndexId = 0;

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
            Index *index = deviceDescription->index(indexId);
            if (index->subIndexExist(subIndexId))
            {
                SubIndex *sub = index->subIndex(subIndexId);
                return sub;
            }
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

            Index *index = deviceDescription->index(indexName);
            if (index != nullptr)
            {
                SubIndex *sub = nullptr;
                if (subIndexName.startsWith("sub"))
                {
                    uint subIndexNumber = subIndexName.mid(3).toUInt();
                    sub = index->subIndex(subIndexNumber);
                }
                else
                {
                    sub = index->subIndex(subIndexName);
                }
                return sub;
            }
        }
        else
        {
            indexName = childKey;
            Index *index = deviceDescription->index(indexName);
            if (index != nullptr)
            {
                SubIndex *sub = index->subIndex(0);
                return sub;
            }
        }
    }
    return nullptr;
}

void ConfigurationApply::resizeArray(Index *index, int newSize)
{
    // remove all sub > 1
    for (int sub = 2; sub <= index->subIndexesCount(); sub++)
    {
        index->removeSubIndex(sub);
    }

    SubIndex *subIndex = index->subIndex(1);
    for (int sub = 2; sub < newSize + 1; sub++)
    {
        SubIndex *newSubIndex = new SubIndex(*subIndex);
        newSubIndex->setSubIndex(sub);
        newSubIndex->setName(renameItem(subIndex->name(), sub));
        index->addSubIndex(newSubIndex);
    }
    subIndex->setName(renameItem(subIndex->name(), 1));

    index->setMaxSubIndex(newSize + 1);
    index->subIndex(0)->setValue(newSize);
}

QString ConfigurationApply::renameItem(const QString &name, int value)
{
    QString newName = name;
    QRegularExpression reg("%([0-9]*)([z]*)([cCd])");
    QRegularExpressionMatch match = reg.match(name);
    QString modifier = match.captured(2);
    QString type = match.captured(3);
    if (type == 'd')
    {
        if (modifier == 'z')
        {
            value--;
        }
        int length = match.captured(1).toInt();
        newName.replace(reg, QString::number(value).rightJustified(length, '0'));
        return newName;
    }
    if (type == "C")
    {
        newName.replace("%C", QChar('A' + value - 1));
        return newName;
    }
    if (type == "c")
    {
        newName.replace("%c", QChar('a' + value - 1));
        return newName;
    }
    return newName;
}

/**
 * @brief read data to correct format from dcf or eds file
 * @param dcf or eds file
 * @return data
 */
QVariant ConfigurationApply::readData(SubIndex::DataType dataType, const QString &stringValue)
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

    bool ok = false;
    switch (dataType)
    {
        case SubIndex::INTEGER8:
        case SubIndex::INTEGER16:
        case SubIndex::INTEGER24:
        case SubIndex::INTEGER32:
            return QVariant(stringValue.toInt(&ok, base));

        case SubIndex::INTEGER40:
        case SubIndex::INTEGER48:
        case SubIndex::INTEGER56:
        case SubIndex::INTEGER64:
            return QVariant(stringValue.toLongLong(&ok, base));

        case SubIndex::UNSIGNED8:
        case SubIndex::UNSIGNED16:
        case SubIndex::UNSIGNED24:
        case SubIndex::UNSIGNED32:
            return QVariant(stringValue.toUInt(&ok, base));

        case SubIndex::UNSIGNED40:
        case SubIndex::UNSIGNED48:
        case SubIndex::UNSIGNED56:
        case SubIndex::UNSIGNED64:
            return QVariant(stringValue.toULongLong(&ok, base));

        case SubIndex::REAL32:
            return QVariant(stringValue.toFloat());

        case SubIndex::REAL64:
            return QVariant(stringValue.toDouble());

        case SubIndex::VISIBLE_STRING:
        case SubIndex::OCTET_STRING:
        case SubIndex::UNICODE_STRING:
            return QVariant(stringValue);

        default:
            return QVariant();
    }
}
