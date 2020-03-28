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

#include "dcfwriter.h"

#include <QFile>
#include <QFileInfo>

#include "deviceiniwriter.h"

/**
 * @brief default constructor
 */
DcfWriter::DcfWriter()
{
}

/**
 * @brief destructor
 */
DcfWriter::~DcfWriter()
{
}

/**
 * @brief writes a device configuration model to a dcf file
 * @param device configuration model
 * @param file name
 */
void DcfWriter::write(DeviceConfiguration *deviceConfiguration, const QString &filePath) const
{
    QFile dcfFile(filePath);

    if (!dcfFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    QTextStream out(&dcfFile);
    DeviceIniWriter writer(&out);

    QString name = QFileInfo(filePath).fileName();
    deviceConfiguration->setFileName(name);

    writer.writeFileInfo(deviceConfiguration->fileInfos());
    writer.writeDeviceComissioning(deviceConfiguration->deviceComissionings());
    writer.writeDummyUsage(deviceConfiguration->dummyUsages());

    writer.writeObjects(deviceConfiguration);

    dcfFile.close();
}

void DcfWriter::write(DeviceDescription *deviceDescription, const QString &filePath, uint8_t nodeId) const
{
    DeviceConfiguration *deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, nodeId);
    write(deviceConfiguration, filePath);
}
