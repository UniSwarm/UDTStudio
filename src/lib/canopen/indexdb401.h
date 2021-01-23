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
        OD_MS_DI_ST_HT_00,
        OD_MS_DI_ST_HT_01,
        OD_MS_DI_ST_HT_02,
        OD_MS_DI_ST_HT_03,
        OD_MS_DI_ST_HT_04,
        OD_MS_DI_ST_HT_05,
        OD_MS_DI_ST_HT_06,
        OD_MS_DI_ST_HT_07,
        OD_MS_DI_ST_LT_00,
        OD_MS_DI_ST_LT_01,
        OD_MS_DI_ST_LT_02,
        OD_MS_DI_ST_LT_03,
        OD_MS_DI_ST_LT_04,
        OD_MS_DI_ST_LT_05,
        OD_MS_DI_ST_LT_06,
        OD_MS_DI_ST_LT_07,
        OD_MS_DO_MODE_00,
        OD_MS_DO_MODE_01,
        OD_MS_DO_MODE_02,
        OD_MS_DO_MODE_03,
        OD_MS_DO_MODE_04,
        OD_MS_DO_MODE_05,
        OD_MS_DO_MODE_06,
        OD_MS_DO_MODE_07,
    };

    static NodeObjectId getObjectId(OdObject object, uint opt = 0, uint opt2 = 0);
};

#endif // INDEXDB401_H
