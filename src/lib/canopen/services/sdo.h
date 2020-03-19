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


class CANOPEN_EXPORT SDO : public Service
{
    Q_OBJECT
public:
    SDO(CanOpenBus *bus);

    virtual void parseFrame(const QCanBusFrame &frame);

//protected:
    void sendSdoReadReq(uint8_t nodeId, uint16_t index, uint8_t subindex);                                          // OBSOLETE
    void sendSdoWriteReq(uint8_t nodeId, uint16_t index, uint8_t subindex, const QVariant &value, uint8_t size);    // OBSOLETE

    qint32 uploadData(uint8_t nodeId, Index &index, uint8_t subindex);
    qint32 downloadData(uint8_t nodeId, Index &index, uint8_t subindex);

    typedef enum
    {
        CO_SDO_STATE_FREE,
        CO_SDO_STATE_UPLOAD_SEGMENT,
        CO_SDO_STATE_DOWNLOAD_SEGMENT,
        CO_SDO_STATE_BLOCK_DOWNLOAD,
        CO_SDO_STATE_BLOCK_DOWNLOAD_END_SUB,
        CO_SDO_STATE_BLOCK_DOWNLOAD_END
    } CO_SDO_STATE;


    typedef struct
    {
        Index *index;
        CO_SDO_STATE state;
        uint8_t nodeId;
        uint8_t subIndex;
        QByteArray data;
        uint32_t stay;  // nombre octet restant ou taille de l'objet
        uint8_t toggle;

        // For SDO Block
        uint8_t blksize;  // Number of segments per block with 0 < blksize < 128.
        uint8_t moreBlockSegments;  // indicates whether there are still more segments to be downloaded
        uint8_t seqno;  // sequence number of segment
    } co_sdo_t;

    co_sdo_t co_sdo;

    //uint32_t blksize = 0;

  signals:
    void dataObjetAvailable();
    void dataObjetWritten();

  private:

    qint32 sdoUploadInitiate(const QCanBusFrame &frame);
    qint32 sdoUploadSegment(const QCanBusFrame &frame);
    qint32 sdoDownloadInitiate(const QCanBusFrame &frame);
    qint32 sdoDownloadSegment(const QCanBusFrame &frame);
    qint32 sdoBlockDownload(const QCanBusFrame &frame);
    qint32 sdoBlockUpload(const QCanBusFrame &frame);

    bool sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint16_t index, uint8_t subindex);                         // SDO upload initiate
    bool sendSdoRequest(uint8_t nodeId, uint8_t cmd);                                                           // SDO upload segment, SDO block upload initiate, SDO block upload ends
    //bool sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint16_t index, uint8_t subindex, uint32_t size);          //
    bool sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint16_t index, uint8_t subindex, const QVariant &data);   // SDO download initiate, SDO block download initiate
    bool sendSdoRequest(uint8_t nodeId, uint8_t cmd, const QByteArray &value);                                    // SDO download segment

    bool sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint16_t index, uint8_t subindex, uint8_t blksize, uint8_t pst);   // SDO block upload initiate
    bool sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint8_t &ackseq, uint8_t blksize);                                 // SDO block upload sub-block

    bool sendSdoRequest(uint8_t nodeId, bool moreSegments, uint8_t seqno, const QVariant &segData);             // SDO block download sub-block
    bool sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint16_t &crc);                                            // SDO block download end
};

#endif // SDO_H
