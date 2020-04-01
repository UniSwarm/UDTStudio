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

#include "edsparser.h"

#include <QRegularExpression>
#include <QSettings>

#include "deviceiniparser.h"

/**
 * @brief default constructor
 */
EdsParser::EdsParser()
{
}

/**
 * @brief destructor
 */
EdsParser::~EdsParser()
{
}

/**
 * @brief parse a .eds file
 * @param eds file name
 * @return device descritpion model completed by parser
 */
DeviceDescription *EdsParser::parse(const QString &path) const
{
    DeviceDescription *deviceDescription = new DeviceDescription;

    QSettings file(path, QSettings::IniFormat);
    DeviceIniParser parser(&file);

    // infos
    for (const QString &group : file.childGroups())
    {
        if (group == "DeviceInfo")
        {
            file.beginGroup(group);
            parser.readDeviceInfo(deviceDescription);
            file.endGroup();
            continue;
        }
        else if (group == "FileInfo")
        {
            file.beginGroup(group);
            parser.readFileInfo(deviceDescription);
            file.endGroup();
            continue;
        }
        else if (group == "DummyUsage")
        {
            file.beginGroup(group);
            parser.readDummyUsage(deviceDescription);
            file.endGroup();
            continue;
        }
    }

    // objects
    parser.readObjects(deviceDescription);

    return deviceDescription;
}
