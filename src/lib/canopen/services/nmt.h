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

#ifndef NMT_H
#define NMT_H

#include "canopen_global.h"

#include "service.h"

class CANOPEN_EXPORT NMT : public Service
{
    Q_OBJECT
public:
    NMT(Node *node);

    uint32_t cobId() const;

    void sendPreop();
    void sendStart();
    void sendStop();
    void sendResetComm();
    void sendResetNode();
    void sendNodeGuarding();

    QString type() const override;

    void parseFrame(const QCanBusFrame &frame) override;

private:
    uint32_t _cobId;
    quint8 _nodeId;
    void sendNmt(quint8 cmd);
};

#endif  // NMT_H
