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

#ifndef UPDATEPROCESS_H
#define UPDATEPROCESS_H

#include "canopenbus.h"

#include "bootloader/parser/hexparser.h"
#include "bootloader/utility/hexmerger.h"
#include "bootloader/parser/ufwparser.h"

#include <QObject>

class UpdateProcess : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    UpdateProcess(quint8 bus, quint8 speed, quint8 nodeid, QString binary);

    bool connectDevice();

    Node *node() const;

    int openUni(const QString &file);

    bool update();

    enum Status
    {
        STATE_NONE,
        STATE_PROGRAM_STOPPED,
        STATE_PROGRAM_STARTED,
        STATE_NO_PROGRAM,
        STATE_UPDATE_IN_PROGRESS,
        STATE_SUCCESSFUL,
        STATE_NOT_SUCCESSFUL
    };

    Status status() const;

public slots:
  void stopProgram();
  void startProgram();
  void resetProgram();
  void clearProgram();
  void updateProgram();

signals:
    void nodeConnected(bool connected);
    void finished(bool finished);

private slots:
    void nodeDetected(int nodeId);
    void sendSyncOne();
    void nodeDetectedTimeout();


private:
    Status _status;

    CanOpenBus *_bus;
    Node *_node;

    quint8 _busId;
    quint8 _speed;
    quint8 _nodeId;
    QString _binary;

    QList<NodeObjectId> _objectIdentityList;
    NodeObjectId _bootloaderObjectId;
    NodeObjectId _programObjectId;
    NodeObjectId _programControlObjectId;
    NodeObjectId _manufacturerSoftwareVersionObjectId;
    NodeObjectId _firmwareBuildDateObjectId;

    UfwParser *_uniBinary;

    QTimer *_nodeDetectedTimer;
    QTimer *_timer;

    enum ProgramState
    {
        STATE_FREE,
        STATE_CHECK_MODE,
        STATE_STOP_PROGRAM,
        STATE_CLEAR_PROGRAM,
        STATE_UPDATE_PROGRAM,
        STATE_FLASH_FINISHED,
        STATE_CHECK,
        STATE_OK,
        STATE_NOT_OK
    };

    ProgramState _state;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // UPDATEPROCESS_H
