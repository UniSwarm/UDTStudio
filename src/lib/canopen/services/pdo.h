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
    PDO(Node *node);

    virtual QString type() const = 0;
    virtual void parseFrame(const QCanBusFrame &frame) = 0;
    void odNotify(const NodeObjectId &objId, const QVariant &value) = 0;

  protected:
    QList<TPDO *> _tpdos;
    QList<RPDO *> _rpdos;
};

#endif // PDO_H
