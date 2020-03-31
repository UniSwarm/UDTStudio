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

#ifndef SDO_H
#define SDO_H

#include <QQueue>
#include <QTimer>
#include "canopen_global.h"

#include "service.h"
#include "nodeindex.h"

class CANOPEN_EXPORT SDO : public Service
{
    Q_OBJECT
public:
    SDO(Node *node);

    QString type() const override;

    void parseFrame(const QCanBusFrame &frame) override;

    quint32 cobIdClientToServer();
    quint32 cobIdServerToClient();

    qint32 uploadData(quint16 index, quint8 subindex);
    qint32 downloadData(quint16 index, quint8 subindex, const QByteArray &data);

    enum Status
    {
        SDO_STATE_FREE,
        SDO_STATE_NOT_FREE
    };

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

    Status status() const;

  signals:
    void sdoFree();
    void dataObjetAvailable(NodeIndex *nodeIndex);
    void dataObjetWritten();

private:
    quint32 _cobIdClientToServer;
    quint32 _cobIdServerToClient;
    quint8 _nodeId;

    struct RequestSdo
    {
        RequestState state;
        quint16 index;
        quint8 subIndex;
        QByteArray data;
        quint32 stay;
        quint8 toggle;

        // For SDO Block
        quint8 blksize;             // Number of segments per block with 0 < blksize < 128.
        quint8 moreBlockSegments;   // indicates whether there are still more segments to be downloaded
        quint8 seqno;               // sequence number of segment
    };

    RequestSdo *_request;
    QQueue<RequestSdo *> _requestQueue;
    Status _state;

    qint32 uploadDispatcher();
    qint32 downloadDispatcher();

    void errorManagement();
    void requestFinished();
    void nextRequest();

    int _time;
    QTimer *_timer;
    void timeout();

    qint32 sdoUploadInitiate(const QCanBusFrame &frame);
    qint32 sdoUploadSegment(const QCanBusFrame &frame);
    qint32 sdoDownloadInitiate(const QCanBusFrame &frame);
    qint32 sdoDownloadSegment(const QCanBusFrame &frame);
    qint32 sdoBlockDownload(const QCanBusFrame &frame);
    qint32 sdoBlockUpload(const QCanBusFrame &frame);

    bool sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex);                            // SDO upload initiate
    bool sendSdoRequest(quint8 cmd);                                                            // SDO upload segment, SDO block upload initiate, SDO block upload ends
    bool sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex, const QVariant &data);      // SDO download initiate, SDO block download initiate
    bool sendSdoRequest(quint8 cmd, const QByteArray &value);                                   // SDO download segment
    bool sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex, quint8 blksize, quint8 pst);// SDO block upload initiate
    bool sendSdoRequest(quint8 cmd, quint8 &ackseq, quint8 blksize);                            // SDO block upload sub-block
    bool sendSdoRequest(bool moreSegments, quint8 seqno, const QByteArray &segData);            // SDO block download sub-block
    bool sendSdoRequest(quint8 cmd, quint16 &crc);                                              // SDO block download end

    enum CCS : uint8_t  // CCS : Client Command Specifier from Client to Server
    {
        SDO_CCS_CLIENT_DOWNLOAD_INITIATE = 0x20,// ccs:1
        SDO_CCS_CLIENT_DOWNLOAD_SEGMENT = 0x00, // ccs:0
        SDO_CCS_CLIENT_UPLOAD_INITIATE = 0x40,  // ccs:2 : initiate upload request
        SDO_CCS_CLIENT_UPLOAD_SEGMENT = 0x60,   // ccs:3
        SDO_CCS_CLIENT_BLOCK_DOWNLOAD = 0xC0,   // ccs:6
        SDO_CCS_CLIENT_BLOCK_UPLOAD = 0xA0,     // ccs:5
    };

    enum SCS : uint8_t  // SCS : Server Command Specifier from Server to Client
    {
        SDO_SCS_SERVER_DOWNLOAD_INITIATE = 0x60,// scs:3
        SDO_SCS_SERVER_DOWNLOAD_SEGMENT = 0x20, // scs:1
        SDO_SCS_SERVER_UPLOAD_INITIATE = 0x40,  // scs:2
        SDO_SCS_SERVER_UPLOAD_SEGMENT = 0x00,   // scs:0
        SDO_SCS_SERVER_BLOCK_DOWNLOAD = 0xA0,   // scs:5
        SDO_SCS_SERVER_BLOCK_UPLOAD = 0xC0,     // scs:6
    };

    enum CS : uint8_t   // cs: client subcommand
    {
        SDO_CCS_CLIENT_BLOCK_DOWNLOAD_CS_MASK = 0x1,    // Maks for cs: client subcommand
        SDO_CCS_CLIENT_BLOCK_DOWNLOAD_CS_INIT_REQ = 0x0,// cs:0: initiate download request
        SDO_CCS_CLIENT_BLOCK_DOWNLOAD_CS_END_REQ = 0x1, // cs:1: end block download request
        SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_MASK = 0x3,      // Mask for cs: client subcommand
        SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_INIT_REQ = 0x0,  // 0: initiate upload request
        SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_END_REQ = 0x1,   // 1: end block upload request
        SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_RESP = 0x2,      // 2: block upload response
        SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_START = 0x3,     // 3: start upload
    };

    enum SS : uint8_t   // ss: server subcommand
    {
        SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_MASK = 0x3,        // ss :0: initiate download response
        SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_INIT_RESP = 0x0,   // ss :0: initiate download response
        SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_RESP = 0x2,        // ss :2: block download response
        SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_END_RESP = 0x1,    // ss :1: end block download response
        SDO_SCS_SERVER_BLOCK_UPLOAD_SS_INIT_RESP = 0x0,     // 0: initiate upload response
        SDO_SCS_SERVER_BLOCK_UPLOAD_SS_END_RESP = 0x1       // 1: end block upload response
    };

    enum Flag :  uint8_t
    {
        SDO_E_MASK = 0x2,
        SDO_E_NORMAL = 0x0,     // E: transfer type
        SDO_E_EXPEDITED = 0x1,  // E: transfer type
        SDO_S_MASK = 0x1,
        SDO_S = 0x1,            // S : size indicator
        SDO_C_MASK = 0x1,
        SDO_C = 0x1             // C: indicates whether there are still more segments to be downloaded.
    };

    enum FlagBlock :  uint8_t
    {
        BLOCK_SIZE = 0x2, // s: size indicator
        BLOCK_CRC = 0x04
    };
};

#endif // SDO_H
