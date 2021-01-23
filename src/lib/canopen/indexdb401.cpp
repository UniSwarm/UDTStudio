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

#include "indexdb401.h"

#include "../canopen/nodeobjectid.h"

NodeObjectId IndexDb401::getObjectId(IndexDb401::OdObject object, uint opt, uint opt2)
{
    Q_UNUSED(opt)
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
    case OD_MS_DI_ST_HT_00:
        return {0x4000, 0x1};
    case OD_MS_DI_ST_HT_01:
        return {0x4000, 0x2};
    case OD_MS_DI_ST_HT_02:
        return {0x4000, 0x3};
    case OD_MS_DI_ST_HT_03:
        return {0x4000, 0x4};
    case OD_MS_DI_ST_HT_04:
        return {0x4000, 0x5};
    case OD_MS_DI_ST_HT_05:
        return {0x4000, 0x6};
    case OD_MS_DI_ST_HT_06:
        return {0x4000, 0x7};
    case OD_MS_DI_ST_HT_07:
        return {0x4000, 0x8};
    case OD_MS_DI_ST_LT_00:
        return {0x4001, 0x1};
    case OD_MS_DI_ST_LT_01:
        return {0x4001, 0x2};
    case OD_MS_DI_ST_LT_02:
        return {0x4001, 0x3};
    case OD_MS_DI_ST_LT_03:
        return {0x4001, 0x4};
    case OD_MS_DI_ST_LT_04:
        return {0x4001, 0x5};
    case OD_MS_DI_ST_LT_05:
        return {0x4001, 0x6};
    case OD_MS_DI_ST_LT_06:
        return {0x4001, 0x7};
    case OD_MS_DI_ST_LT_07:
        return {0x4001, 0x8};
    case OD_MS_DO_MODE_00:
        return {0x4200, 0x1};
    case OD_MS_DO_MODE_01:
        return {0x4200, 0x2};
    case OD_MS_DO_MODE_02:
        return {0x4200, 0x3};
    case OD_MS_DO_MODE_03:
        return {0x4200, 0x4};
    case OD_MS_DO_MODE_04:
        return {0x4200, 0x5};
    case OD_MS_DO_MODE_05:
        return {0x4200, 0x6};
    case OD_MS_DO_MODE_06:
        return {0x4200, 0x7};
    case OD_MS_DO_MODE_07:
        return {0x4200, 0x8};
    }
    return NodeObjectId();
}
