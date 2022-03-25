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

#ifndef FASTDATALOGGERCONFIG_H
#define FASTDATALOGGERCONFIG_H

#include "canopen_global.h"

#include "nodeobjectid.h"

#include <QVariant>

class CANOPEN_EXPORT FastDataLoggerConfig
{
public:
    FastDataLoggerConfig();

    NodeObjectId &data1_objId();
    const NodeObjectId &data1_objId() const;
    void setData1_objId(const NodeObjectId &data1_objId);

    NodeObjectId &data2_objId();
    const NodeObjectId &data2_objId() const;
    void setData2_objId(const NodeObjectId &data2_objId);

    uint16_t frequencyDivider() const;
    void setFrequencyDivider(uint16_t frequencyDivider);

    NodeObjectId &trigger_objId();
    const NodeObjectId &trigger_objId() const;
    void setTrigger_objId(const NodeObjectId &trigger_objId);

    enum TriggerType
    {
        TriggerTypeSoftware = 0x00,
        TriggerTypeLessThan = 0x01,
        TriggerTypeEqualTo = 0x02,
        TriggerTypeGreaterThan = 0x03,
    };
    TriggerType triggerType() const;
    void setTriggerType(TriggerType triggerType);

    const QVariant &triggerValue() const;
    void setTriggerValue(const QVariant &triggerValue);

private:
    NodeObjectId _data1_objId;
    NodeObjectId _data2_objId;
    uint16_t _frequencyDivider;
    NodeObjectId _trigger_objId;
    TriggerType _triggerType;
    QVariant _triggerValue;
};

#endif  // FASTDATALOGGERCONFIG_H
