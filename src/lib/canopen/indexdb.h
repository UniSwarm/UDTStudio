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

#ifndef INDEXDB_H
#define INDEXDB_H

#include "canopen_global.h"

class NodeObjectId;

class CANOPEN_EXPORT IndexDb
{
public:
    enum OdObject
    {
        OD_DEVICE_TYPE,
        OD_ERROR_REGISTER,
        OD_STANDARD_ERROR_FIELD,
        OD_COB_ID_SYNC,
        OD_MANUFACTURER_DEVICE_NAME,
        OD_MANUFACTURER_HARDWARE_VERSION,
        OD_MANUFACTURER_SOFTWARE_VERSION,
        OD_GUARD_TIME,
        OD_LIFE_TIME_FACTOR,

        OD_STORE,
        OD_RESTORE,

        OD_COB_ID_EMCY,
        OD_CONSUMER_HEARTBEAT_TIME,
        OD_PRODUCER_HEARTBEAT_TIME,
        OD_VENDOR_ID,
        OD_PRODUCT_CODE,
        OD_REVISION_NUMBER,
        OD_SERIAL_NUMBER,
        OD_COMMUNICATION_ERROR,
        OD_COB_ID_CLIENT_TO_SERVER,
        OD_COB_ID_SERVER_TO_CLIENT,

        OD_RPDO_COB_ID,
        OD_RPDO_TRANSMISSION_TYPE,
        OD_RPDO_INHIBIT_TIME,
        OD_RPDO_EVENT_TIMER,
        OD_RPDO_MAPPING_ENTRY,

        OD_TPDO_COB_ID,
        OD_TPDO_TRANSMISSION_TYPE,
        OD_TPDO_INHIBIT_TIME,
        OD_TPDO_EVENT_TIMER,
        OD_TPDO_SYNC_START_VALUE,
        OD_TPDO_MAPPING_ENTRY,

        OD_PROGRAM_DATA_1,
        OD_PROGRAM_CONTROL_1,
        OD_BOOTLOADER_KEY,
        OD_BOOTLOADER_CHECKSUM,
        OD_BOOTLOADER_STATUS
    };

    static NodeObjectId getObjectId(OdObject object, uint opt = 0, uint opt2 = 0);

    // FUTURE
    //    static QString name(const NodeObjectId &nodeObjectId);
    //    static QVariant min(const NodeObjectId &nodeObjectId);
    //    static QVariant max(const NodeObjectId &nodeObjectId);
    //    static QVariant defaultValue(const NodeObjectId &nodeObjectId);
    //    static QString unit(const NodeObjectId &nodeObjectId);
    //    static QString description(const NodeObjectId &nodeObjectId);

    static bool isQ1516(const NodeObjectId &objId, quint16 profileNumber);
};

#endif // INDEXDB_H
