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

#include "indexdb.h"

#include "../canopen/nodeobjectid.h"

#include "db/odindexdb.h"

NodeObjectId IndexDb::getObjectId(IndexDb::OdObject object, uint opt, uint optMappingEntry)
{
    quint16 index = 0;
    switch (object)
    {
        case OD_DEVICE_TYPE:
            return {0x1000, 0x0};
        case OD_ERROR_REGISTER:
            return {0x1001, 0x0};
        case OD_STANDARD_ERROR_FIELD:
            return {0x1003, 0x1};
        case OD_COB_ID_SYNC:
            return {0x1005, 0x0};
        case OD_MANUFACTURER_DEVICE_NAME:
            return {0x1008, 0x0};
        case OD_MANUFACTURER_HARDWARE_VERSION:
            return {0x1009, 0x0};
        case OD_MANUFACTURER_SOFTWARE_VERSION:
            return {0x100A, 0x0};
        case OD_GUARD_TIME:
            return {0x100C, 0x0};
        case OD_LIFE_TIME_FACTOR:
            return {0x100D, 0x0};

        case OD_STORE:
            return {0x1010, static_cast<quint8>(opt)};
        case OD_RESTORE:
            return {0x1011, static_cast<quint8>(opt)};

        case OD_COB_ID_EMCY:
            return {0x1014, 0x0};
        case OD_CONSUMER_HEARTBEAT_TIME:
            return {0x1016, 0x1};
        case OD_PRODUCER_HEARTBEAT_TIME:
            return {0x1017, 0x0};
        case OD_VENDOR_ID:
            return {0x1018, 0x1};
        case OD_PRODUCT_CODE:
            return {0x1018, 0x2};
        case OD_REVISION_NUMBER:
            return {0x1018, 0x3};
        case OD_SERIAL_NUMBER:
            return {0x1018, 0x4};
        case OD_COMMUNICATION_ERROR:
            return {0x1029, 0x1};
        case OD_COB_ID_CLIENT_TO_SERVER:
            return {0x1200, 0x1};
        case OD_COB_ID_SERVER_TO_CLIENT:
            return {0x1200, 0x2};
        case OD_RPDO_COB_ID:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1400;
                return {index, 0x1};
            }
            return NodeObjectId();
        case OD_RPDO_TRANSMISSION_TYPE:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1400;
                return {index, 0x2};
            }
            return NodeObjectId();
        case OD_RPDO_INHIBIT_TIME:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1400;
                return {index, 0x3};
            }
            return NodeObjectId();
        case OD_RPDO_EVENT_TIMER:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1400;
                return {index, 0x5};
            }
            return NodeObjectId();
        case OD_RPDO_MAPPING_ENTRY:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1600;
                return {index, static_cast<quint8>(optMappingEntry)};
            }
            return NodeObjectId();

        case OD_TPDO_COB_ID:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1800;
                return {index, 0x1};
            }
            return NodeObjectId();
        case OD_TPDO_TRANSMISSION_TYPE:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1800;
                return {index, 0x2};
            }
            return NodeObjectId();
        case OD_TPDO_INHIBIT_TIME:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1800;
                return {index, 0x3};
            }
            return NodeObjectId();
        case OD_TPDO_EVENT_TIMER:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1800;
                return {index, 0x5};
            }
            return NodeObjectId();
        case OD_TPDO_SYNC_START_VALUE:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1800;
                return {index, 0x6};
            }
            return NodeObjectId();
        case OD_TPDO_MAPPING_ENTRY:
            if ((static_cast<quint16>(opt) > 0) && (static_cast<quint16>(opt) <= 4))
            {
                index = static_cast<quint16>(opt - 1) + 0x1A00;
                return {index, static_cast<quint8>(optMappingEntry)};
            }
            return NodeObjectId();

        case OD_PROGRAM_DATA_1:
            return {0x1F50, 0x1};

        case OD_PROGRAM_CONTROL_1:
            return {0x1F51, 0x1};

        case OD_BOOTLOADER_KEY:
            return {0x2050, 0x1};

        case OD_BOOTLOADER_CHECKSUM:
            return {0x2050, 0x2};

        case OD_BOOTLOADER_STATUS:
            return {0x2050, 0x3};
    }
    return NodeObjectId();
}

bool IndexDb::isQ1516(const NodeObjectId &objId, quint16 profileNumber)
{
    return ODIndexDb::isQ1516(objId.index(), objId.index(), profileNumber);
}
