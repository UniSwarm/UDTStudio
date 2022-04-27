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

#ifndef SDO_H
#define SDO_H

#include "canopen_global.h"

#include "service.h"

#include <QQueue>
#include <QTimer>

#include "nodeindex.h"

class CANOPEN_EXPORT SDO : public Service
{
    Q_OBJECT
public:
    SDO(Node *node);
    ~SDO() override;

    // Settings
    int maxErrorAttempt() const;
    void setMaxErrorAttempt(int maxErrorAttempt);

    int blockDownloadIntervalMs() const;
    void setBlockDownloadIntervalMs(int blockDownloadIntervalMs);

    int timeoutMs() const;
    void setTimeoutMs(int timeoutMs);

    quint32 cobIdClientToServer() const;
    quint32 cobIdServerToClient() const;

    bool hasRequestPending() const;

    bool uploadData(quint16 index, quint8 subindex, QMetaType::Type dataType);
    bool downloadData(quint16 index, quint8 subindex, const QVariant &data);

    enum Status
    {
        SDO_STATE_FREE,
        SDO_STATE_NOT_FREE
    };
    Status status() const;

    // ================= sdo abort codes ====================
    enum SDOAbortCodes : quint32
    {
        CO_SDO_ABORT_CODE_BIT_NOT_ALTERNATED = 0x05030000,  // Toggle bit not alternated
        CO_SDO_ABORT_CODE_TIMED_OUT = 0x05040000,           // SDO protocol timed out
        CO_SDO_ABORT_CODE_CMD_NOT_VALID = 0x05040001,       // Client/server command specifier not valid or unknown
        CO_SDO_ABORT_CODE_INVALID_BLOCK_SIZE = 0x05040002,  // Invalid block size (block mode only)
        CO_SDO_ABORT_CODE_INVALID_SEQ_NUMBER = 0x05040003,  // Invalid sequence number (block mode only)
        CO_SDO_ABORT_CODE_CRC_ERROR = 0x05040004,           // CRC error (block mode only)
        CO_SDO_ABORT_CODE_OUT_OF_MEMORY = 0x05040005,       // Out of memory
        CO_SDO_ABORT_CODE_UNSUPPORTED_ACCESS = 0x06010000,  // Unsupported access to an object

        // access errors
        CO_SDO_ABORT_CODE_WRITE_ONLY = 0x06010001,  // Attempt to read a write only object
        CO_SDO_ABORT_CODE_READ_ONLY = 0x06010002,   // Attempt to write a read only object

        // no object erros
        CO_SDO_ABORT_CODE_NO_OBJECT = 0x06020000,    // Object does not exist in the object dictionary
        CO_SDO_ABORT_CODE_NO_SUBINDEX = 0x06090011,  // Sub-index does not exist

        // PDO errors
        CO_SDO_ABORT_CODE_CANNOT_MAP_PDO = 0x06040041,     // Object cannot be mapped to the PDO
        CO_SDO_ABORT_CODE_EXCEED_PDO_LENGTH = 0x06040042,  // The number and length of the objects to be mapped would exceed PDO length

        CO_SDO_ABORT_CODE_PARAM_IMCOMPATIBILITY = 0x06040043,  // General parameter incompatibility reason
        CO_SDO_ABORT_CODE_ITRN_IMCOMPATIBILITY = 0x06040047,   // General internal incompatibility in the device
        CO_SDO_ABORT_CODE_FAILED_HARDWARE_ERR = 0x06060000,    // Access failed due to an hardware error

        // data type length errors
        CO_SDO_ABORT_CODE_LENGTH_DOESNT_MATCH = 0x06070010,  // Data type does not match, length of service parameter does not match
        CO_SDO_ABORT_CODE_LENGTH_TOO_HIGH = 0x06070012,      // Data type does not match, length of service parameter too high
        CO_SDO_ABORT_CODE_LENGTH_TOO_LOW = 0x06070013,       // Data type does not match, length of service parameter too low

        // data value errors
        CO_SDO_ABORT_CODE_INVALID_VALUE = 0x06090030,       // Invalid value for parameter (download only)
        CO_SDO_ABORT_CODE_VALUE_TOO_HIGH = 0x06090031,      // Value of parameter written too high (download only)
        CO_SDO_ABORT_CODE_VALUE_TOO_LOW = 0x06090032,       // Value of parameter written too low (download only)
        CO_SDO_ABORT_CODE_MAX_VALUE_LESS_MIN = 0x06090036,  // Maximum value is less than minimum value

        CO_SDO_ABORT_CODE_RESRC_NOT_AVAILABLE = 0x060A0023,  // Resource not available: SDO connection
        CO_SDO_ABORT_CODE_GENERAL_ERROR = 0x08000000,        // General error

