/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019 UniSwarm sebastien.caux@uniswarm.eu
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

#include <QDebug>

#include "sdo.h"
#include "canopenbus.h"

#define SDO_SCS(data)       ((data)[0] & 0xE0)  // E0 -> mask for css
#define SDO_E_MASK          0x2
#define SDO_E_NORMAL        0x0              // E: transfer type. See CIA 310 7.2.4.3.3 -> 0: normal transfer
#define SDO_E_EXPEDITED     0x1              // E: transfer type. See CIA 310 7.2.4.3.3 -> 1: expedited transfer

#define SDO_S_MASK          0x1
#define SDO_S               0x1              // S : size indicator. See CIA 310 7.2.4.3.3
#define SDO_C_MASK          0x1
#define SDO_C               0x1              // C: indicates whether there are still more segments to be downloaded.

#define SDO_N_MASK          0x03
#define SDO_N(data)         ((((data)[0]) >> 2) & 0x03) // Used for download/upload initiate. \
    // If valid it indicates the number of bytes in d that do not contain data
#define SDO_N_SEG(data)     ((((data)[0]) >> 1) & 0x07) // Used for download/upload segment. \
    // indicates the number of bytes in seg-data that do not contain segment data.

#define SDO_TOG_BIT(data)   ((data)[0] & 0x10)  // 0x10 -> mask for toggle bit
#define SDO_TOGGLE_MASK     1 << 4

// CCS : Client Command Specifier from Client to Server
#define SDO_CCS_CLIENT_DOWNLOAD_INITIATE        0x20    // ccs:1
#define SDO_CCS_CLIENT_DOWNLOAD_SEGMENT         0x00    // ccs:0
#define SDO_CCS_CLIENT_UPLOAD_INITIATE          0x40    // ccs:2 : initiate upload request
#define SDO_CCS_CLIENT_UPLOAD_SEGMENT           0x60    // ccs:3

// SCS : Server Command Specifier from Server to Client
#define SDO_SCS_SERVER_DOWNLOAD_INITIATE        0x60    // scs:3
#define SDO_SCS_SERVER_DOWNLOAD_SEGMENT         0x20    // scs:1
#define SDO_SCS_SERVER_UPLOAD_INITIATE          0x40    // scs:2
#define SDO_SCS_SERVER_UPLOAD_SEGMENT           0x00    // scs:0

/************************************************************************************/
/*                            BLOCK DOWNLOAD                                        */
// CCS : Client Command Specifier from Client to Server
#define SDO_CCS_CLIENT_BLOCK_DOWNLOAD_INITIATE      0xC0    // ccs:6
#define SDO_CCS_CLIENT_BLOCK_DOWNLOAD_END           0xC0    // ccs:6
#define SDO_CCS_CLIENT_BLOCK_DOWNLOAD_SUB_BLOCK     0xA0    // ccs:5
//cs: client subcommand for DOWNLOAD
#define SDO_CCS_CLIENT_BLOCK_DOWNLOAD_CS_MASK       0x1 // Maks for cs: client subcommand
#define SDO_CCS_CLIENT_BLOCK_DOWNLOAD_CS_INIT_REQ   0x0 // cs:0: initiate download request
#define SDO_CCS_CLIENT_BLOCK_DOWNLOAD_CS_END_REQ    0x1 // cs:1: end block download request

#define SDO_SCS_SERVER_BLOCK_DOWNLOAD_S             0x2 // s: size indicator

// SCS : Server Command Specifier from Server to Client
#define SDO_SCS_SERVER_BLOCK_DOWNLOAD_INITIATE      0xA0    // scs:5
#define SDO_SCS_SERVER_BLOCK_DOWNLOAD_RESP          0xA0    // scs:5
#define SDO_SCS_SERVER_BLOCK_DOWNLOAD_END           0xA0    // scs:5
// ss: server subcommand for DOWNLOAD
#define SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_MASK       0x3 // ss :0: initiate download response
#define SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_INIT_RESP  0x0 // ss :0: initiate download response
#define SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_RESP       0x2 // ss :2: block download response
#define SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_END_RESP   0x1 // ss :1: end block download response

