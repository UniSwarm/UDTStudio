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

#include "profileduplicate.h"

void ProfileDuplicate::duplicate(DeviceModel *deviceModel, uint8_t profileCount)
{
    if (!deviceModel->indexExist(0x1000))
    {
        return;
    }
    uint32_t deviceType = deviceModel->index(0x1000)->subIndex(0)->value().toUInt();

    if ((deviceType & 0x0000FFFF) == 402)
    {
        // Manufacturer-specific profile area : Axis
        QList<Index *> manufactureIndex;
        // Standardized profile area : 0x6000 to 0x9FFF
        QList<Index *> standardizedIndex;

        for (auto it = deviceModel->indexes().begin(); it != deviceModel->indexes().end(); )
        {
            uint16_t numIndex = (*it)->index();
            if (numIndex >= 0x4000 && numIndex < 0x4200)
            {
                manufactureIndex.append(*it);
                it = deviceModel->indexes().erase(it);
            }
            else if (numIndex >= 0x4200 && numIndex <= 0x4FFF)
            {
                it = deviceModel->indexes().erase(it);
            }
            else if (numIndex >= 0x6000 && numIndex < 0x6800)
            {
                standardizedIndex.append(*it);
                it = deviceModel->indexes().erase(it);
            }
            else if (numIndex >= 0x6800 && numIndex <= 0x9FFF)
            {
                it = deviceModel->indexes().erase(it);
            }
            else
            {
                it++;
            }
        }

        QString name;
        QRegExp reg("^[a][0-9]_");
        for (uint16_t count = 0; count < profileCount; count++)
        {
            for (const Index *index : manufactureIndex)
            {
                Index *newIndex = new Index(*index);
                uint16_t indexId = index->index() + (0x200 * count);
                name = index->name();
                name.remove(reg);
                newIndex->setName(QString("a%1_").arg(count + 1) + name);
                newIndex->setIndex(indexId);
                deviceModel->addIndex(newIndex);
            }
        }

        for (uint16_t count = 0; count < profileCount; count++)
        {
            for (const Index *index : standardizedIndex)
            {
                Index *newIndex = new Index(*index);
                uint16_t indexId = index->index() + (0x800 * count);
                name = index->name();
                name.remove(reg);
                newIndex->setName(QString("a%1_").arg(count + 1) + name);
                newIndex->setIndex(indexId);
                deviceModel->addIndex(newIndex);
            }
        }
    }

    if ((deviceType & 0x0000FFFF) == 428)
    {
        // Standardized profile area : 0x6000 to 0x9FFF
        QList<Index *> standardizedIndex;

        for (auto it = deviceModel->indexes().begin(); it != deviceModel->indexes().end(); )
        {
            uint16_t numIndex = (*it)->index();
            if (numIndex >= 0x6000 && numIndex < 0x6100)
            {
                standardizedIndex.append(*it);
                it = deviceModel->indexes().erase(it);
            }
            else if (numIndex >= 0x6100 && numIndex <= 0x9FFF)
            {
                it = deviceModel->indexes().erase(it);
            }
            else
            {
                it++;
            }
        }

        QString name;
        QRegExp reg("^[s][0-9]+_");
        for (uint16_t count = 0; count < profileCount; count++)
        {
            for (const Index *index : standardizedIndex)
            {
                Index *newIndex = new Index(*index);
                uint16_t indexId = newIndex->index() + (0x100 * count);
                name = index->name();
                name.remove(reg);
                newIndex->setName(QString("s%1_").arg(QString::number(count + 1).rightJustified(2, '0')) + name);
                newIndex->setIndex(indexId);
                deviceModel->addIndex(newIndex);
            }
        }
    }
}
