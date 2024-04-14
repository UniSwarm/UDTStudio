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

#include "fastdataloggerconfig.h"

FastDataLoggerConfig::FastDataLoggerConfig()
{
    _frequencyDivider = 1;
    _triggerType = TriggerTypeSoftware;
}

NodeObjectId &FastDataLoggerConfig::data1_objId()
{
    return _data1_objId;
}

const NodeObjectId &FastDataLoggerConfig::data1_objId() const
{
    return _data1_objId;
}

void FastDataLoggerConfig::setData1_objId(const NodeObjectId &data1_objId)
{
    _data1_objId = data1_objId;
}

NodeObjectId &FastDataLoggerConfig::data2_objId()
{
    return _data2_objId;
}

const NodeObjectId &FastDataLoggerConfig::data2_objId() const
{
    return _data2_objId;
}

void FastDataLoggerConfig::setData2_objId(const NodeObjectId &data2_objId)
{
    _data2_objId = data2_objId;
}

uint16_t FastDataLoggerConfig::frequencyDivider() const
{
    return _frequencyDivider;
}

void FastDataLoggerConfig::setFrequencyDivider(uint16_t frequencyDivider)
{
    _frequencyDivider = frequencyDivider;
}

NodeObjectId &FastDataLoggerConfig::trigger_objId()
{
    return _trigger_objId;
}

const NodeObjectId &FastDataLoggerConfig::trigger_objId() const
{
    return _trigger_objId;
}

void FastDataLoggerConfig::setTrigger_objId(const NodeObjectId &trigger_objId)
{
    _trigger_objId = trigger_objId;
}

FastDataLoggerConfig::TriggerType FastDataLoggerConfig::triggerType() const
{
    return _triggerType;
}

void FastDataLoggerConfig::setTriggerType(TriggerType triggerType)
{
    _triggerType = triggerType;
}

const QVariant &FastDataLoggerConfig::triggerValue() const
{
    return _triggerValue;
}

void FastDataLoggerConfig::setTriggerValue(const QVariant &triggerValue)
{
    _triggerValue = triggerValue;
}