        // transferred or stored data erros
        CO_SDO_ABORT_CODE_CANNOT_TRANSFERRED_1 = 0x08000020,  // Data cannot be transferred or stored to the application
        CO_SDO_ABORT_CODE_CANNOT_TRANSFERRED_2 = 0x08000021,  // Data cannot be transferred or stored to the application because of local control
        CO_SDO_ABORT_CODE_CANNOT_TRANSFERRED_3 = 0x08000022,  // Data cannot be transferred or stored to the application because of the present device state

        CO_SDO_ABORT_CODE_NO_OBJECT_DICO = 0x08000023,  // Object dictionary dynamic generation fails or no object dictionary is present
        // object dictionary is generated from file and generation fails because of an file error)
        CO_SDO_ABORT_CODE_NO_DATA_AVAILABLE = 0x08000024  // No data available
    };
    QString sdoAbort(SDOAbortCodes error) const;

private:
    quint32 _cobIdClientToServer;
    quint32 _cobIdServerToClient;
    quint8 _nodeId;

    void processingFrameFromClient(const QCanBusFrame &frame);
    void processingFrameFromServer(const QCanBusFrame &frame);

    enum RequestState
    {
        STATE_FREE,
        STATE_UPLOAD,
        STATE_UPLOAD_SEGMENT,
        STATE_DOWNLOAD,
        STATE_DOWNLOAD_SEGMENT,
        STATE_BLOCK_DOWNLOAD,
        STATE_BLOCK_DOWNLOAD_END_SUB,
        STATE_BLOCK_DOWNLOAD_END,
        STATE_BLOCK_UPLOAD,
        STATE_BLOCK_UPLOAD_END_SUB,
        STATE_BLOCK_UPLOAD_END
    };

    struct RequestSdo
    {
        RequestState state;
        quint16 index;
        quint8 subIndex;
        QByteArray dataByte;
        QMetaType::Type dataType;
        QVariant data;
        quint32 stay;
        quint32 size;
        quint8 toggle;

        // For SDO Block
        QByteArray dataByteBySegment;
        quint8 blksize;            // Number of segments per block with 0 < blksize < 128.
        quint8 moreBlockSegments;  // indicates whether there are still more segments to be downloaded
        quint8 seqno;              // sequence number of segment
        quint8 ackseq;             // sequence number of segment
        bool error;
        quint8 attemptCount;
    };

    RequestSdo *_requestCurrent;
    QQueue<RequestSdo *> _requestQueue;
    Status _status;

    bool uploadDispatcher();
    bool downloadDispatcher();

    void sendErrorSdoToDevice(SDOAbortCodes error);
    void setErrorToObject(SDOAbortCodes error);
    void endRequest();
    void nextRequest();

    QTimer *_timeoutTimer;
    void timeout();

    QTimer *_subBlockDownloadTimer;

    quint16 indexFromFrame(const QCanBusFrame &frame);
    quint8 subIndexFromFrame(const QCanBusFrame &frame);
    bool sdoUploadInitiate(const QCanBusFrame &frame);
    bool sdoUploadSegment(const QCanBusFrame &frame);
    bool sdoDownloadInitiate(const QCanBusFrame &frame);
    bool sdoDownloadSegment(const QCanBusFrame &frame);
    bool sdoBlockDownload(const QCanBusFrame &frame);
    void sdoBlockDownloadSubBlock();
    bool sdoBlockDownloadEnd();
    bool sdoBlockUpload(const QCanBusFrame &frame);
    bool sdoBlockUploadSubBlock(const QCanBusFrame &frame);

    bool sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex);                              // SDO upload initiate
    bool sendSdoRequest(quint8 cmd);                                                              // SDO upload segment, SDO block upload initiate, SDO block upload ends
    bool sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex, const QByteArray &data);      // SDO download initiate, SDO block download initiate
    bool sendSdoRequest(quint8 cmd, const QByteArray &data);                                      // SDO download segment
    bool sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex, quint8 blksize, quint8 pst);  // SDO block upload initiate
    bool sendSdoRequest(quint8 cmd, quint8 &ackseq, quint8 blksize);                              // SDO block upload sub-block
    bool sendSdoRequest(bool moreSegments, quint8 seqno, const QByteArray &segData);              // SDO block download sub-block
    bool sendSdoRequest(quint8 cmd, quint16 &crc);                                                // SDO block download end
    bool sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex, quint32 error);               // SDO abort transfer
    quint8 calculateBlockSize(quint32 size);

    QVariant arrangeDataUpload(QByteArray, QMetaType::Type type);
    void arrangeDataDownload(QDataStream &request, const QVariant &data);

    // SDO settings
    int _maxErrorAttempt;
    int _blockDownloadIntervalMs;
    int _timeoutMs;

    // Service interface
    QString type() const override;
    void reset() override;
    void parseFrame(const QCanBusFrame &frame) override;
};

#endif  // SDO_H
