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

#include "ufwparser.h"

#include <QByteArray>
#include <QFile>
#include <QtEndian>

#include "bootloader/model/ufwmodel.h"

UfwModel *UfwParser::parse(const QString &fileName)
{
    QByteArray ufwData;
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
    {
        return nullptr;
    }

    ufwData = file.read(file.size());
    if (ufwData.size() < 0x25)
    {
        return nullptr;
    }

    uint8_t index = 0;
    UfwModel *ufwModel = new UfwModel();

    uint16_t device = qFromLittleEndian<uint16_t>(ufwData);
    ufwModel->setDeviceType(device);
    index = sizeof(ufwModel->deviceType());

    // VERSION
    uint8_t size = qFromLittleEndian<uint8_t>(ufwData.mid(index, sizeof(uint8_t)));
    index += sizeof(uint8_t);

    QString version = ufwData.mid(index, size);
    ufwModel->setSoftwareVersion(version);
    index += size;

    // DATE
    size = qFromLittleEndian<uint8_t>(ufwData.mid(index, sizeof(uint8_t)));
    index += sizeof(uint8_t);

    QString date = ufwData.mid(index, size);
    ufwModel->setBuildDate(date);
    index += size;

    uint8_t count = qFromLittleEndian<uint8_t>(ufwData.mid(index, sizeof(uint8_t)));
    index += sizeof(uint8_t);

    for (int i = 0; i < count; i++)
    {
        uint32_t start = qFromLittleEndian<uint32_t>(ufwData.mid(index + i * 8, sizeof(uint32_t)));
        uint32_t end = qFromLittleEndian<uint32_t>(ufwData.mid(index + 4 + i * 8, sizeof(uint32_t)));
        ufwModel->appendSegment(start, end);
    }

    ufwModel->setProg(ufwData.mid(index + count * 8, ufwData.size()));
    if (ufwModel->prog().size() == 0)
    {
        delete ufwModel;
        return nullptr;
    }

    return ufwModel;
}
