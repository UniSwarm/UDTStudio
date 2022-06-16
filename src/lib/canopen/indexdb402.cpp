/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation + axisDecal)), either version 3 of the License + axisDecal)), or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not + axisDecal)), see <http://www.gnu.org/licenses/>.
 **/

#include "indexdb402.h"

#include "../canopen/nodeobjectid.h"

NodeObjectId IndexDb402::getObjectId(IndexDb402::OdObject object, uint axis, uint opt2)
{
    quint16 axisDecal = 0;

    if (object <= OD_MS_DUTY_CYCLE_MODE_SLOPE)
    {
        return getObjectIdMs(object, axis, opt2);
    }

    axisDecal += 0x0800 * static_cast<quint16>(axis);

    switch (object)
    {
        case OD_ABORT_CONNECTION_OPTION:
            return {static_cast<quint16>((0x6007 + axisDecal)), 0x0};
        case OD_CONTROLWORD:
            return {static_cast<quint16>((0x6040 + axisDecal)), 0x0};
        case OD_STATUSWORD:
            return {static_cast<quint16>((0x6041 + axisDecal)), 0x0};
        case OD_VL_VELOCITY_TARGET:
            return {static_cast<quint16>((0x6042 + axisDecal)), 0x0};
        case OD_VL_VELOCITY_DEMAND:
            return {static_cast<quint16>((0x6043 + axisDecal)), 0x0};
        case OD_VL_VELOCITY_ACTUAL_VALUE:
            return {static_cast<quint16>((0x6044 + axisDecal)), 0x0};
        case OD_VL_MIN:
            return {static_cast<quint16>((0x6046 + axisDecal)), 0x1};
        case OD_VL_MAX:
            return {static_cast<quint16>((0x6046 + axisDecal)), 0x2};
        case OD_VL_ACCELERATION_DELTA_SPEED:
            return {static_cast<quint16>((0x6048 + axisDecal)), 0x1};
        case OD_VL_ACCELERATION_DELTA_TIME:
            return {static_cast<quint16>((0x6048 + axisDecal)), 0x2};
        case OD_VL_DECELERATION_DELTA_SPEED:
            return {static_cast<quint16>((0x6049 + axisDecal)), 0x1};
        case OD_VL_DECELERATION_DELTA_TIME:
            return {static_cast<quint16>((0x6049 + axisDecal)), 0x2};
        case OD_VL_QUICK_STOP_DELTA_SPEED:
            return {static_cast<quint16>((0x604A + axisDecal)), 0x1};
        case OD_VL_QUICK_STOP_DELTA_TIME:
            return {static_cast<quint16>((0x604A + axisDecal)), 0x2};
        case OD_VL_SET_POINT_FACTOR_NUMERATOR:
            return {static_cast<quint16>((0x604B + axisDecal)), 0x1};
        case OD_VL_SET_POINT_FACTOR_DENOMINATOR:
            return {static_cast<quint16>((0x604B + axisDecal)), 0x2};
        case OD_VL_DIMENSION_FACTOR_NUMERATOR:
            return {static_cast<quint16>((0x604C + axisDecal)), 0x1};
        case OD_VL_DIMENSION_FACTOR_DENOMINATOR:
            return {static_cast<quint16>((0x604C + axisDecal)), 0x2};
        case OD_QUICK_STOP_OPTION:
            return {static_cast<quint16>((0x605A + axisDecal)), 0x0};
        case OD_SHUTDOWN_OPTION:
            return {static_cast<quint16>((0x605B + axisDecal)), 0x0};
        case OD_DISABLE_OPERATION_OPTION:
            return {static_cast<quint16>((0x605C + axisDecal)), 0x0};
        case OD_HALT_OPTION:
            return {static_cast<quint16>((0x605D + axisDecal)), 0x0};
        case OD_FAULT_REACTION_OPTION:
            return {static_cast<quint16>((0x605E + axisDecal)), 0x0};
        case OD_MODES_OF_OPERATION:
            return {static_cast<quint16>((0x6060 + axisDecal)), 0x0};
        case OD_MODES_OF_OPERATION_DISPLAY:
            return {static_cast<quint16>((0x6061 + axisDecal)), 0x0};
        case OD_PC_POSITION_DEMAND_VALUE:
            return {static_cast<quint16>((0x6062 + axisDecal)), 0x0};
        case OD_PC_POSITION_ACTUAL_INTERNAL_VALUE:
            return {static_cast<quint16>((0x6063 + axisDecal)), 0x0};
        case OD_PC_POSITION_ACTUAL_VALUE:
            return {static_cast<quint16>((0x6064 + axisDecal)), 0x0};
        case OD_PC_FOLLOWING_ERROR_WINDOW:
            return {static_cast<quint16>((0x6065 + axisDecal)), 0x0};
        case OD_PC_FOLLOWING_ERROR_TIME_OUT:
            return {static_cast<quint16>((0x6066 + axisDecal)), 0x0};
        case OD_PC_POSITION_WINDOW:
            return {static_cast<quint16>((0x6067 + axisDecal)), 0x0};
        case OD_PC_POSITION_WINDOW_TIME:
            return {static_cast<quint16>((0x6068 + axisDecal)), 0x0};
        case OD_PV_VELOCITY_SENSOR_ACTUAL_VALUE:
            return {static_cast<quint16>((0x6069 + axisDecal)), 0x0};
        case OD_PV_SENSOR_SELECTION_CODE:
            return {static_cast<quint16>((0x606A + axisDecal)), 0x0};
        case OD_PV_VELOCITY_DEMAND_VALUE:
            return {static_cast<quint16>((0x606B + axisDecal)), 0x0};
        case OD_PV_VELOCITY_ACTUAL_VALUE:
            return {static_cast<quint16>((0x606C + axisDecal)), 0x0};
        case OD_PV_VELOCITY_WINDOW:
            return {static_cast<quint16>((0x606D + axisDecal)), 0x0};
        case OD_PV_VELOCITY_WINDOW_TIME:
            return {static_cast<quint16>((0x606E + axisDecal)), 0x0};
        case OD_PV_VELOCITY_THRESHOLD:
            return {static_cast<quint16>((0x606F + axisDecal)), 0x0};
        case OD_PV_VELOCITY_THRESHOLD_TIME:
            return {static_cast<quint16>((0x6070 + axisDecal)), 0x0};
        case OD_TQ_TORQUE_TARGET:
            return {static_cast<quint16>((0x6071 + axisDecal)), 0x0};
        case OD_TQ_MAX_TORQUE:
            return {static_cast<quint16>((0x6072 + axisDecal)), 0x0};
        case OD_TQ_MAX_CURRENT:
            return {static_cast<quint16>((0x6073 + axisDecal)), 0x0};
        case OD_TQ_TORQUE_DEMAND:
            return {static_cast<quint16>((0x6074 + axisDecal)), 0x0};
        case OD_TQ_MOTOR_RATED_CURRENT:
            return {static_cast<quint16>((0x6075 + axisDecal)), 0x0};
        case OD_TQ_MOTOR_RATED_TORQUE:
            return {static_cast<quint16>((0x6076 + axisDecal)), 0x0};
        case OD_TQ_TORQUE_ACTUAL_VALUE:
            return {static_cast<quint16>((0x6077 + axisDecal)), 0x0};
        case OD_TQ_CURRENT_ACTUAL_VALUE:
            return {static_cast<quint16>((0x6078 + axisDecal)), 0x0};
        case OD_TQ_DC_LINK_CIRCUIT_VOLTAGE:
            return {static_cast<quint16>((0x6079 + axisDecal)), 0x0};
        case OD_PP_POSITION_TARGET:
            return {static_cast<quint16>((0x607A + axisDecal)), 0x0};
        case OD_PC_POSITION_RANGE_LIMIT_MIN:
            return {static_cast<quint16>((0x607B + axisDecal)), 0x1};
        case OD_PC_POSITION_RANGE_LIMIT_MAX:
            return {static_cast<quint16>((0x607B + axisDecal)), 0x2};
        case OD_HM_HOME_OFFSET:
            return {static_cast<quint16>((0x607C + axisDecal)), 0x0};
        case OD_PC_SOFTWARE_POSITION_LIMIT_MIN:
            return {static_cast<quint16>((0x607D + axisDecal)), 0x1};
        case OD_PC_SOFTWARE_POSITION_LIMIT_MAX:
            return {static_cast<quint16>((0x607D + axisDecal)), 0x2};
        case OD_FG_POLARITY:
            return {static_cast<quint16>((0x607E + axisDecal)), 0x0};
        case OD_PC_MAX_PROFILE_VELOCITY:
            return {static_cast<quint16>((0x607F + axisDecal)), 0x0};
        case OD_PC_MAX_MOTOR_SPEED:
            return {static_cast<quint16>((0x6080 + axisDecal)), 0x0};
        case OD_PC_PROFILE_VELOCITY:
            return {static_cast<quint16>((0x6081 + axisDecal)), 0x0};
        case OD_PC_END_VELOCITY:
            return {static_cast<quint16>((0x6082 + axisDecal)), 0x0};
        case OD_PC_PROFILE_ACCELERATION:
            return {static_cast<quint16>((0x6083 + axisDecal)), 0x0};
        case OD_PC_PROFILE_DECELERATION:
            return {static_cast<quint16>((0x6084 + axisDecal)), 0x0};
        case OD_PC_QUICK_STOP_DECELERATION:
            return {static_cast<quint16>((0x6085 + axisDecal)), 0x0};
        case OD_PP_MOTION_PROFILE_TYPE:
            return {static_cast<quint16>((0x6086 + axisDecal)), 0x0};
        case OD_TQ_TORQUE_SLOPE:
            return {static_cast<quint16>((0x6087 + axisDecal)), 0x0};
        case OD_TQ_TORQUE_PROFILE_TYPE:
            return {static_cast<quint16>((0x6088 + axisDecal)), 0x0};
        case OD_FG_POSITION_RESOLUTION_ENCODER_INCREMENTS:
            return {static_cast<quint16>((0x608F + axisDecal)), 0x1};
        case OD_FG_POSITION_RESOLUTION_MOTOR_REVOLUTIONS:
            return {static_cast<quint16>((0x608F + axisDecal)), 0x2};
        case OD_FG_VELOCITY_ENCODER_INCREMENTS_PER_SECOND:
            return {static_cast<quint16>((0x6090 + axisDecal)), 0x1};
        case OD_FG_VELOCITY_MOTOR_REVOLUTIONS_PER_SECOND:
            return {static_cast<quint16>((0x6090 + axisDecal)), 0x2};
        case OD_FG_GEAR_RATIO_MOTOR_REVOLUTIONS:
            return {static_cast<quint16>((0x6091 + axisDecal)), 0x1};
        case OD_FG_GEAR_RATIO_SHAFT_REVOLUTIONS:
            return {static_cast<quint16>((0x6091 + axisDecal)), 0x2};
        case OD_FG_FEED_CONSTANT_FEED:
            return {static_cast<quint16>((0x6092 + axisDecal)), 0x1};
        case OD_FG_FEED_CONSTANT_SHAFT_REVOLUTIONS:
            return {static_cast<quint16>((0x6092 + axisDecal)), 0x2};
        case OD_HM_HOMING_METHOD:
            return {static_cast<quint16>((0x6098 + axisDecal)), 0x0};
        case OD_HM_HOMING_SPEED_DURING_SEARCH_FOR_SWITCH:
            return {static_cast<quint16>((0x6099 + axisDecal)), 0x1};
        case OD_HM_HOMING_SPEED_DURING_SEARCH_FOR_ZERO:
            return {static_cast<quint16>((0x6099 + axisDecal)), 0x2};
        case OD_HM_HOMING_ACCELERATION:
            return {static_cast<quint16>((0x609A + axisDecal)), 0x0};
        case OD_PP_PROFILE_JERK_USE:
            return {static_cast<quint16>((0x60A3 + axisDecal)), 0x0};
        case OD_PP_PROFILE_JERK_1:
            return {static_cast<quint16>((0x60A4 + axisDecal)), 0x1};
        case OD_PP_PROFILE_JERK_2:
            return {static_cast<quint16>((0x60A4 + axisDecal)), 0x2};
        case OD_PP_PROFILE_JERK_3:
            return {static_cast<quint16>((0x60A4 + axisDecal)), 0x3};
        case OD_PP_PROFILE_JERK_4:
            return {static_cast<quint16>((0x60A4 + axisDecal)), 0x4};
        case OD_PP_PROFILE_JERK_5:
            return {static_cast<quint16>((0x60A4 + axisDecal)), 0x5};
        case OD_PP_PROFILE_JERK_6:
            return {static_cast<quint16>((0x60A4 + axisDecal)), 0x6};
        case OD_CSP_POSITION_OFFSET:
            return {static_cast<quint16>((0x60B0 + axisDecal)), 0x0};
        case OD_CSP_VELOCITY_OFFSET:
            return {static_cast<quint16>((0x60B1 + axisDecal)), 0x0};
        case OD_CSP_TORQUE_OFFSET:
            return {static_cast<quint16>((0x60B2 + axisDecal)), 0x0};
        case OD_TP_TOUCH_PROBE_FUNCTION:
            return {static_cast<quint16>((0x60B8 + axisDecal)), 0x0};
        case OD_TP_TOUCH_PROBE_STATUS:
            return {static_cast<quint16>((0x60B9 + axisDecal)), 0x0};
        case OD_TP_TOUCH_PROBE_POS_1_POS_VALUE:
            return {static_cast<quint16>((0x60BA + axisDecal)), 0x0};
        case OD_TP_TOUCH_PROBE_POS_1_NEG_VALUE:
            return {static_cast<quint16>((0x60BB + axisDecal)), 0x0};
        case OD_TP_TOUCH_PROBE_POS_2_POS_VALUE:
            return {static_cast<quint16>((0x60BC + axisDecal)), 0x0};
        case OD_TP_TOUCH_PROBE_POS_2_NEG_VALUE:
            return {static_cast<quint16>((0x60BD + axisDecal)), 0x0};
        case OD_IP_SUB_MODE_SELECT:
            return {static_cast<quint16>((0x60C0 + axisDecal)), 0x0};
        case OD_IP_DATA_RECORD_SET_POINT:
            return {static_cast<quint16>((0x60C1 + axisDecal)), 0x1};
        case OD_IP_TIME_PERIOD_TIME_UNITS:
            return {static_cast<quint16>((0x60C2 + axisDecal)), 0x1};
        case OD_IP_TIME_PERIOD_TIME_INDEX:
            return {static_cast<quint16>((0x60C2 + axisDecal)), 0x2};
        case OD_IP_MAXIMUM_BUFFER_SIZE:
            return {static_cast<quint16>((0x60C4 + axisDecal)), 0x1};
        case OD_IP_ACTUAL_BUFFER_SIZE:
            return {static_cast<quint16>((0x60C4 + axisDecal)), 0x2};
        case OD_IP_BUFFER_ORGANIZATION:
            return {static_cast<quint16>((0x60C4 + axisDecal)), 0x3};
        case OD_IP_BUFFER_POSITION:
            return {static_cast<quint16>((0x60C4 + axisDecal)), 0x4};
        case OD_IP_SIZE_OF_DATA_RECORD:
            return {static_cast<quint16>((0x60C4 + axisDecal)), 0x5};
        case OD_IP_BUFFER_CLEAR:
            return {static_cast<quint16>((0x60C4 + axisDecal)), 0x6};
        case OD_PC_MAX_ACCELERATION:
            return {static_cast<quint16>((0x60C5 + axisDecal)), 0x0};
        case OD_PC_MAX_DECELERATION:
            return {static_cast<quint16>((0x60C6 + axisDecal)), 0x0};
        case OD_CSTCA_COMMUTATION_ANGLE:
            return {static_cast<quint16>((0x60EA + axisDecal)), 0x0};
        case OD_PC_POSITIONING_OPTION_CODE:
            return {static_cast<quint16>((0x60F2 + axisDecal)), 0x0};
        case OD_PC_FOLLOWING_ERROR_ACTUAL_VALUE:
            return {static_cast<quint16>((0x60F4 + axisDecal)), 0x0};
        case OD_PV_MAX_SLIPPAGE:
            return {static_cast<quint16>((0x60F8 + axisDecal)), 0x0};
        case OD_PC_CONTROL_EFFORT:
            return {static_cast<quint16>((0x60FA + axisDecal)), 0x0};
        case OD_DIGITAL_INPUTS:
            return {static_cast<quint16>((0x60FD + axisDecal)), 0x0};
        case OD_DIGITAL_OUTPUTS_PHYSICAL_OUTPUTS:
            return {static_cast<quint16>((0x60FE + axisDecal)), 0x1};
        case OD_DIGITAL_OUTPUTS_BIT_MASK:
            return {static_cast<quint16>((0x60FE + axisDecal)), 0x2};
        case OD_PV_VELOCITY_TARGET:
            return {static_cast<quint16>((0x60FF + axisDecal)), 0x0};
        case OD_MOTOR_TYPE:
            return {static_cast<quint16>((0x6402 + axisDecal)), 0x0};
        case OD_MOTOR_CATALOGUE_NUMBER:
            return {static_cast<quint16>((0x6403 + axisDecal)), 0x0};
        case OD_MOTOR_MANUFACTURER:
            return {static_cast<quint16>((0x6404 + axisDecal)), 0x0};
        case OD_HTTP_MOTOR_CATALOGUE_ADDRESS:
            return {static_cast<quint16>((0x6405 + axisDecal)), 0x0};
        case OD_MOTOR_SERVICE_PERIOD:
            return {static_cast<quint16>((0x6407 + axisDecal)), 0x0};
        case OD_SUPPORTED_DRIVE_MODES:
            return {static_cast<quint16>((0x6502 + axisDecal)), 0x0};
        case OD_DRIVE_CATALOGUE_NUMBER:
            return {static_cast<quint16>((0x6503 + axisDecal)), 0x0};
        case OD_HTTP_DRIVE_CATALOGUE_ADDRESS:
            return {static_cast<quint16>((0x6505 + axisDecal)), 0x0};
        case OD_VERSION_NUMBER:
            return {static_cast<quint16>((0x67FE + axisDecal)), 0x0};
        case OD_SINGLE_DEVICE_TYPE:
            return {static_cast<quint16>((0x67FF + axisDecal)), 0x0};
        default:
            return NodeObjectId();
    }
}

