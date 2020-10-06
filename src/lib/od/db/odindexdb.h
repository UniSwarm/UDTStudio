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

#ifndef ODINDEXDB_H
#define ODINDEXDB_H

#include "od_global.h"

#include "../canopen/nodeobjectid.h"

class OD_EXPORT OdIndexDb
{
public:
    enum OdObject
    {
        OD_RPDO_MAPPING,
        OD_RPDO_PARAM_COBID,
        OD_RPDO_PARAM_TRANSMISSIONTYPE,
        OD_RPDO_PARAM2222,

        OD_TPDO_PARAM_COBID,
        OD_TPDO_PARAM_TRANSMISSIONTYPE,
        OD_TPDO_PARAM2222,
        OD_TPDO_PARAM
    };

    static NodeObjectId getObjectId(OdObject object, uint opt = 0, uint opt2 = 0);

     // FUTURE
//    static QString name(const NodeObjectId &nodeObjectId);
//    static QVariant min(const NodeObjectId &nodeObjectId);
//    static QVariant max(const NodeObjectId &nodeObjectId);
//    static QVariant defaultValue(const NodeObjectId &nodeObjectId);
//    static QString unit(const NodeObjectId &nodeObjectId);
//    static QString description(const NodeObjectId &nodeObjectId);
};

#endif // ODINDEXDB_H
