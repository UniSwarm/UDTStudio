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

#include <QFile>
#include <QtEndian>

#include "bootloader/model/ufwmodel.h"

UfwParser::UfwParser()
{
    _ufwModel = new UfwModel();
}

UfwParser::~UfwParser()
{
    delete _ufwModel;
}

UfwModel *UfwParser::parse(const QString &fileName) const
{
    QByteArray uni;
    QFile file(fileName);

    if (!file.open(QFile::ReadOnly))
    {
        return nullptr;
    }
    else
    {
        uni = file.read(file.size());
        file.close();
    }

    if (uni.size() < 0x25)
    {
        return nullptr;
    }

    uint8_t index = 0;

    uint16_t device = qFromLittleEndian<uint16_t>(uni);
    _ufwModel->setDeviceType(device);
    index = sizeof(_ufwModel->deviceType());

    // VERSION
    uint8_t size = qFromLittleEndian<uint8_t>(uni.mid(index, sizeof(uint8_t)));
    index += sizeof(uint8_t);

    QString version = uni.mid(index, size);
    _ufwModel->setSoftwareVersion(version);
    index += size;

    // DATE
    size = qFromLittleEndian<uint8_t>(uni.mid(index, sizeof(uint8_t)));
    index += sizeof(uint8_t);

    QString date = uni.mid(index, size);
    _ufwModel->setBuildDate(date);
    index += size;

    uint8_t count = qFromLittleEndian<uint8_t>(uni.mid(index, sizeof(uint8_t)));
    index += sizeof(uint8_t);

    for (int i = 0; i < count; i++)
    {
        uint32_t start = qFromLittleEndian<uint32_t>(uni.mid(index + i * 8, sizeof(uint32_t)));
        uint32_t end = qFromLittleEndian<uint32_t>(uni.mid(index + 4 + i * 8, sizeof(uint32_t)));
        _ufwModel->appendSegment(start, end);
    }

    _ufwModel->setProg(uni.mid(index + count * 8, uni.size()));
    if (_ufwModel->prog().size() == 0)
    {
        return nullptr;
    }

    return _ufwModel;
}