/***********************************************************************************/
/*                            BLOCK UPLOAD                                         */
// CCS : Client Command Specifier from Client to Server
#define SDO_CCS_CLIENT_BLOCK_UPLOAD_INITIATE        0xA0// ccs:5
#define SDO_CCS_CLIENT_BLOCK_UPLOAD_END             0xA0// ccs:5
//cs: client subcommand for UPLOAD
#define SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_MASK         0x3 // Mask for cs: client subcommand
#define SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_INIT_REQ     0x0 // 0: initiate upload request
#define SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_END_REQ      0x1 // 1: end block upload request
#define SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_RESP         0x2 // 2: block upload response
#define SDO_CCS_CLIENT_BLOCK_UPLOAD_CS_START        0x3 // 3: start upload

// SCS : Server Command Specifier from Server to Client
#define SDO_SCS_SERVER_BLOCK_UPLOAD_INITIATE        0xC0    // scs:6
#define SDO_SCS_SERVER_BLOCK_UPLOAD_END             0xC0    // scs:6
// ss: server subcommand
#define SDO_SCS_SERVER_BLOCK_UPLOAD_SS_INIT_RESP    0x0 // 0: initiate upload response
#define SDO_SCS_SERVER_BLOCK_UPLOAD_SS_END_RESP     0x1 // 1: end block upload response
// s: size indicator
#define SDO_SCS_SERVER_BLOCK_UPLOAD_S_MASK          0x1 // s: size indicator
#define SDO_SCS_SERVER_BLOCK_UPLOAD_S               0x1 // s: size indicator

#define SDO_BLOCK_CRC_MASK   0x04  // Mask CRC support
#define SDO_BLOCK_CRC_CC     0x04  // cc: client CRC support
#define SDO_BLOCK_CRC_SC     0x04  // sc: server CRC support

#define SDO_INDEX(data) static_cast<uint16_t>(((data)[2]) << 8) + static_cast<uint8_t>((data)[1])
#define SDO_SUBINDEX(data) static_cast<uint8_t>((data)[3])
#define SDO_SG_SIZE       7         // size max by segment

#define CO_SDO_CS_ABORT                         0x80

SDO::SDO(CanOpenBus *bus)
    : Service (bus)
{

}

// OBSOLETE
void SDO::sendSdoReadReq(uint8_t nodeId, uint16_t index, uint8_t subindex)
{
    if (!_bus)
        return;
    if (!_bus->canDevice())
        return;

    QByteArray sdoReadReqPayload;
    uint8_t cmd = 0x40;
    QDataStream data(&sdoReadReqPayload, QIODevice::WriteOnly);
    data.setByteOrder(QDataStream::LittleEndian);
    data << cmd;
    data << index;
    data << subindex;
    data << static_cast<uint32_t>(0);

    QCanBusFrame frameNmt;
    frameNmt.setFrameId(0x600 + nodeId);
    frameNmt.setPayload(sdoReadReqPayload);
    _bus->canDevice()->writeFrame(frameNmt);
}

// OBSOLETE
void SDO::sendSdoWriteReq(uint8_t nodeId, uint16_t index, uint8_t subindex, const QVariant &value, uint8_t size)
{
    if (!_bus)
        return;
    if (!_bus->canDevice())
        return;

    if (size <= 4)
    {
        // expedited with size
        QByteArray sdoWriteReqPayload;

        uint8_t cmd = 0x20 + static_cast<uint8_t>((4 - size) << 2) + 0x03;
        QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
        data.setByteOrder(QDataStream::LittleEndian);
        data << cmd;
        data << index;
        data << subindex;
        data << value.toUInt();

        QCanBusFrame frameNmt;
        frameNmt.setFrameId(0x600 + nodeId);
        frameNmt.setPayload(sdoWriteReqPayload);
        _bus->canDevice()->writeFrame(frameNmt);
    }
}


