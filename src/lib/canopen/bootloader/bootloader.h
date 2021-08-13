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

#ifndef BOOTLOADER_H
#define BOOTLOADER_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"
#include "parser/ufwparser.h"

#include <QObject>

class Node;
class NodeObjectId;

class CANOPEN_EXPORT Bootloader : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    Bootloader(Node *node);

    quint8 busId() const;
    quint8 nodeId() const;
    Node *node() const;

    enum StatusProgram
    {
        NONE,
        PROGRAM_STOPPED,
        PROGRAM_STARTED,
        NO_PROGRAM,
        UPDATE_IN_PROGRESS,
        SUCCESSFUL,
        NOT_SUCCESSFUL
    };
    StatusProgram statusProgram();

    enum StatusFlash
    {
        STATUS_OK = 0x0,
        STATUS_IN_PROGRESS = 0x1,
        STATUS_VALID_PROGRAM_AVAILABLE = 0x2,
        STATUS_NO_VALID_PROGRAM_AVAILABLE = 0x4,
        STATUS_DATA_FORMAT_UNKNOWN = 0x8,
        STATUS_DATA_FORMAT_ERROR_OR_DATA_CRC_ERROR = 0x10,
        STATUS_FLASH_NOT_CLEARED_BEFORE_WRITE = 0x20,
        STATUS_FLASH_WRITE_ERROR = 0x40,
        STATUS_GENERAL_ADDRESS_ERROR = 0x80,
        STATUS_FLASH_SECURED = 0x100
    };
    StatusFlash statusFlash();

    bool openUfw(const QString &fileName);

public slots:
    void stopProgram();
    void startProgram();
    void resetProgram();
    void clearProgram();
    void updateProgram();
    void sendDevice();

signals:
    void statusProgramChanged(StatusProgram);
    void statusFlashChanged(StatusFlash);

private:
    Node *_node;

    StatusProgram _statusProgram;
    StatusFlash _statusFlash;

    NodeObjectId _bootloaderObjectId;
    NodeObjectId _programObjectId;
    NodeObjectId _programControlObjectId;
    NodeObjectId _flashStatusObjectId;

    UfwParser *_ufw;

    QTimer *_timer;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // BOOTLOADER_H
