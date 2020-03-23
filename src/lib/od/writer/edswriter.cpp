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

#include "edswriter.h"

#include "deviceiniwriter.h"

#include <QFile>

/**
 * @brief default constructor
 */
EdsWriter::EdsWriter()
{
}

/**
 * @brief destructor
 */
EdsWriter::~EdsWriter()
{
}

/**
 * @brief writes a device description model to an eds file
 * @param device description model
 * @param file name
 */
void EdsWriter::write(const DeviceDescription *deviceDescription, const QString &filePath) const
{
    QFile dcfFile(filePath);

    if (!dcfFile.open(QIODevice::WriteOnly))
        return;

    QTextStream out(&dcfFile);
    DeviceIniWriter writer(&out);

    writer.writeFileInfo(deviceDescription->fileInfos());
    writer.writeDeviceInfo(deviceDescription->deviceInfos());
    writer.writeDummyUsage(deviceDescription->dummyUsages());

    writer.writeObjects(deviceDescription);

    dcfFile.close();
}
