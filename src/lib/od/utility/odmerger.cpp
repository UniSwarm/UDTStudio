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

#include "odmerger.h"

#include <QDebug>
#define dbg() qDebug().noquote().nospace()

void ODMerger::merge(DeviceDescription *deviceDescription, DeviceDescription *secondDeviceDescription)
{
    QMap<QString, QString> deviceInfos(deviceDescription->deviceInfos());
    for (auto i = secondDeviceDescription->deviceInfos().constBegin(); i != secondDeviceDescription->deviceInfos().constEnd(); i++)
    {
        deviceInfos.insert(i.key(), i.value());
    }
    deviceDescription->setDeviceInfos(deviceInfos);

    merge(static_cast<DeviceModel *>(deviceDescription), static_cast<DeviceModel *>(secondDeviceDescription));
}

void ODMerger::merge(DeviceModel *deviceModel, DeviceModel *secondDeviceModel)
{
    QMap<QString, QString> fileInfos(deviceModel->fileInfos());
    for (auto i = secondDeviceModel->fileInfos().constBegin(); i != secondDeviceModel->fileInfos().constEnd(); i++)
    {
        fileInfos.insert(i.key(), i.value());
    }
    deviceModel->setFileInfos(fileInfos);

    QMap<QString, QString> dummyUsages(deviceModel->dummyUsages());
    for (auto i = secondDeviceModel->dummyUsages().constBegin(); i != secondDeviceModel->dummyUsages().constEnd(); i++)
    {
        dummyUsages.insert(i.key(), i.value());
    }
    deviceModel->setDummyUsages(dummyUsages);

    QMap<QString, QString> comments(deviceModel->comments());
    for (auto i = secondDeviceModel->comments().constBegin(); i != secondDeviceModel->comments().constEnd(); i++)
    {
        comments.insert(i.key(), i.value());
    }
    deviceModel->setComments(comments);

    for (Index *index2 : qAsConst(secondDeviceModel->indexes()))
    {
        Index *index = deviceModel->index(index2->index());
        if (index == nullptr)
        {
            // dbg() << "Missing index 0x" << indexStr(index2) << " " << index2->name();
            deviceModel->addIndex(new Index(*index2));
        }
        else
        {
            if (index->name() != index2->name())
            {
                // dbg() << "Change index name 0x" << indexStr(index) << " to \"" << index->name() << "\"";
                index->setName(index2->name());
            }
            if (index->objectType() != index2->objectType())
            {
                // dbg() << "Change index objectType 0x" << indexStr(index) << " to " << index->objectType();
                index->setObjectType(index2->objectType());
            }
            mergeIndex(index, secondDeviceModel->index(index2->index()));
        }
    }

    for (Index *index : qAsConst(deviceModel->indexes()))
    {
        if (!secondDeviceModel->indexExist(index->index()))
        {
            // dbg() << "Extra index 0x" << indexStr(index) << " \"" << index->name() << "\"";
        }
    }
}

void ODMerger::mergeIndex(Index *index, Index *index2)
{
    for (SubIndex *subIndex2 : index2->subIndexes())
    {
        SubIndex *subIndex = index->subIndex(subIndex2->subIndex());
        if (subIndex == nullptr)
        {
            // dbg() << "+ Missing subindex 0x" << indexStr(index) << "." << subIndex2->subIndex() << " : \"" << index->name() << "\"";
            index->addSubIndex(new SubIndex(*index2->subIndex(subIndex2->subIndex())));
        }
        else
        {
            if (subIndex->name() != subIndex2->name())
            {
                // dbg() << "+ Change subindex name 0x" << indexStr(index) << "." << subIndex2->subIndex() << " to \"" << index->name() << "\"";
                subIndex->setName(subIndex2->name());
            }
            if (subIndex->accessType() != subIndex2->accessType())
            {
                // dbg() << "+ Change subindex accessType 0x" << indexStr(index) << "." << subIndex2->subIndex() << " to " << subIndex2->accessType();
                subIndex->setAccessType(subIndex2->accessType());
            }
            if (subIndex->dataType() != subIndex2->dataType())
            {
                // dbg() << "+ Change subindex dataType 0x" << indexStr(index) << "." << subIndex2->subIndex() << " to " << subIndex2->dataType();
                subIndex->setDataType(subIndex2->dataType());
            }
            if (subIndex->lowLimit() != subIndex2->lowLimit())
            {
                // dbg() << "+ Change subindex lowLimit 0x" << indexStr(index) << "." << subIndex2->subIndex() << " to " << subIndex2->lowLimit();
                subIndex->setLowLimit(subIndex2->lowLimit());
            }
            if (subIndex->highLimit() != subIndex2->highLimit())
            {
                // dbg() << "+ Change subindex highLimit 0x" << indexStr(index) << "." << subIndex2->subIndex() << " to " << subIndex2->highLimit();
                subIndex->setLowLimit(subIndex2->highLimit());
            }
        }
    }
}

QString ODMerger::indexStr(Index *index)
{
    return QString::number(index->index(), 16).toUpper();
}
