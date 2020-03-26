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

#include "canopen_global.h"
#include "model/deviceconfiguration.h"

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

    qint32 uploadData(NodeIndex &index, quint8 subindex);
    qint32 downloadData(NodeIndex &index, quint8 subindex);

    enum Status
    {
        SDO_STATE_FREE,
        SDO_STATE_UPLOAD,
        SDO_STATE_UPLOAD_SEGMENT,
        SDO_STATE_DOWNLOAD,
        SDO_STATE_DOWNLOAD_SEGMENT,
        SDO_STATE_BLOCK_DOWNLOAD,
        SDO_STATE_BLOCK_DOWNLOAD_END_SUB,
        SDO_STATE_BLOCK_DOWNLOAD_END,
        SDO_STATE_BLOCK_UPLOAD,
        SDO_STATE_BLOCK_UPLOAD_END_SUB,
        SDO_STATE_BLOCK_UPLOAD_END
    };

    Status status() const;

  signals:
    void sdoFree();
    void dataObjetAvailable();
    void dataObjetWritten();

private:
    quint32 _cobIdClientToServer;
    quint32 _cobIdServerToClient;
    quint8 _nodeId;

    struct ProtocolSdo
    {
        NodeIndex *index;
        Status state;
        quint8 subIndex;
        QByteArray data;
        quint32 stay;
        quint8 toggle;

        // For SDO Block
        quint8 blksize;             // Number of segments per block with 0 < blksize < 128.
        quint8 moreBlockSegments;   // indicates whether there are still more segments to be downloaded
        quint8 seqno;               // sequence number of segment
    };

    ProtocolSdo _protocol;

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
};

#endif // SDO_H
