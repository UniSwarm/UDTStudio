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

#include "odindexdb.h"


NodeObjectId OdIndexDb::getObjectId(OdIndexDb::OdObject object, uint opt, uint opt2)
{
    switch (object) {
    case OdIndexDb::OD_TPDO_PARAM2222:
        return {0x1001, static_cast<quint8>(opt)};
        break;
    case OdIndexDb::OD_RPDO_MAPPING:
        break;
    case OdIndexDb::OD_RPDO_PARAM_COBID:
        break;
    case OdIndexDb::OD_RPDO_PARAM_TRANSMISSIONTYPE:
        break;
    case OdIndexDb::OD_RPDO_PARAM2222:
        break;
    case OdIndexDb::OD_TPDO_PARAM_COBID:
        break;
    case OdIndexDb::OD_TPDO_PARAM_TRANSMISSIONTYPE:
        break;
    case OdIndexDb::OD_TPDO_PARAM:
        break;

    }
    return NodeObjectId();
}
