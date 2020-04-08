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

#ifndef PDO_H
#define PDO_H

#include "canopen_global.h"

#include "nodeobjectid.h"
#include "nodeod.h"
#include "nodeodsubscriber.h"
#include "service.h"

class RPDO;
class TPDO;

class CANOPEN_EXPORT PDO : public Service, public NodeOdSubscriber
{
    Q_OBJECT
public:
    PDO(Node *node, quint8 number);

    virtual QString type() const = 0;
    virtual void parseFrame(const QCanBusFrame &frame) = 0;
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) = 0;

protected:
    enum StateMapping
    {
        STATE_FREE,
        STATE_DEACTIVATE,
        STATE_DISABLE,
        STATE_MODIFY,
        STATE_ENABLE,
        STATE_ACTIVATE,
    };
    StateMapping state;

    quint8 _number;
    quint32 _cobId;
    quint8 _nodeId;
    NodeOd *_nodeOd;
    quint16 _objectMappingId;
    quint16 _objectCommId;

    QList<NodeObjectId> _objectMapped;
    bool createListObjectMapped();
    QList<NodeObjectId> listObjectMapped();

    quint8 _numberObjectCurrent;
    QList<NodeObjectId> _objectMap;
    void mapObjectList(QList<NodeObjectId> objectList);
    QList<NodeObjectId> currentMappedObjectList() const;
    void applyMapping();

    bool sendData(const QByteArray data);
    void arrangeData(QByteArray &request, const QVariant &data);
};

#endif // PDO_H
