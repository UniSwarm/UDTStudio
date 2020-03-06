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

#include "dcfparser.h"

#include <QSettings>
#include <QRegularExpression>

#include "deviceiniparser.h"

/**
 * @brief default constructor
 */
DcfParser::DcfParser()
{
}

/**
 * @brief destructor
 */
DcfParser::~DcfParser()
{
}

/**
 * @brief parse a .dcf file
 * @param dcf file name
 * @return device congiguration model completed by parser
 */
DeviceConfiguration *DcfParser::parse(const QString &path) const
{
    DeviceConfiguration *deviceConfiguration = new DeviceConfiguration;

    QSettings file(path, QSettings::IniFormat);
    DeviceIniParser parser(&file);

    // infos
    foreach (const QString &group, file.childGroups())
    {
        if (group == "DeviceComissioning")
        {
            file.beginGroup(group);
            parser.readDeviceComissioning(deviceConfiguration);
            file.endGroup();
            continue;
        }

        else if (group == "FileInfo")
        {
            file.beginGroup(group);
            parser.readFileInfo(deviceConfiguration);
            file.endGroup();
            continue;
        }

        else if (group == "DummyUsage")
        {
            file.beginGroup(group);
            parser.readDummyUsage(deviceConfiguration);
            file.endGroup();
            continue;
        }
    }

    // objects
    parser.readObjects(deviceConfiguration);

    return deviceConfiguration;
}
