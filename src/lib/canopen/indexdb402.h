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

#ifndef INDEXDB402_H
#define INDEXDB402_H

#include "canopen_global.h"

class NodeObjectId;

class CANOPEN_EXPORT IndexDb402
{
public:
    enum OdObject
    {

        // S12_SYNCHRO_STATUS
        S12_SYNCHRO_STATUS_FLAG,
        S12_SYNCHRO_STATUS_ERROR,
        S12_SYNCHRO_STATUS_CORRECTOR,

        // S12_SYNCHRO_CONFIG
        S12_SYNCHRO_CONFIG_MODE_SYNCHRO,
        S12_SYNCHRO_CONFIG_MAX_DIFF,
        S12_SYNCHRO_CONFIG_COEFF,
        S12_SYNCHRO_CONFIG_WINDOW,
        S12_SYNCHRO_CONFIG_OFFSET,

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
        OD_MS_NODE_ID,
        OD_MS_BIT_RATE,
        OD_MS_TEMPERATURE_MOTOR_1,
        OD_MS_TEMPERATURE_DRIVER_1,
        OD_MS_TEMPERATURE_DRIVER_2,
        OD_MS_BACK_EMF_A,
        OD_MS_BACK_EMF_B,
        OD_MS_BACK_EMF_C,
        OD_MS_BACK_EMF_D,
        OD_MS_CURRENT_A,
        OD_MS_CURRENT_B,
        OD_MS_CURRENT_C,
        OD_MS_CURRENT_D,
        OD_MS_BLDC_STATUS,
        OD_MS_BLDC_CODER,
        OD_MS_BLDC_TIME_CODER,
        OD_MS_BLDC_PHASE,
        OD_MS_BLDC_BRIDGE_PWM,

        OD_MS_TORQUE_PID_INPUT,
        OD_MS_TORQUE_PID_ERROR,
        OD_MS_TORQUE_PID_INTEGRATOR,
        OD_MS_TORQUE_PID_OUTPUT,
        OD_MS_TORQUE_PID_P,
        OD_MS_TORQUE_PID_I,
        OD_MS_TORQUE_PID_D,
        OD_MS_TORQUE_PID_MIN,
        OD_MS_TORQUE_PID_MAX,
        OD_MS_TORQUE_PID_THRESHOLD,
        OD_MS_TORQUE_SENSOR_STATUS_RAWDATA,
        OD_MS_TORQUE_SENSOR_STATUS_FLAG,
        OD_MS_TORQUE_SENSOR_SELECT,
        OD_MS_TORQUE_SENSOR_THRESHOLD_MIN,
        OD_MS_TORQUE_SENSOR_THRESHOLD_MAX,
        OD_MS_TORQUE_SENSOR_THRESHOLD_MODE,
        OD_MS_TORQUE_SENSOR_PRE_OFFSET,
        OD_MS_TORQUE_SENSOR_SCALE,
        OD_MS_TORQUE_SENSOR_POST_OFFSET,
        OD_MS_TORQUE_SENSOR_ERROR_MIN,
        OD_MS_TORQUE_SENSOR_ERROR_MAX,
        OD_MS_TORQUE_SENSOR_FILTER_SELECT,
        OD_MS_TORQUE_SENSOR_FILTER_PARAM_0,
        OD_MS_TORQUE_SENSOR_FILTER_PARAM_1,
        OD_MS_TORQUE_SENSOR_FILTER_PARAM_2,
        OD_MS_TORQUE_SENSOR_FILTER_PARAM_3,