NodeObjectId IndexDb402::getObjectIdMs(IndexDb402::OdObject object, uint axis, uint opt2)
{
    quint16 axisDecal = 0x0200 * static_cast<quint16>(axis);
    quint16 modeDecal = 0x0020 * static_cast<quint16>(opt2);

    switch (object)
    {
        // S12_SYNCHRO_STATUS
        case S12_SYNCHRO_STATUS_FLAG:
            return {0x2A00, 0x1};
        case S12_SYNCHRO_STATUS_ERROR:
            return {0x2A00, 0x2};
        case S12_SYNCHRO_STATUS_CORRECTOR:
            return {0x2A00, 0x3};

        // S12_SYNCHRO_CONFIG
        case S12_SYNCHRO_CONFIG_MODE_SYNCHRO:
            return {0x2A01, 0x1};
        case S12_SYNCHRO_CONFIG_MAX_DIFF:
            return {0x2A01, 0x2};
        case S12_SYNCHRO_CONFIG_COEFF:
            return {0x2A01, 0x3};
        case S12_SYNCHRO_CONFIG_WINDOW:
            return {0x2A01, 0x4};
        case S12_SYNCHRO_CONFIG_OFFSET:
            return {0x2A01, 0x5};

        // STATUS
        case OD_MS_BOARD_VOLTAGE_INPUT:
            return {0x2000, 0x1};
        case OD_MS_MANUFACTURE_DATE:
            return {0x2001, 0x0};
        case OD_MS_CALIBRATION_DATE:
            return {0x2002, 0x0};
        case OD_MS_FIRMWARE_BUILD_DATE:
            return {0x2003, 0x0};
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
        case OD_MS_NODE_ID:
            return {0x2040, 0x1};
        case OD_MS_BIT_RATE:
            return {0x2040, 0x2};

        case OD_MS_DRIVER_TEMPERATURE:
            return NodeObjectId(static_cast<quint16>((0x2801 + axisDecal)), static_cast<quint8>(opt2 + 1));
        case OD_MS_CURRENT_HL:
            return NodeObjectId(static_cast<quint16>((0x2802 + axisDecal)), static_cast<quint8>(opt2 + 1));
        case OD_MS_CURRENT_LL:
            return NodeObjectId(static_cast<quint16>((0x2803 + axisDecal)), static_cast<quint8>(opt2 + 1));
        case OD_MS_PWM:
            return NodeObjectId(static_cast<quint16>((0x2804 + axisDecal)), static_cast<quint8>(opt2 + 1));
        case OD_MS_BACK_EMF:
            return NodeObjectId(static_cast<quint16>((0x2805 + axisDecal)), static_cast<quint8>(opt2 + 1));

        case OD_MS_DRIVER_TEMPERATURE_CONFIG_PROTECTION_SCHMITT_TRIGGERS_LOW:
            return NodeObjectId(static_cast<quint16>(0x2810), 0x1);
        case OD_MS_DRIVER_TEMPERATURE_CONFIG_PROTECTION_SCHMITT_TRIGGERS_HIGH:
            return NodeObjectId(static_cast<quint16>(0x2810), 0x2);

        // ax_Motion_status
        case OD_MS_MOTION_STATUS_ERROR:
            return {static_cast<quint16>(0x4000 + axisDecal), 0x1};

        // ax_Motor_status
        case OD_MS_MOTOR_STATUS_COMMAND:
            return {static_cast<quint16>((0x4006 + axisDecal)), 0x1};
        case OD_MS_MOTOR_STATUS_CURRENT:
            return {static_cast<quint16>((0x4006 + axisDecal)), 0x2};
        case OD_MS_MOTOR_STATUS_TORQUE:
            return {static_cast<quint16>((0x4006 + axisDecal)), 0x3};
        case OD_MS_MOTOR_STATUS_VELOCITY:
            return {static_cast<quint16>((0x4006 + axisDecal)), 0x4};
        case OD_MS_MOTOR_STATUS_POSITION:
            return {static_cast<quint16>((0x4006 + axisDecal)), 0x5};
        case OD_MS_MOTOR_STATUS_ERROR:
            return {static_cast<quint16>((0x4006 + axisDecal)), 0x6};
        case OD_MS_MOTOR_STATUS_TEMP:
            return {static_cast<quint16>((0x4006 + axisDecal)), 0x7};

        // ax_Motor_config
        case OD_MS_MOTOR_CONFIG_TYPE:
            return {static_cast<quint16>((0x4007 + axisDecal)), 0x1};
        case OD_MS_MOTOR_CONFIG_PEAK_CURRENT:
            return {static_cast<quint16>((0x4007 + axisDecal)), 0x2};
        case OD_MS_MOTOR_CONFIG_BURST_CURRENT:
            return {static_cast<quint16>((0x4007 + axisDecal)), 0x3};
        case OD_MS_MOTOR_CONFIG_BURST_DURATION:
            return {static_cast<quint16>((0x4007 + axisDecal)), 0x4};
        case OD_MS_MOTOR_CONFIG_SUSTAINED_CURRENT:
            return {static_cast<quint16>((0x4007 + axisDecal)), 0x5};
        case OD_MS_MOTOR_CONFIG_CURRENT_CONSTANT:
            return {static_cast<quint16>((0x4007 + axisDecal)), 0x6};
        case OD_MS_MOTOR_CONFIG_MAX_VELOCITY:
            return {static_cast<quint16>((0x4007 + axisDecal)), 0x7};
        case OD_MS_MOTOR_CONFIG_VELOCITY_CONSTANT:
            return {static_cast<quint16>((0x4007 + axisDecal)), 0x8};
        case OD_MS_MOTOR_CONFIG_FLAGS:
            return {static_cast<quint16>((0x4007 + axisDecal)), 0x9};

        // ax_BLDC_status
        case OD_MS_BLDC_STATUS_HALL_RAW:
            return {static_cast<quint16>((0x4008 + axisDecal)), 0x1};
        case OD_MS_BLDC_STATUS_HALL_PHASE:
            return {static_cast<quint16>((0x4008 + axisDecal)), 0x2};
        case OD_MS_BLDC_STATUS_ELECTRICAL_ANGLE:
            return {static_cast<quint16>((0x4008 + axisDecal)), 0x3};

        // ax_BLDC_config
        case OD_MS_BLDC_CONFIG_POLE_PAIR:
            return {static_cast<quint16>((0x4009 + axisDecal)), 0x1};

        // ax_Brake_config
        case OD_MS_BRAKE_MODE:
            return {static_cast<quint16>((0x4082 + axisDecal)), 0x1};
        case OD_MS_BRAKE_EXITATION_TIME:
            return {static_cast<quint16>((0x4082 + axisDecal)), 0x2};
        case OD_MS_BRAKE_EXITATION_DUTY:
            return {static_cast<quint16>((0x4082 + axisDecal)), 0x3};
        case OD_MS_BRAKE_ACTIVATED_DUTY:
            return {static_cast<quint16>((0x4082 + axisDecal)), 0x4};

        // RANGE CONTROL LOOP TORQUE
        // ax_NNN_status_PID
        case OD_PID_INPUT:
            return {static_cast<quint16>((0x4020 + axisDecal + modeDecal)), 0x1};
        case OD_PID_ERROR:
            return {static_cast<quint16>((0x4020 + axisDecal + modeDecal)), 0x2};
        case OD_PID_INTEGRATOR:
            return {static_cast<quint16>((0x4020 + axisDecal + modeDecal)), 0x3};
        case OD_PID_OUTPUT:
            return {static_cast<quint16>((0x4020 + axisDecal + modeDecal)), 0x4};

        // ax_NNN_config_PID
        case OD_PID_P:
            return {static_cast<quint16>((0x4021 + axisDecal + modeDecal)), 0x1};
        case OD_PID_I:
            return {static_cast<quint16>((0x4021 + axisDecal + modeDecal)), 0x2};
        case OD_PID_D:
            return {static_cast<quint16>((0x4021 + axisDecal + modeDecal)), 0x3};
        case OD_PID_MIN:
            return {static_cast<quint16>((0x4021 + axisDecal + modeDecal)), 0x4};
        case OD_PID_MAX:
            return {static_cast<quint16>((0x4021 + axisDecal + modeDecal)), 0x5};
        case OD_PID_THRESHOLD:
            return {static_cast<quint16>((0x4021 + axisDecal + modeDecal)), 0x6};
        case OD_PID_FREQDIVIDER:
            return {static_cast<quint16>((0x4021 + axisDecal + modeDecal)), 0x7};
        case OD_PID_CONFIGBIT:
            return {static_cast<quint16>((0x4021 + axisDecal + modeDecal)), 0x8};

        // ax_NNN_sensor_status
        case OD_SENSOR_STATUS_RAW_DATA:
            return {static_cast<quint16>((0x4022 + axisDecal + modeDecal)), 0x1};
        case OD_SENSOR_STATUS_FLAGS:
            return {static_cast<quint16>((0x4022 + axisDecal + modeDecal)), 0x2};
        case OD_SENSOR_STATUS_VALUE:
            return {static_cast<quint16>((0x4022 + axisDecal + modeDecal)), 0x3};

        // ax_NNN_sensor_config
        case OD_SENSOR_SELECT:
            return {static_cast<quint16>((0x4023 + axisDecal + modeDecal)), 0x1};
        case OD_SENSOR_FREQUENCY_DIVIDER:
            return {static_cast<quint16>((0x4023 + axisDecal + modeDecal)), 0x2};
        case OD_SENSOR_CONFIG_BIT:
            return {static_cast<quint16>((0x4023 + axisDecal + modeDecal)), 0x3};
        case OD_SENSOR_PARAM_0:
            return {static_cast<quint16>((0x4023 + axisDecal + modeDecal)), 0x4};
        case OD_SENSOR_PARAM_1:
            return {static_cast<quint16>((0x4023 + axisDecal + modeDecal)), 0x5};
        case OD_SENSOR_PARAM_2:
            return {static_cast<quint16>((0x4023 + axisDecal + modeDecal)), 0x6};
        case OD_SENSOR_PARAM_3:
            return {static_cast<quint16>((0x4023 + axisDecal + modeDecal)), 0x7};

        // ax_NNN_sensor_filter
        case OD_SENSOR_FILTER_SELECT:
            return {static_cast<quint16>((0x4024 + axisDecal + modeDecal)), 0x1};
        case OD_SENSOR_FILTER_PARAM_0:
            return {static_cast<quint16>((0x4024 + axisDecal + modeDecal)), 0x2};
        case OD_SENSOR_FILTER_PARAM_1:
            return {static_cast<quint16>((0x4024 + axisDecal + modeDecal)), 0x3};
        case OD_SENSOR_FILTER_PARAM_2:
            return {static_cast<quint16>((0x4024 + axisDecal + modeDecal)), 0x4};
        case OD_SENSOR_FILTER_PARAM_3:
            return {static_cast<quint16>((0x4024 + axisDecal + modeDecal)), 0x5};

        // ax_NNN_sensor_conditioning
        case OD_SENSOR_PRE_OFFSET:
            return {static_cast<quint16>((0x4025 + axisDecal + modeDecal)), 0x1};
        case OD_SENSOR_SCALE:
            return {static_cast<quint16>((0x4025 + axisDecal + modeDecal)), 0x2};
        case OD_SENSOR_POST_OFFSET:
            return {static_cast<quint16>((0x4025 + axisDecal + modeDecal)), 0x3};
        case OD_SENSOR_ERROR_MIN:
            return {static_cast<quint16>((0x4025 + axisDecal + modeDecal)), 0x4};
        case OD_SENSOR_ERROR_MAX:
            return {static_cast<quint16>((0x4025 + axisDecal + modeDecal)), 0x5};
        case OD_SENSOR_THRESHOLD_MIN:
            return {static_cast<quint16>((0x4025 + axisDecal + modeDecal)), 0x6};
        case OD_SENSOR_THRESHOLD_MAX:
            return {static_cast<quint16>((0x4025 + axisDecal + modeDecal)), 0x7};
        case OD_SENSOR_THRESHOLD_MODE:
            return {static_cast<quint16>((0x4025 + axisDecal + modeDecal)), 0x8};

        // CONFIGURATION OF AXIS
        case OD_MS_DRIVER_TEMP_CONFIG_PROTECTION_SCHMITT_TRIGGERS_LOW:
            return {static_cast<quint16>((0x4080 + axisDecal)), 0x1};
        case OD_MS_DRIVER_TEMP_CONFIG_PROTECTION_SCHMITT_TRIGGERS_HIGH:
            return {static_cast<quint16>((0x4080 + axisDecal)), 0x2};

        case OD_MS_MOTOR_TEMP_CONFIG_SENSOR_TYPESOR_TYPE:
            return {static_cast<quint16>((0x4081 + axisDecal)), 0x1};
        case OD_MS_MOTOR_TEMP_CONFIG_SENSOR_CONSTANT:
            return {static_cast<quint16>((0x4081 + axisDecal)), 0x2};
        case OD_MS_MOTOR_TEMP_CONFIG_PROTECTION_SCHMITT_TRIGGERS_LOW:
            return {static_cast<quint16>((0x4081 + axisDecal)), 0x3};
        case OD_MS_MOTOR_TEMP_CONFIG_PROTECTION_SCHMITT_TRIGGERS_HIGH:
            return {static_cast<quint16>((0x4081 + axisDecal)), 0x4};

        // CONTINUOUS POSITION
        case OD_CP_POSITION_TARGET:
            return {static_cast<quint16>((0x41F0 + axisDecal)), 0x0};

        // DUTY CYCLE MODE
        case OD_MS_DUTY_CYCLE_MODE_TARGET:
            return {static_cast<quint16>((0x41FA + axisDecal)), 0x0};
        case OD_MS_DUTY_CYCLE_MODE_DEMAND:
            return {static_cast<quint16>((0x41FB + axisDecal)), 0x0};
        case OD_MS_DUTY_CYCLE_MODE_MAX:
            return {static_cast<quint16>((0x41FC + axisDecal)), 0x0};
        case OD_MS_DUTY_CYCLE_MODE_SLOPE:
            return {static_cast<quint16>((0x41FD + axisDecal)), 0x0};
        default:
            return NodeObjectId();
    }
}
