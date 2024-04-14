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

#ifndef INDEXDB401_H
#define INDEXDB401_H

#include "canopen_global.h"

class NodeObjectId;

class CANOPEN_EXPORT IndexDb401
{
public:
    enum OdObject
    {
        OD_MS_BOARD_VOLTAGE_INPUT,
        OD_MS_MANUFACTURE_DATE,
        OD_MS_CALIBRATION_DATE,
        OD_MS_FIRMWARE_BUILD_DATE,
        OD_MS_BOARD_LED,
        OD_MS_CPU1_TEMPERATURE,
        OD_MS_CPU1_LIFE_CYCLE,
        OD_MS_CPU1_ERROR,
        OD_MS_CPU1_MIN_CYCLE_US,
        OD_MS_CPU1_MAX_CYCLE_US,
        OD_MS_CPU1_MEAN_CYCLE_US,

        OD_MS_DI_SCHMITT_TRIGGERS_HIGH,
        OD_MS_DI_SCHMITT_TRIGGERS_LOW,
        OD_MS_DO_MODE,
        OD_MS_DO_PWM_FREQUENCY,

        // Object APP
        DI_VALUES_8BITS_CHANNELS_0_7,
        DI_POLARITIES_8BITS_CHANNELS_0_7,
        DI_FILTER_CONSTANTS_8BITS_CHANNELS_0_7,
        DI_GLOBAL_INTERRUPT_ENABLE_8BITS,
        DI_INTERRUPT_MASK_ANY_CHANGE_8BITS_CHANNELS_0_7,
        DI_INTERRUPT_MASK_LOW_TO_HIGH_CHANGE_8BITS_CHANNELS_0_7,
        DI_INTERRUPT_MASK_HIGH_TO_LOW_CHANGE_8BIT_CHANNELS_0_7,

        DO_VALUES_8BITS_CHANNELS_0_7,
        DO_POLARITIES_8BITS_CHANNELS_0_7,
        DO_ERROR_MODES_8BITS_CHANNELS_0_7,
        DO_ERROR_VALUES_8BITS_CHANNELS_0_7,
        DO_FILTER_MASKS_8BITS_CHANNELS_0_7,

        AI_VALUES_8BITS,
        AI_VALUES_16BITS,
        AI_VALUES_32BITS,
        AI_VALUES_REAL,
        AI_INTERRUPT_TRIGGERS,
        AI_INTERRUPT_SOURCE,
        AI_GLOBAL_INTERRUPT_ENABLE,
        AI_INTERRUPT_UPPER_LIMIT_32BITS,
        AI_INTERRUPT_LOWER_IMIT_32BITS,
        AI_INTERRUPT_DELTA_32BITS,
        AI_INTERRUPT_NEGATIVE_DELTA_32BITS,
        AI_INTERRUPT_POSITIVE_DELTA_32BITS,
        AI_INTERRUPT_UPPER_LIMIT_REAL,
        AI_INTERRUPT_LOWER_LIMIT_REAL,
        AI_INTERRUPT_DELTA_REAL,
        AI_INTERRUPT_NEGATIVE_DELTA_REAL,
        AI_INTERRUPT_POSITIVE_DELTA_REAL,
        AI_OFFSET_REAL,
        AI_PRESCALING_REAL,
        AI_SI_UNIT,
        AI_OFFSET_32BITS,
        AI_PRESCALING_32BITS,

        AO_VALUES_8BITS,
        AO_VALUES_16BITS,
        AO_OUTPUT_32BITS,
        AO_OUTPUT_REAL,
    };

    static NodeObjectId getObjectId(OdObject object, uint channel = 0, uint opt2 = 0);
};

#endif  // INDEXDB401_H