        OD_MS_VELOCITY_PID_INPUT,
        OD_MS_VELOCITY_PID_ERROR,
        OD_MS_VELOCITY_PID_INTEGRATOR,
        OD_MS_VELOCITY_PID_OUTPUT,
        OD_MS_VELOCITY_PID_P,
        OD_MS_VELOCITY_PID_I,
        OD_MS_VELOCITY_PID_D,
        OD_MS_VELOCITY_PID_MIN,
        OD_MS_VELOCITY_PID_MAX,
        OD_MS_VELOCITY_PID_THRESHOLD,
        OD_MS_VELOCITY_SENSOR_STATUS_RAWDATA,
        OD_MS_VELOCITY_SENSOR_STATUS_FLAG,
        OD_MS_VELOCITY_SENSOR_SELECT,
        OD_MS_VELOCITY_SENSOR_THRESHOLD_MIN,
        OD_MS_VELOCITY_SENSOR_THRESHOLD_MAX,
        OD_MS_VELOCITY_SENSOR_THRESHOLD_MODE,
        OD_MS_VELOCITY_SENSOR_PRE_OFFSET,
        OD_MS_VELOCITY_SENSOR_SCALE,
        OD_MS_VELOCITY_SENSOR_POST_OFFSET,
        OD_MS_VELOCITY_SENSOR_ERROR_MIN,
        OD_MS_VELOCITY_SENSOR_ERROR_MAX,
        OD_MS_VELOCITY_SENSOR_FILTER_SELECT,
        OD_MS_VELOCITY_SENSOR_FILTER_PARAM_0,
        OD_MS_VELOCITY_SENSOR_FILTER_PARAM_1,
        OD_MS_VELOCITY_SENSOR_FILTER_PARAM_2,
        OD_MS_VELOCITY_SENSOR_FILTER_PARAM_3,

        OD_MS_POSITION_PID_INPUT,
        OD_MS_POSITION_PID_ERROR,
        OD_MS_POSITION_PID_INTEGRATOR,
        OD_MS_POSITION_PID_OUTPUT,
        OD_MS_POSITION_PID_P,
        OD_MS_POSITION_PID_I,
        OD_MS_POSITION_PID_D,
        OD_MS_POSITION_PID_MIN,
        OD_MS_POSITION_PID_MAX,
        OD_MS_POSITION_PID_THRESHOLD,
        OD_MS_POSITION_SENSOR_STATUS_RAWDATA,
        OD_MS_POSITION_SENSOR_STATUS_FLAG,

        OD_MS_POSITION_SENSOR_SELECT,
        OD_MS_POSITION_SENSOR_THRESHOLD_MIN,
        OD_MS_POSITION_SENSOR_THRESHOLD_MAX,
        OD_MS_POSITION_SENSOR_THRESHOLD_MODE,
        OD_MS_POSITION_SENSOR_PRE_OFFSET,
        OD_MS_POSITION_SENSOR_SCALE,
        OD_MS_POSITION_SENSOR_POST_OFFSET,
        OD_MS_POSITION_SENSOR_ERROR_MIN,
        OD_MS_POSITION_SENSOR_ERROR_MAX,

        OD_MS_POSITION_SENSOR_FILTER_SELECT,
        OD_MS_POSITION_SENSOR_FILTER_PARAM_0,
        OD_MS_POSITION_SENSOR_FILTER_PARAM_1,
        OD_MS_POSITION_SENSOR_FILTER_PARAM_2,
        OD_MS_POSITION_SENSOR_FILTER_PARAM_3,

        OD_MS_CONF_MOTOR_TYPE,
        OD_MS_CONF_MOTOR_PEAK_CURRENT,
        OD_MS_CONF_MOTOR_POLE_PAIR,
        OD_MS_CONF_MOTOR_MAX_VELOCITY,
        OD_MS_CONF_MOTOR_VELOCITY_CONSTANT,
        OD_MS_CONF_MOTOR_CURRENT_TEMPERATURE,
        OD_MS_CONF_TEMP_SENSOR_TYPE,
        OD_MS_CONF_TEMP_SENSOR_CONSTANT,
        OD_MS_CONF_TEMP_MAX,
        OD_MS_CONF_BRAKE_MAINTAIN,
        OD_MS_CONF_BRAKE_PEAK,