void SDO::parseFrame(const QCanBusFrame &frame)
{
    co_sdo.nodeId =frame.frameId() & 0x07F;
    uint8_t scs = static_cast<uint8_t>SDO_SCS(frame.payload());
    qDebug() << QString::number(frame.frameId(), 16) << QString::number(scs, 16);

    switch (scs)
    {
    case SDO_SCS_SERVER_UPLOAD_INITIATE:
        sdoUploadInitiate(frame);
        break;

    case SDO_SCS_SERVER_UPLOAD_SEGMENT:
        sdoUploadSegment(frame);
        break;

    case SDO_SCS_SERVER_BLOCK_UPLOAD_INITIATE:
        sdoBlockUpload(frame);
        break;

    case SDO_SCS_SERVER_DOWNLOAD_INITIATE:
        sdoDownloadInitiate(frame);
        break;

    case SDO_SCS_SERVER_DOWNLOAD_SEGMENT:
        sdoDownloadSegment(frame);
        break;

    case SDO_SCS_SERVER_BLOCK_DOWNLOAD_INITIATE:
        sdoBlockDownload(frame);
        break;

    case CO_SDO_CS_ABORT:
        qDebug() << "ABORT received";
        co_sdo.state = CO_SDO_STATE_FREE;
        break;

    default:
        break;
    }
}

qint32 SDO::uploadData(uint8_t nodeId, Index &index, uint8_t subindex)
{
    bool error;
    uint8_t cmd = 0;
    co_sdo.index = &index;
    co_sdo.nodeId = nodeId;
    co_sdo.subIndex = subindex;

    if (co_sdo.index->subIndex(subindex)->dataType() != SubIndex::DDOMAIN)
    {
        cmd = SDO_CCS_CLIENT_UPLOAD_INITIATE;
        error = sendSdoRequest(co_sdo.nodeId, cmd, co_sdo.index->index(), co_sdo.subIndex);
        if (error)
        {
            // TODO ABORT management error with framesWritten() and errorOccurred()
        }
    }
    else
    {
         cmd = SDO_CCS_CLIENT_BLOCK_UPLOAD_INITIATE;
         // TODO
    }

    return 0;
}

qint32 SDO::sdoUploadInitiate(const QCanBusFrame &frame)
{
    uint8_t transferType = (static_cast<uint8_t>(frame.payload()[0] & SDO_E_MASK)) >> 1;
    uint8_t sizeIndicator = static_cast<uint8_t>(frame.payload()[0] & SDO_S_MASK);
    uint8_t cmd = 0;
    bool error;

    co_sdo.data.clear();
    co_sdo.index->subIndex(co_sdo.subIndex)->clearValue();
    if (transferType == SDO_E_EXPEDITED)
    {
        if (sizeIndicator == 0)  // data set size is not indicated
        {
        }
        else if (sizeIndicator == 1)  // data set size is indicated
        {
            co_sdo.stay = (4 - SDO_N(frame.payload()));
            co_sdo.index->subIndex(co_sdo.subIndex)->setValue( frame.payload().mid(4, static_cast<uint8_t>(co_sdo.stay)));

            emit dataObjetAvailable();
        }
        else
        {
        }

        co_sdo.state = CO_SDO_STATE_FREE;
    }
    else if (transferType == SDO_E_NORMAL)
    {
        if (sizeIndicator == 0)  // data set size is not indicated
        {
            // NOT USED
        }
        else if (sizeIndicator == 1)  // data set size is indicated
        {

        }
        else
        {

        }
        co_sdo.stay = static_cast<uint32_t>(frame.payload()[4]);
        cmd = SDO_CCS_CLIENT_UPLOAD_SEGMENT;
        co_sdo.toggle = 0;
        cmd |= (co_sdo.toggle << 4) & SDO_TOGGLE_MASK;

        error = sendSdoRequest(co_sdo.nodeId, cmd);
        if (error)
        {
            // TODO ABORT management error with framesWritten() and errorOccurred()
        }
        co_sdo.stay = CO_SDO_STATE_UPLOAD_SEGMENT;
    }
    else
    {
        // ERROR
        co_sdo.state = CO_SDO_STATE_FREE;
    }

    return 0;
}
qint32 SDO::sdoUploadSegment(const QCanBusFrame &frame)
{
    bool error;
    uint8_t cmd = 0;
    uint8_t toggle = SDO_TOG_BIT(frame.payload());
    uint8_t size = 0;

    if (toggle != (co_sdo.toggle & SDO_TOGGLE_MASK))
    {
        // ABORT        
        co_sdo.state = CO_SDO_STATE_FREE;
        return 1;
    }
    else
    {
        size = (7 - SDO_N(frame.payload()));
        co_sdo.data.append(frame.payload().mid(4, size));
         co_sdo.stay -= size;

        if ((frame.payload()[0] & SDO_C_MASK) == SDO_C)  // no more segments to be uploaded
        {
            co_sdo.index->subIndex(co_sdo.subIndex)->setValue(co_sdo.data);
            co_sdo.stay = CO_SDO_STATE_FREE;
            emit dataObjetAvailable();
        }
        else // more segments to be uploaded (C=0)
        {

            cmd = SDO_CCS_CLIENT_UPLOAD_SEGMENT;
            co_sdo.toggle = ~co_sdo.toggle;
            cmd |= (co_sdo.toggle << 4) & SDO_TOGGLE_MASK;
            qDebug() << "Send upload segment request : ccs :" << cmd;
            error = sendSdoRequest(co_sdo.nodeId, cmd);
            if (error)
            {
                // TODO ABORT management error with framesWritten() and errorOccurred()
            }
            co_sdo.stay = CO_SDO_STATE_UPLOAD_SEGMENT;
        }
    }
    return 0;
}

