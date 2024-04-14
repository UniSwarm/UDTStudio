/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include <QObject>

class Node;
class NodeObjectId;
class UfwModel;
class UfwParser;
class UfwUpdate;

class CANOPEN_EXPORT Bootloader : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    Bootloader(Node *node);
    ~Bootloader() override;

    Node *node() const;

    enum Status
    {
        STATUS_ERROR_OPEN_FILE = -1,
        STATUS_ERROR_NO_FILE = -2,
        STATUS_ERROR_ERROR_PARSER = -3,
        STATUS_ERROR_FILE_NOT_CORRESPONDING = -4,
        STATUS_ERROR_UPDATE_FAILED = -5,
        STATUS_UPDATE_ALREADY_IN_PROGRESS = -6,
        STATUS_MEMORY_ALREADY_WRITTEN = -7,

        STATUS_FILE_ANALYZED_OK = 1,
        STATUS_UPDATE_SUCCESSFUL = 2,
        STATUS_CHECK_FILE_AND_DEVICE = 3,
        STATUS_DEVICE_STOP_IN_PROGRESS = 4,
        STATUS_DEVICE_CLEAR_IN_PROGRESS = 5,
        STATUS_DEVICE_WRITING_OTP_IN_PROGRESS = 6,
        STATUS_DEVICE_UPDATE_IN_PROGRESS = 7,
        STATUS_CHECKING_UPDATE = 8,
    };
    Status status() const;
    QString statusStr(Status status) const;
    quint32 error() const;
    double updateProgress() const;

    bool openUfw(const QString &fileName);

    void startUpdate();

    void setOtpInformation(uint32_t address, const QString &date, uint16_t device, uint32_t serialNumber, const QString &version);
    void startOtpUpload();

    // Info UFW
    uint32_t deviceType();
    QString versionSoftware();
    QString buildDate();

public slots:
    void stopProgram();
    void startProgram();
    void resetProgram();
    void clearProgram();
    void sendKey();

signals:
    void statusEvent();

private slots:
    void readStatusProgram();
    void readStatusBootloader();
    void processEndUpload(bool ok);

private:
    Node *_node;

    Status _status;
    void setStatus(Status status);

    quint32 _error;

    enum StatusProgram
    {
        NONE,
        PROGRAM_STOPPED,
        PROGRAM_STARTED,
        NO_PROGRAM
    };
    StatusProgram _statusProgram;

    NodeObjectId _bootloaderKeyObjectId;
    NodeObjectId _bootloaderChecksumObjectId;
    NodeObjectId _bootloaderStatusObjectId;
    NodeObjectId _programControlObjectId;
    NodeObjectId _programDataObjectId;

    UfwModel *_ufwModel;
    UfwUpdate *_ufwUpdate;

    enum BootloaderState
    {
        STATE_FREE,
        STATE_CHECK_MODE,
        STATE_STOP_PROGRAM,
        STATE_CLEAR_PROGRAM,
        STATE_UPDATE_PROGRAM,
        STATE_UPLOADED_PROGRAM_FINISHED,
        STATE_UPLOAD_OTP_START,
        STATE_UPLOAD_OTP_IN_PROGRESS,
        STATE_UPLOAD_OTP_FINISHED,
        STATE_CHECK,
        STATE_OK,
        STATE_NOT_OK
    };
    BootloaderState _state;

    enum BootloaderMode
    {
        MODE_NONE = 0,
        MODE_OTP = 1
    };
    BootloaderMode _mode;

    void process();
    void updateProgram();
    void updateStartProgram();
    void updateFinishedProgram();

    QByteArray _progOtp;
    uint16_t _deviceOtp;
    void sendOtpUploadStart();
    void uploadOtpData();

    QByteArray capString(const QString &str, int size);

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif  // BOOTLOADER_H