        // Object APP
        OD_ABORT_CONNECTION_OPTION,
        OD_CONTROLWORD,
        OD_STATUSWORD,
        OD_VL_VELOCITY_TARGET,
        OD_VL_VELOCITY_DEMAND,
        OD_VL_VELOCITY_ACTUAL_VALUE,
        OD_VL_MIN,
        OD_VL_MAX,
        OD_VL_ACCELERATION_DELTA_SPEED,
        OD_VL_ACCELERATION_DELTA_TIME,
        OD_VL_DECELERATION_DELTA_SPEED,
        OD_VL_DECELERATION_DELTA_TIME,
        OD_VL_QUICK_STOP_DELTA_SPEED,
        OD_VL_QUICK_STOP_DELTA_TIME,
        OD_VL_SET_POINT_FACTOR_NUMERATOR,
        OD_VL_SET_POINT_FACTOR_DENOMINATOR,
        OD_VL_DIMENSION_FACTOR_NUMERATOR,
        OD_VL_DIMENSION_FACTOR_DENOMINATOR,
        OD_QUICK_STOP_OPTION,
        OD_SHUTDOWN_OPTION,
        OD_DISABLE_OPERATION_OPTION,
        OD_HALT_OPTION,
        OD_FAULT_REACTION_OPTION,
        OD_MODES_OF_OPERATION,
        OD_MODES_OF_OPERATION_DISPLAY,
        OD_PC_POSITION_DEMAND_VALUE,
        OD_PC_POSITION_ACTUAL_INTERNAL_VALUE,
        OD_PC_POSITION_ACTUAL_VALUE,
        OD_PC_FOLLOWING_ERROR_WINDOW,
        OD_PC_FOLLOWING_ERROR_TIME_OUT,
        OD_PC_POSITION_WINDOW,
        OD_PC_POSITION_WINDOW_TIME,
        OD_PV_VELOCITY_SENSOR_ACTUAL_VALUE,
        OD_PV_SENSOR_SELECTION_CODE,
        OD_PV_VELOCITY_DEMAND_VALUE,
        OD_PV_VELOCITY_ACTUAL_VALUE,
        OD_PV_VELOCITY_WINDOW,
        OD_PV_VELOCITY_WINDOW_TIME,
        OD_PV_VELOCITY_THRESHOLD,
        OD_PV_VELOCITY_THRESHOLD_TIME,
        OD_TQ_TARGET_TORQUE,
        OD_TQ_MAX_TORQUE,
        OD_TQ_MAX_CURRENT,
        OD_TQ_TORQUE_DEMAND,
        OD_TQ_MOTOR_RATED_CURRENT,
        OD_TQ_MOTOR_RATED_TORQUE,
        OD_TQ_TORQUE_ACTUAL_VALUE,
        OD_TQ_CURRENT_ACTUAL_VALUE,
        OD_TQ_DC_LINK_CIRCUIT_VOLTAGE,
        OD_PP_TARGET_POSITION,
        OD_PC_POSITION_RANGE_LIMIT_MIN,
        OD_PC_POSITION_RANGE_LIMIT_MAX,
        OD_HM_HOME_OFFSET,
        OD_PC_SOFTWARE_POSITION_LIMIT_MIN,
        OD_PC_SOFTWARE_POSITION_LIMIT_MAX,
        OD_FG_POLARITY,
        OD_PC_MAX_PROFILE_VELOCITY,
        OD_PC_MAX_MOTOR_SPEED,
        OD_PC_PROFILE_VELOCITY,
        OD_PC_END_VELOCITY,
        OD_PC_PROFILE_ACCELERATION,
        OD_PC_PROFILE_DECELERATION,
        OD_PC_QUICK_STOP_DECELERATION,
        OD_PP_MOTION_PROFILE_TYPE,
        OD_TQ_TORQUE_SLOPE,
        OD_TQ_TORQUE_PROFILE_TYPE,
        OD_FG_POSITION_RESOLUTION_ENCODER_INCREMENTS,
        OD_FG_POSITION_RESOLUTION_MOTOR_REVOLUTIONS,
        OD_FG_VELOCITY_ENCODER_INCREMENTS_PER_SECOND,
        OD_FG_VELOCITY_MOTOR_REVOLUTIONS_PER_SECOND,
        OD_FG_GEAR_RATIO_MOTOR_REVOLUTIONS,
        OD_FG_GEAR_RATIO_SHAFT_REVOLUTIONS,
        OD_FG_FEED_CONSTANT_FEED,
        OD_FG_FEED_CONSTANT_SHAFT_REVOLUTIONS,
        OD_HM_HOMING_METHOD,
        OD_HM_HOMING_SPEED_DURING_SEARCH_FOR_SWITCH,
        OD_HM_HOMING_SPEED_DURING_SEARCH_FOR_ZERO,
        OD_HM_HOMING_ACCELERATION,
        OD_PP_PROFILE_JERK_USE,
        OD_PP_PROFILE_JERK_1,
        OD_PP_PROFILE_JERK_2,
        OD_PP_PROFILE_JERK_3,
        OD_PP_PROFILE_JERK_4,
        OD_PP_PROFILE_JERK_5,
        OD_PP_PROFILE_JERK_6,
        OD_CSP_POSITION_OFFSET,
        OD_CSP_VELOCITY_OFFSET,
        OD_CSP_TORQUE_OFFSET,
        OD_TP_TOUCH_PROBE_FUNCTION,
        OD_TP_TOUCH_PROBE_STATUS,
        OD_TP_TOUCH_PROBE_POS_1_POS_VALUE,
        OD_TP_TOUCH_PROBE_POS_1_NEG_VALUE,
        OD_TP_TOUCH_PROBE_POS_2_POS_VALUE,
        OD_TP_TOUCH_PROBE_POS_2_NEG_VALUE,
        OD_IP_SUB_MODE_SELECT,
        OD_IP_SET_POINT,
        OD_IP_TIME_UNITS,
        OD_IP_TIME_INDEX,
        OD_IP_MAXIMUM_BUFFER_SIZE,
        OD_IP_ACTUAL_BUFFER_SIZE,
        OD_IP_BUFFER_ORGANIZATION,
        OD_IP_BUFFER_POSITION,
        OD_IP_SIZE_OF_DATA_RECORD,
        OD_IP_BUFFER_CLEAR,
        OD_PC_MAX_ACCELERATION,
        OD_PC_MAX_DECELERATION,
        OD_PC_POSITIONING_OPTION_CODE,
        OD_PC_FOLLOWING_ERROR_ACTUAL_VALUE,
        OD_PV_MAX_SLIPPAGE,
        OD_PC_CONTROL_EFFORT,
        OD_DIGITAL_INPUTS,
        OD_DIGITAL_OUTPUTS_PHYSICAL_OUTPUTS,
        OD_DIGITAL_OUTPUTS_BIT_MASK,
        OD_PV_TARGET_VELOCITY,
        OD_MOTOR_TYPE,
        OD_MOTOR_CATALOGUE_NUMBER,
        OD_MOTOR_MANUFACTURER,
        OD_HTTP_MOTOR_CATALOGUE_ADDRESS,
        OD_MOTOR_SERVICE_PERIOD,
        OD_SUPPORTED_DRIVE_MODES,
        OD_DRIVE_CATALOGUE_NUMBER,
        OD_HTTP_DRIVE_CATALOGUE_ADDRESS,
        OD_VERSION_NUMBER,
        OD_SINGLE_DEVICE_TYPE,
    };

    static NodeObjectId getObjectId(OdObject object, uint axis = 0, uint opt2 = 0);

private:
    static NodeObjectId getObjectIdMs(OdObject object, uint axis = 0, uint opt2 = 0);
    // FUTURE
    static QString name(const NodeObjectId &nodeObjectId);
    static QString name(IndexDb402::OdObject object, uint axis, uint opt2);
    //    static QVariant min(const NodeObjectId &nodeObjectId);
    //    static QVariant max(const NodeObjectId &nodeObjectId);
    //    static QVariant defaultValue(const NodeObjectId &nodeObjectId);
    //    static QString unit(const NodeObjectId &nodeObjectId);
    //    static QString description(const NodeObjectId &nodeObjectId);
};

#endif // INDEXDB402_H