qint32 SDO::sdoBlockUpload(const QCanBusFrame &frame)
{
    Q_UNUSED(frame);
    return 0;
}

qint32 SDO::downloadData(uint8_t nodeId, Index &index, uint8_t subindex)
{
    bool error = true;
    uint8_t cmd = 0;

    co_sdo.index = &index;
    co_sdo.nodeId = nodeId;
    co_sdo.subIndex = subindex;

    if (co_sdo.index->subIndex(subindex)->dataType() == SubIndex::DDOMAIN)
    {
        co_sdo.stay = static_cast<uint32_t>(co_sdo.index->subIndex(subindex)->value().toByteArray().size());
        // block download
        cmd = SDO_CCS_CLIENT_BLOCK_DOWNLOAD_INITIATE;
        // No support crc
        if (co_sdo.stay < 0xFFFF)
        {
            cmd |= SDO_SCS_SERVER_BLOCK_DOWNLOAD_S;

            error = sendSdoRequest(co_sdo.nodeId, cmd, co_sdo.index->index(), co_sdo.subIndex, co_sdo.stay);
            if (error)
            {
                // TODO ABORT management error with framesWritten() and errorOccurred()
            }
        }
        else
        {  // Overload size so no indicate the size in frame S=0
            error = sendSdoRequest(co_sdo.nodeId, cmd, co_sdo.index->index(), co_sdo.subIndex, 0);
            if (error)
            {
                // TODO ABORT management error with framesWritten() and errorOccurred()
            }
        }
        co_sdo.seqno = 1;
        co_sdo.state = CO_SDO_STATE_FREE;
    }
    else
    {
         // expedited transfer
        if (co_sdo.index->subIndex(subindex)->length() <= 4)
        {
            cmd = SDO_CCS_CLIENT_DOWNLOAD_INITIATE;
            cmd |= SDO_E_EXPEDITED << 1;
            cmd |= SDO_S;
            cmd |= ((4 - co_sdo.index->subIndex(subindex)->length()) & SDO_N_MASK) << 2;
            sendSdoRequest(co_sdo.nodeId, cmd, co_sdo.index->index(), co_sdo.subIndex, co_sdo.index->subIndex(subindex)->value());
            co_sdo.state = CO_SDO_STATE_FREE;
        }
        else
        {   // normal transfer
            cmd = SDO_CCS_CLIENT_DOWNLOAD_INITIATE;
            cmd |= SDO_S;
            sendSdoRequest(co_sdo.nodeId, cmd, co_sdo.index->index(), co_sdo.subIndex, co_sdo.index->subIndex(subindex)->value());
            co_sdo.stay = static_cast<uint32_t>(co_sdo.index->subIndex(subindex)->length());
            co_sdo.state = CO_SDO_STATE_UPLOAD_SEGMENT;
        }
    }

    return 0;
}

