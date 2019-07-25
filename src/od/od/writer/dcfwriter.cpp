/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#include "dcfwriter.h"

#include <QFile>
#include <QLatin1String>

#include "deviceiniwriter.h"

DcfWriter::DcfWriter()
{
}

DcfWriter::~DcfWriter()
{
}

bool indexLessThan(const Index *i1, const Index *i2)
{
    return i1->index() < i2->index();
}

void DcfWriter::write(const DeviceConfiguration *deviceConfiguration, const QString &dir) const
{

    QFile dcfFile(dir + "/" + deviceConfiguration->getFileName());

    if (!dcfFile.open(QIODevice::WriteOnly))
        return;

    QTextStream out(&dcfFile);
    DeviceIniWriter writer(&out);

    writer.writeFileInfo(deviceConfiguration->fileInfos());
    writer.writeDeviceComissioning(deviceConfiguration->deviceComissionings());
    writer.writeDummyUsage(deviceConfiguration->dummyUsages());

    QList<Index *> mandatories;
    QList<Index *> optionals;
    QList<Index *> manufacturers;

    foreach (Index *index, deviceConfiguration->indexes().values())
    {
        uint16_t numIndex = index->index();

        if (numIndex == 0x1000 || numIndex == 0x1001 || numIndex == 0x1018)
            mandatories.append(index);

        else if (numIndex >= 0x2000 && numIndex < 0x6000)
            manufacturers.append(index);

        else
            optionals.append(index);
    }

    qSort(mandatories.begin(), mandatories.end(), indexLessThan);
    qSort(optionals.begin(), optionals.end(), indexLessThan);
    qSort(manufacturers.begin(), manufacturers.end(), indexLessThan);

    out << "[MandatoryObjects]" << "\n";
    writer.writeSupportedIndexes(mandatories);
    out << "\n";
    writer.writeListIndex(mandatories);

    out << "[OptionalObjects]" << "\n";
    writer.writeSupportedIndexes(optionals);
    out << "\n";
    writer.writeListIndex(optionals);

    out << "[ManufacturerObjects]" << "\n";
    writer.writeSupportedIndexes(manufacturers);
    out << "\n";
    writer.writeListIndex(manufacturers);

    dcfFile.close();
}
