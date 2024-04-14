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

#include "indexdb401.h"

#include "../canopen/nodeobjectid.h"

NodeObjectId IndexDb401::getObjectId(IndexDb401::OdObject object, uint channel, uint opt2)
{
    Q_UNUSED(opt2)

    switch (object)
    {
        case OD_MS_BOARD_VOLTAGE_INPUT:
            return {0x2000, 0x1};
        case OD_MS_MANUFACTURE_DATE:
            return {0x2001, 0x0};
        case OD_MS_CALIBRATION_DATE:
            return {0x2002, 0x0};
        case OD_MS_FIRMWARE_BUILD_DATE:
            return {0x2003, 0x0};
        case OD_MS_BOARD_LED:
            return {0x2004, 0x0};
        case OD_MS_CPU1_TEMPERATURE:
            return {0x2020, 0x1};
        case OD_MS_CPU1_LIFE_CYCLE:
            return {0x2021, 0x1};
        case OD_MS_CPU1_ERROR:
            return {0x2022, 0x1};
        case OD_MS_CPU1_MIN_CYCLE_US:
            return {0x2023, 0x1};
        case OD_MS_CPU1_MAX_CYCLE_US:
            return {0x2024, 0x1};
        case OD_MS_CPU1_MEAN_CYCLE_US:
            return {0x2025, 0x1};

        case OD_MS_DI_SCHMITT_TRIGGERS_HIGH:
            return {0x4000, static_cast<quint8>(channel)};
        case OD_MS_DI_SCHMITT_TRIGGERS_LOW:
            return {0x4001, static_cast<quint8>(channel)};

        case OD_MS_DO_MODE:
            return {0x4200, static_cast<quint8>(channel)};

        case OD_MS_DO_PWM_FREQUENCY:
            return {0x4201, static_cast<quint8>(channel)};

        case IndexDb401::DI_VALUES_8BITS_CHANNELS_0_7:
            return {0x6000, 0x1};
        case IndexDb401::DI_POLARITIES_8BITS_CHANNELS_0_7:
            return {0x6002, 0x1};
        case IndexDb401::DI_FILTER_CONSTANTS_8BITS_CHANNELS_0_7:
            return {0x6003, 0x1};
        case IndexDb401::DI_GLOBAL_INTERRUPT_ENABLE_8BITS:
            return {0x6005, 0x1};
        case IndexDb401::DI_INTERRUPT_MASK_ANY_CHANGE_8BITS_CHANNELS_0_7:
            return {0x6006, 0x1};
        case IndexDb401::DI_INTERRUPT_MASK_LOW_TO_HIGH_CHANGE_8BITS_CHANNELS_0_7:
            return {0x6007, 0x1};
        case IndexDb401::DI_INTERRUPT_MASK_HIGH_TO_LOW_CHANGE_8BIT_CHANNELS_0_7:
            return {0x6008, 0x1};

        case IndexDb401::DO_VALUES_8BITS_CHANNELS_0_7:
            return {0x6200, 0x1};
        case IndexDb401::DO_POLARITIES_8BITS_CHANNELS_0_7:
            return {0x6202, 0x1};
        case IndexDb401::DO_ERROR_MODES_8BITS_CHANNELS_0_7:
            return {0x6206, 0x1};
        case IndexDb401::DO_ERROR_VALUES_8BITS_CHANNELS_0_7:
            return {0x6207, 0x1};
        case IndexDb401::DO_FILTER_MASKS_8BITS_CHANNELS_0_7:
            return {0x6208, 0x1};

        case IndexDb401::AI_VALUES_8BITS:
            return {0x6400, static_cast<quint8>(channel)};
        case IndexDb401::AI_VALUES_16BITS:
            return {0x6401, static_cast<quint8>(channel)};
        case IndexDb401::AI_VALUES_32BITS:
            return {0x6402, static_cast<quint8>(channel)};
        case IndexDb401::AI_VALUES_REAL:
            return {0x6403, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_TRIGGERS:
            return {0x6421, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_SOURCE:
            return {0x6422, static_cast<quint8>(channel)};
        case IndexDb401::AI_GLOBAL_INTERRUPT_ENABLE:
            return {0x6423, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_UPPER_LIMIT_32BITS:
            return {0x6424, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_LOWER_IMIT_32BITS:
            return {0x6425, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_DELTA_32BITS:
            return {0x6426, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_NEGATIVE_DELTA_32BITS:
            return {0x6427, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_POSITIVE_DELTA_32BITS:
            return {0x6428, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_UPPER_LIMIT_REAL:
            return {0x6429, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_LOWER_LIMIT_REAL:
            return {0x642A, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_DELTA_REAL:
            return {0x642B, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_NEGATIVE_DELTA_REAL:
            return {0x642C, static_cast<quint8>(channel)};
        case IndexDb401::AI_INTERRUPT_POSITIVE_DELTA_REAL:
            return {0x642D, static_cast<quint8>(channel)};
        case IndexDb401::AI_OFFSET_REAL:
            return {0x642E, static_cast<quint8>(channel)};
        case IndexDb401::AI_PRESCALING_REAL:
            return {0x642F, static_cast<quint8>(channel)};
        case IndexDb401::AI_SI_UNIT:
            return {0x6430, static_cast<quint8>(channel)};
        case IndexDb401::AI_OFFSET_32BITS:
            return {0x6431, static_cast<quint8>(channel)};
        case IndexDb401::AI_PRESCALING_32BITS:
            return {0x6421, static_cast<quint8>(channel)};
        case IndexDb401::AO_VALUES_8BITS:
            return {0x6410, static_cast<quint8>(channel)};
        case IndexDb401::AO_VALUES_16BITS:
            return {0x6411, static_cast<quint8>(channel)};
        case IndexDb401::AO_OUTPUT_32BITS:
            return {0x6412, static_cast<quint8>(channel)};
        case IndexDb401::AO_OUTPUT_REAL:
            return {0x6413, static_cast<quint8>(channel)};
    }
    return NodeObjectId();
}