qint32 SDO::sdoDownloadInitiate(const QCanBusFrame &frame)
{
    uint32_t seek = 0;
    QByteArray buffer;
    uint8_t cmd = 0;
    uint16_t index = 0;
    uint8_t subindex = 0;
    index = SDO_INDEX(frame.payload());
    subindex = SDO_SUBINDEX(frame.payload());

    if ((index != co_sdo.index->index()) || (subindex |= co_sdo.subIndex))
    {
        // ABORT
        co_sdo.state = CO_SDO_STATE_FREE;
        return 1;
    }
    else
    {
        if (co_sdo.state == CO_SDO_STATE_UPLOAD_SEGMENT)
        {
            cmd = SDO_CCS_CLIENT_UPLOAD_SEGMENT;
            co_sdo.toggle = 0;
            cmd |= (co_sdo.toggle << 4) & SDO_TOGGLE_MASK;
            cmd |= ((7 - co_sdo.index->subIndex(co_sdo.subIndex)->length()) & SDO_N_MASK) << 2;

            seek = (static_cast<uint32_t>(co_sdo.index->subIndex(subindex)->length()) - co_sdo.stay);
            buffer.clear();
            buffer = co_sdo.index->subIndex(co_sdo.subIndex)->value().toByteArray().mid(static_cast<int32_t>(seek), SDO_SG_SIZE);
            co_sdo.stay -= SDO_SG_SIZE;

            if (co_sdo.stay < SDO_SG_SIZE)
            {
                // no more segments to be downloaded
                cmd |= SDO_C;
                co_sdo.state = CO_SDO_STATE_FREE;
                emit dataObjetWritten();
            }
            co_sdo.state = CO_SDO_STATE_UPLOAD_SEGMENT;
            sendSdoRequest(co_sdo.nodeId, cmd, buffer);
        }
        else
        {
            emit dataObjetWritten();
        }
    }
    return 0;
}
qint32 SDO::sdoDownloadSegment(const QCanBusFrame &frame)
{
    uint32_t seek = 0;
    QByteArray buffer;
    uint8_t cmd = 0;

    if (co_sdo.state == CO_SDO_STATE_UPLOAD_SEGMENT)
    {
        uint8_t toggle = SDO_TOG_BIT(frame.payload());
        if (toggle != co_sdo.toggle)
        {
            // ABORT
            co_sdo.state = CO_SDO_STATE_FREE;
            return 1;
        }
        else
        {
            cmd = SDO_CCS_CLIENT_UPLOAD_SEGMENT;
            co_sdo.toggle = ~co_sdo.toggle;
            cmd |= (co_sdo.toggle << 4) & SDO_TOGGLE_MASK;
            cmd |= ((7 - co_sdo.index->subIndex(co_sdo.subIndex)->length()) & SDO_N_MASK) << 2;

            seek = (static_cast<uint32_t>(co_sdo.index->subIndex(co_sdo.subIndex)->length()) - co_sdo.stay);
            buffer.clear();
            buffer = co_sdo.index->subIndex(co_sdo.subIndex)->value().toByteArray().mid(static_cast<int32_t>(seek), SDO_SG_SIZE);
            co_sdo.stay -= SDO_SG_SIZE;

            if (co_sdo.stay < SDO_SG_SIZE)
            {
                // no more segments to be downloaded
                cmd |= SDO_C;
                co_sdo.state = CO_SDO_STATE_FREE;
                emit dataObjetWritten();
            }
            co_sdo.state = CO_SDO_STATE_UPLOAD_SEGMENT;
            sendSdoRequest(co_sdo.nodeId, cmd, buffer);
        }
    }

    return 0;
}

qint32 SDO::sdoBlockDownload(const QCanBusFrame &frame)
{
    uint16_t index = 0;
    uint8_t subindex = 0;
    uint8_t cmd = 0;
    uint8_t sizeSeg = 7;
    uint32_t seek = 0;
    QByteArray buffer;

    // ss : server subcommand
    uint8_t ss = static_cast<uint8_t> (frame.payload()[0] & SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_MASK);

    if ((ss == SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_INIT_RESP) && (co_sdo.state == CO_SDO_STATE_FREE))
    {
        // STATE SS = 0 -> Initiale download response
        index = SDO_INDEX(frame.payload());
        subindex = SDO_SUBINDEX(frame.payload());
        if (index != co_sdo.index->index() || subindex != co_sdo.subIndex)
        {
            qDebug() << "ERROR index, subindex not corresponding";
            co_sdo.state = CO_SDO_STATE_FREE;
            return 1;
        }
        else
        {
            co_sdo.blksize = static_cast<uint8_t>(frame.payload().data()[4]);
            co_sdo.state = CO_SDO_STATE_BLOCK_DOWNLOAD;
            co_sdo.seqno = 1;
        }
    }
    else if ((ss == SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_RESP) && (co_sdo.state == CO_SDO_STATE_BLOCK_DOWNLOAD))
    {
        co_sdo.blksize = static_cast<uint8_t>(frame.payload().data()[2]);
        uint8_t ackseq = static_cast<uint8_t>(frame.payload().data()[1]);
        if (ackseq == 0)
        {
            // ERROR sequence detection from server
            // Re-Send block
            qDebug() << "ERROR sequence detection from server, ackseq : " << ackseq;
            co_sdo.seqno = 1;
            co_sdo.state = CO_SDO_STATE_BLOCK_DOWNLOAD;
            return 1;
        }
        //blksize++;
    }
    else if (ss == SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_END_RESP)
    {
        co_sdo.state = CO_SDO_STATE_FREE;
    }

    if (co_sdo.state == CO_SDO_STATE_BLOCK_DOWNLOAD_END)
    {
        cmd = SDO_CCS_CLIENT_BLOCK_DOWNLOAD_END;
        cmd |= SDO_CCS_CLIENT_BLOCK_DOWNLOAD_CS_END_REQ;
        cmd |= (sizeSeg - co_sdo.stay) << 2;
        uint16_t crc = 0;        
        sendSdoRequest(co_sdo.nodeId, cmd, crc);

        co_sdo.state = CO_SDO_STATE_FREE;
        emit dataObjetWritten();
    }
    else
    {
        co_sdo.seqno = 1;
        co_sdo.state = CO_SDO_STATE_BLOCK_DOWNLOAD;
    }

    if (co_sdo.state == CO_SDO_STATE_BLOCK_DOWNLOAD)
    {
        while (co_sdo.seqno <= (co_sdo.blksize) && (co_sdo.stay > sizeSeg))
        {
            seek = (static_cast<uint32_t>(co_sdo.index->subIndex(co_sdo.subIndex)->value().toByteArray().size()) - co_sdo.stay);
            buffer.clear();
            buffer = co_sdo.index->subIndex(co_sdo.subIndex)->value().toByteArray().mid(static_cast<int32_t>(seek), sizeSeg);

            sendSdoRequest(co_sdo.nodeId, true, co_sdo.seqno, buffer);

            co_sdo.stay -= sizeSeg;
            co_sdo.seqno++;

            if (co_sdo.stay < sizeSeg)
            {
                seek = (static_cast<uint32_t>(co_sdo.index->subIndex(co_sdo.subIndex)->value().toByteArray().size()) - co_sdo.stay);
                buffer.clear();
                buffer = co_sdo.index->subIndex(co_sdo.subIndex)->value().toByteArray().mid(static_cast<int32_t>(seek), sizeSeg);
                co_sdo.seqno++;

                sendSdoRequest(co_sdo.nodeId, false, co_sdo.seqno, buffer);
                co_sdo.state = CO_SDO_STATE_BLOCK_DOWNLOAD_END;
            }
        }
    }
    return 0;
}

// SDO upload initiate
bool SDO::sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint16_t index, uint8_t subindex)
{
    if (!_bus)
    {
        return false;
    }
    if (!_bus->canDevice())
    {
        return false;
    }
    QByteArray sdoWriteReqPayload;
    QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
    data.setByteOrder(QDataStream::LittleEndian);
    data << cmd;
    data << index;
    data << subindex;

    QCanBusFrame frame;
    frame.setFrameId(0x600 + nodeId);
    frame.setPayload(sdoWriteReqPayload);
    return _bus->canDevice()->writeFrame(frame);
}
// SDO upload segment, SDO block upload initiate, SDO block upload ends
bool SDO::sendSdoRequest(uint8_t nodeId, uint8_t cmd)
{
    if (!_bus)
    {
        return false;
    }
    if (!_bus->canDevice())
    {
        return false;
    }
    QByteArray sdoWriteReqPayload;
    QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
    data.setByteOrder(QDataStream::LittleEndian);
    data << cmd;

    QCanBusFrame frame;
    frame.setFrameId(0x600 + nodeId);
    frame.setPayload(sdoWriteReqPayload);
    return _bus->canDevice()->writeFrame(frame);
}

// SDO download initiate
bool SDO::sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint16_t index, uint8_t subindex, const QVariant &data)
{
    if (!_bus)
    {
        return false;
    }
    if (!_bus->canDevice())
    {
        return false;
    }
    QByteArray sdoWriteReqPayload;
    QDataStream request(&sdoWriteReqPayload, QIODevice::WriteOnly);
    request.setByteOrder(QDataStream::LittleEndian);
    request << cmd;
    request << index;
    request << subindex;
    request << data.toUInt();

    QCanBusFrame frame;
    frame.setFrameId(0x600 + nodeId);
    frame.setPayload(sdoWriteReqPayload);
    return _bus->canDevice()->writeFrame(frame);
}

// SDO download segment
bool SDO::sendSdoRequest(uint8_t nodeId, uint8_t cmd, const QByteArray &value)
{
    if (!_bus)
    {
        return false;
    }
    if (!_bus->canDevice())
    {
        return false;
    }
    QByteArray sdoWriteReqPayload;
    QDataStream request(&sdoWriteReqPayload, QIODevice::WriteOnly);
    request.setByteOrder(QDataStream::LittleEndian);
    request << cmd;
    request << value.toUInt();

    QCanBusFrame frame;
    frame.setFrameId(0x600 + nodeId);
    frame.setPayload(sdoWriteReqPayload);
    return _bus->canDevice()->writeFrame(frame);
}

// SDO block download end
bool SDO::sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint16_t &crc)
{
    if (!_bus)
    {
        return false;
    }
    if (!_bus->canDevice())
    {
        return false;
    }
    QByteArray sdoWriteReqPayload;
    QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
    data.setByteOrder(QDataStream::LittleEndian);
    data << cmd;
    data << crc;

    QCanBusFrame frame;
    frame.setFrameId(0x600 + nodeId);
    frame.setPayload(sdoWriteReqPayload);
    return _bus->canDevice()->writeFrame(frame);
}

// SDO block upload initiate
bool SDO::sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint16_t index, uint8_t subindex, uint8_t blksize, uint8_t pst)
{
    Q_UNUSED(nodeId);
    Q_UNUSED(cmd);
    Q_UNUSED(index);
    Q_UNUSED(subindex);
    Q_UNUSED(blksize);
    Q_UNUSED(pst);
    return true;
}
// SDO block upload sub-block
bool SDO::sendSdoRequest(uint8_t nodeId, uint8_t cmd, uint8_t &ackseq, uint8_t blksize)
{
    Q_UNUSED(nodeId);
    Q_UNUSED(cmd);
    Q_UNUSED(ackseq);
    Q_UNUSED(blksize);
    return true;
}

// SDO block download sub-block
bool SDO::sendSdoRequest(uint8_t nodeId, bool moreSegments, uint8_t seqno, const QVariant &segData)
{
    QByteArray sdoWriteReqPayload;
    QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
    QCanBusFrame frame;
    data.setByteOrder(QDataStream::LittleEndian);

    if (moreSegments == false)
    {
        seqno |= 0x80;
        data << static_cast<uint8_t>(seqno);
    }
    else
    {
        data << static_cast<uint8_t>(seqno);
    }
    data << segData.toInt();

    frame.setFrameId(0x600 + nodeId);
    frame.setPayload(sdoWriteReqPayload);

    return _bus->canDevice()->writeFrame(frame);
}

