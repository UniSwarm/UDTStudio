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

#include <QDebug>
#include <QDataStream>

#include "sdo.h"
#include "canopenbus.h"

#define SDO_SCS(data)       ((data).at(0) & 0xE0)  // E0 -> mask for css

#define SDO_N_MASK          0x03
#define SDO_N(data)         ((((data).at(0)) >> 2) & 0x03) // Used for download/upload initiate.
    // If valid it indicates the number of bytes in d that do not contain data
#define SDO_N_SEG(data)     ((((data).at(0)) >> 1) & 0x07) // Used for download/upload segment.
    // indicates the number of bytes in seg-data that do not contain segment data.

#define SDO_TOG_BIT(data)   ((data).at(0) & 0x10)  // 0x10 -> mask for toggle bit
#define SDO_TOGGLE_MASK     1 << 4

#define SDO_INDEX(data) static_cast<quint16>(((data).at(2)) << 8) + static_cast<quint8>((data).at(1))
#define SDO_SUBINDEX(data) static_cast<quint8>((data).at(3))
#define SDO_SG_SIZE       7         // size max by segment

#define CO_SDO_CS_ABORT                         0x80

SDO::SDO(Node *node)
    : Service (node)
{
    _nodeId = node->nodeId();
    _cobIdClientToServer = 0x600;
    _cobIdServerToClient = 0x580;
    _cobIds.append(_cobIdClientToServer + _nodeId);
    _cobIds.append(_cobIdServerToClient + _nodeId);

    _time = 1000;
    _timer = new QTimer(this);
    connect(_timer, &QTimer::timeout, this, &SDO::timeout);

    _state = SDO_STATE_FREE;
    _request = nullptr;
}

SDO::~SDO()
{
    delete _timer;
}

QString SDO::type() const
{
    return QLatin1String("SDO");
}

quint32 SDO::cobIdClientToServer()
{
    return _cobIdClientToServer;
}

quint32 SDO::cobIdServerToClient()
{
    return _cobIdServerToClient;
}

void SDO::parseFrame(const QCanBusFrame &frame)
{
    if (frame.frameId() == _cobIdClientToServer + _nodeId)
    {
        processingFrameFromClient(frame);
    }
    else if (frame.frameId() == _cobIdServerToClient + _nodeId)
    {
        processingFrameFromServer(frame);
    }
    else
    {
        return;
    }
}

void SDO::processingFrameFromClient(const QCanBusFrame &frame)
{
    Q_UNUSED(frame);
    return;
}

void SDO::processingFrameFromServer(const QCanBusFrame &frame)
{
    if (_request == nullptr)
    {
        return;
    }

    if (frame.payload().size() != 8)
    {
        errorManagement();
        return;
    }

    quint8 scs = static_cast<quint8>SDO_SCS(frame.payload());
    //qDebug() << QString::number(frame.frameId(), 16) << QString::number(scs, 16);

    _timer->stop();
    switch (scs)
    {
    case SCS::SDO_SCS_SERVER_UPLOAD_INITIATE:
        sdoUploadInitiate(frame);
        break;

    case SCS::SDO_SCS_SERVER_UPLOAD_SEGMENT:
        sdoUploadSegment(frame);
        break;

    case SCS::SDO_SCS_SERVER_BLOCK_UPLOAD:
        sdoBlockUpload(frame);
        break;

    case SCS::SDO_SCS_SERVER_DOWNLOAD_INITIATE:
        sdoDownloadInitiate(frame);
        break;

    case SCS::SDO_SCS_SERVER_DOWNLOAD_SEGMENT:
        sdoDownloadSegment(frame);
        break;

    case SCS::SDO_SCS_SERVER_BLOCK_DOWNLOAD:
        sdoBlockDownload(frame);
        break;

    case CO_SDO_CS_ABORT:
        qDebug() << "ABORT received : Index :" << QString::number(SDO_INDEX(frame.payload()), 16).toUpper()
                 << ", SubIndex :" << QString::number(SDO_SUBINDEX(frame.payload()), 16).toUpper()
                 << ", abort :" << QString::number(arrangeData(frame.payload().mid(4, 4), QMetaType::Type::UInt).toUInt(), 16).toUpper();
        requestFinished();
        break;

    default:
        break;
    }
}

SDO::Status SDO::status() const
{
    return _state;
}

qint32 SDO::uploadData(quint16 index, quint8 subindex, QMetaType::Type dataType)
{
    RequestSdo *request = new RequestSdo();
    request->index = index;
    request->subIndex = subindex;
    request->dataType = dataType;
    request->state = STATE_UPLOAD;

    _requestQueue.enqueue(request);
    nextRequest();
    return 0;
}

qint32 SDO::downloadData(quint16 index, quint8 subindex, const QVariant &data)
{
    RequestSdo *request = new RequestSdo();
    request->index = index;
    request->subIndex = subindex;
    request->dataByte = data.toByteArray();
    request->data = data;
    request->dataType =  QMetaType::Type(data.type());
    request->state = STATE_DOWNLOAD;

    if (request->dataType != QMetaType::Type::QByteArray)
    {
        request->size = static_cast<quint32>(QMetaType::sizeOf(QMetaType::Type(data.type())));
    }
    else
    {
        request->size = static_cast<quint32>(request->dataByte.size());
    }

    _requestQueue.enqueue(request);
    nextRequest();
    return 0;
}

qint32 SDO::uploadDispatcher()
{
    quint8 cmd = 0;

    if (QMetaType::sizeOf(_request->dataType) < 256)
    {
        cmd = CCS::SDO_CCS_CLIENT_UPLOAD_INITIATE;
        sendSdoRequest(cmd, _request->index, _request->subIndex);
        _request->state = STATE_UPLOAD;
    }
    else // Not Implemented
    {
        cmd = CCS::SDO_CCS_CLIENT_BLOCK_UPLOAD;
        _request->state = STATE_BLOCK_UPLOAD;
    }

    return 0;
}
qint32 SDO::sdoUploadInitiate(const QCanBusFrame &frame)
{
    quint8 transferType = (static_cast<quint8>(frame.payload().at(0) & Flag::SDO_E_MASK)) >> 1;
    quint8 sizeIndicator = static_cast<quint8>(frame.payload().at(0) & Flag::SDO_S_MASK);
    quint8 cmd = 0;

    if (transferType == Flag::SDO_E_EXPEDITED)
    {
        if (sizeIndicator == 1)  // data set size is indicated
        {
            _request->stay = (4 - SDO_N(frame.payload()));
            _request->dataByte.append(frame.payload().mid(4, static_cast<quint8>(_request->stay)));
        }
        else
        {
            // d contains unspecified number of bytes to be uploaded.
        }
        requestFinished();
    }
    else if (transferType == Flag::SDO_E_NORMAL)
    {
        if (sizeIndicator == 0)  // data set size is not indicated
        {
            // NOT USED -> ERROR d is reserved for further use.
        }

        _request->stay = static_cast<quint32>(frame.payload().at(4));
        cmd = CCS::SDO_CCS_CLIENT_UPLOAD_SEGMENT;
        _request->toggle = 0;
        cmd |= (_request->toggle << 4) & SDO_TOGGLE_MASK;

        sendSdoRequest(cmd);
        _request->state = STATE_UPLOAD_SEGMENT;
    }
    else
    {
        // ERROR
        _request->state = STATE_FREE;
        errorManagement();
    }

    return 0;
}

qint32 SDO::sdoUploadSegment(const QCanBusFrame &frame)
{
    quint8 cmd = 0;
    quint8 toggle = SDO_TOG_BIT(frame.payload());
    quint8 size = 0;

    if (_request->state != STATE_UPLOAD_SEGMENT)
    {
        // Wrong State
        return 0;
    }

    if (toggle != (_request->toggle & SDO_TOGGLE_MASK))
    {
        // ABORT        
        errorManagement();
        return 1;
    }
    else
    {
        size = (7 - SDO_N_SEG(frame.payload()));
        _request->dataByte.append(frame.payload().mid(1, size));
        _request->stay -= size;

        if ((frame.payload().at(0) & Flag::SDO_C_MASK) == Flag::SDO_C)  // no more segments to be uploaded
        {
            requestFinished();
        }
        else // more segments to be uploaded (C=0)
        {
            cmd = CCS::SDO_CCS_CLIENT_UPLOAD_SEGMENT;
            _request->toggle = ~_request->toggle;
            cmd |= (_request->toggle << 4) & SDO_TOGGLE_MASK;
            qDebug() << "Send upload segment request : ccs :" << cmd;

            sendSdoRequest(cmd);
            _request->stay = STATE_UPLOAD_SEGMENT;
        }
    }
    return 0;
}

qint32 SDO::sdoBlockUpload(const QCanBusFrame &frame)
{
    Q_UNUSED(frame);
    return 0;
}

qint32 SDO::downloadDispatcher()
{
    quint8 cmd = 0;

    if (_request->size >= 128)
    {
        // block download
        cmd = CCS::SDO_CCS_CLIENT_BLOCK_DOWNLOAD;
        if (_request->size < 0xFFFF)
        {
            cmd |= FlagBlock::BLOCK_SIZE;
            sendSdoRequest(cmd, _request->index, _request->subIndex, _request->size);
        }
        else
        {  // Overload size so no indicate the size in frame S=0
            sendSdoRequest(cmd, _request->index, _request->subIndex, 0);
        }
        _request->seqno = 1;
    }
    else
    {
        // expedited transfer
        if (_request->size <= 4)
        {
            cmd = CCS::SDO_CCS_CLIENT_DOWNLOAD_INITIATE;
            cmd |= SDO_E_EXPEDITED << 1;
            cmd |= Flag::SDO_S;
            cmd |= ((4 - _request->size) & SDO_N_MASK) << 2;
            for (quint8 i = static_cast<quint8>(_request->size - 1); i < 4; i++)
            {
                _request->dataByte.append(static_cast<quint8>(0));
            }
            sendSdoRequest(cmd, _request->index, _request->subIndex, _request->data);
            _request->state = STATE_DOWNLOAD;
        }
        else
        {   // normal transfer
            cmd = CCS::SDO_CCS_CLIENT_DOWNLOAD_INITIATE;
            cmd |= Flag::SDO_S;
            sendSdoRequest(cmd, _request->index, _request->subIndex, static_cast<quint32>(_request->size));
            _request->stay = _request->size;
            _request->state = STATE_DOWNLOAD_SEGMENT;
        }
    }

    return 0;
}

qint32 SDO::sdoDownloadInitiate(const QCanBusFrame &frame)
{
    quint32 seek = 0;
    QByteArray buffer;
    quint8 cmd = 0;
    quint16 index = 0;
    quint8 subindex = 0;
    index = SDO_INDEX(frame.payload());
    subindex = SDO_SUBINDEX(frame.payload());

    if ((index != _request->index) || (subindex != _request->subIndex))
    {
        // ABORT
        errorManagement();
        return 1;
    }
    else
    {
        if (_request->state == STATE_DOWNLOAD_SEGMENT)
        {
            cmd = CCS::SDO_CCS_CLIENT_DOWNLOAD_SEGMENT;
            _request->toggle = 0;
            cmd |= (_request->toggle << 4) & SDO_TOGGLE_MASK;
            cmd |= ((7 - _request->size) & SDO_N_MASK) << 2;

            seek = _request->size - _request->stay;
            buffer.clear();
            buffer = _request->dataByte.mid(static_cast<int32_t>(seek), SDO_SG_SIZE);
           _request->stay -= SDO_SG_SIZE;

            if (_request->stay < SDO_SG_SIZE) // no more segments to be downloaded
            {
                for (int i = (buffer.size() - 1 ); i < SDO_SG_SIZE; i++)
                {
                    buffer.append(static_cast<quint8>(0));
                }
                cmd |= SDO_C;
                requestFinished();
            }
            _request->state = STATE_DOWNLOAD_SEGMENT;
            sendSdoRequest(cmd, buffer);
        }
        else if (_request->state == STATE_DOWNLOAD)
        {
            requestFinished();
        }
    }
    return 0;
}

qint32 SDO::sdoDownloadSegment(const QCanBusFrame &frame)
{
    quint32 seek = 0;
    QByteArray buffer;
    quint8 cmd = 0;

    if (_request->state == STATE_DOWNLOAD_SEGMENT)
    {
        quint8 toggle = SDO_TOG_BIT(frame.payload());
        if (toggle != _request->toggle)
        {
            // ABORT
            errorManagement();
            return 1;
        }
        else
        {
            cmd = CCS::SDO_CCS_CLIENT_DOWNLOAD_SEGMENT;
            _request->toggle = ~_request->toggle;
            cmd |= (_request->toggle << 4) & SDO_TOGGLE_MASK;
            cmd |= ((7 - _request->size) & SDO_N_MASK) << 2;

            seek = _request->size - _request->stay;
            buffer.clear();
            buffer = _request->dataByte.mid(static_cast<int32_t>(seek), SDO_SG_SIZE);
            _request->stay -= SDO_SG_SIZE;

            if (_request->stay < SDO_SG_SIZE)
            {
                // no more segments to be downloaded
                for (int i = (buffer.size() - 1 ); i < SDO_SG_SIZE; i++)
                {
                    buffer.append(static_cast<quint8>(0));
                }
                cmd |= SDO_C;
                requestFinished();
            }
            _request->state = STATE_DOWNLOAD_SEGMENT;
            sendSdoRequest(cmd, buffer);
        }
    }

    return 0;
}

qint32 SDO::sdoBlockDownload(const QCanBusFrame &frame)
{
    quint16 index = 0;
    quint8 subindex = 0;
    quint8 cmd = 0;
    quint32 seek = 0;
    QByteArray buffer;

    quint8 ss = static_cast<quint8> (frame.payload().at(0) & SS::SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_MASK);

    if ((ss == SS::SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_INIT_RESP) && (_request->state == STATE_DOWNLOAD))
    {
        index = SDO_INDEX(frame.payload());
        subindex = SDO_SUBINDEX(frame.payload());
        if (index != _request->index || subindex != _request->subIndex)
        {
            qDebug() << "ERROR index, subindex not corresponding";
            errorManagement();
            return 1;
        }
        else
        {
            _request->blksize = static_cast<quint8>(frame.payload().at(4));
            _request->state = STATE_BLOCK_DOWNLOAD;
            _request->seqno = 1;
        }
    }
    else if ((ss == SS::SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_RESP) && (_request->state == STATE_BLOCK_DOWNLOAD))
    {
        _request->blksize = static_cast<quint8>(frame.payload().at(2));
        quint8 ackseq = static_cast<quint8>(frame.payload().at(1));
        if (ackseq == 0)
        {
            // ERROR sequence detection from server
            // Re-Send block
            qDebug() << "ERROR sequence detection from server, ackseq : " << ackseq;
            _request->seqno = 1;
            _request->state = STATE_BLOCK_DOWNLOAD;
            return 1;
        }
    }
    else if (ss == SS::SDO_SCS_SERVER_BLOCK_DOWNLOAD_SS_END_RESP)
    {
        requestFinished();
    }

    if (_request->state == STATE_BLOCK_DOWNLOAD_END)
    {
        cmd = CCS::SDO_CCS_CLIENT_BLOCK_DOWNLOAD;
        cmd |= CS::SDO_CCS_CLIENT_BLOCK_DOWNLOAD_CS_END_REQ;
        cmd |= (SDO_SG_SIZE - _request->stay) << 2;
        quint16 crc = 0;
        sendSdoRequest(cmd, crc);
    }
    else
    {
        _request->seqno = 1;
        _request->state = STATE_BLOCK_DOWNLOAD;
    }

    if (_request->state == STATE_BLOCK_DOWNLOAD)
    {
        while (_request->seqno <= (_request->blksize) && (_request->stay > SDO_SG_SIZE))
        {
            seek = _request->size - _request->stay;
            buffer.clear();
            buffer = _request->dataByte.mid(static_cast<int32_t>(seek), SDO_SG_SIZE);

            sendSdoRequest(true, _request->seqno, buffer);
            _request->stay -= SDO_SG_SIZE;
            _request->seqno++;
            qDebug() << "_request->stay" << _request->stay;
            if (_request->stay < SDO_SG_SIZE)
            {
                seek = _request->size - _request->stay;
                buffer.clear();
                buffer = _request->dataByte.mid(static_cast<int32_t>(seek), SDO_SG_SIZE);
                for (int i = (buffer.size() - 1 ); i < SDO_SG_SIZE; i++)
                {
                    buffer.append(static_cast<quint8>(0));
                }
                _request->seqno++;

                sendSdoRequest(false, _request->seqno, buffer);
                _request->state = STATE_BLOCK_DOWNLOAD_END;
            }
        }
    }
    return 0;
}
void SDO::errorManagement()
{
    uint32_t error = 0x08000000;
    sendSdoRequest(CO_SDO_CS_ABORT, _request->index,_request->subIndex, error);
    _state = SDO_STATE_FREE;
    _request->state = STATE_FREE;
    _timer->stop();
    nextRequest();
}

void SDO::requestFinished()
{
    if (_request->state == STATE_UPLOAD)
    {
         _node->nodeOd()->updateObjectFromDevice(_request->index, _request->subIndex, arrangeData(_request->dataByte, _request->dataType));
    }
    else if(_request->state == STATE_DOWNLOAD)
    {
        _node->nodeOd()->updateObjectFromDevice(_request->index, _request->subIndex, _request->data);
    }

    _state = SDO_STATE_FREE;
    _timer->stop();
    nextRequest();
}

void SDO::nextRequest()
{
    if (!_requestQueue.isEmpty())
    {
        if (_state == SDO_STATE_FREE)
        {
            _request = _requestQueue.dequeue();
            if (_request->state == STATE_UPLOAD)
            {
                _state = SDO_STATE_NOT_FREE;
                uploadDispatcher();
            }
            else if(_request->state == STATE_DOWNLOAD)
            {
                _state = SDO_STATE_NOT_FREE;
                downloadDispatcher();
            }
        }
    }
    else
    {
        _request = nullptr;
    }
}

void SDO::timeout()
{
    uint32_t error = 0x08000000;
    sendSdoRequest(CO_SDO_CS_ABORT, _request->index,_request->subIndex, error);
    _state = SDO_STATE_FREE;
    _request->state = STATE_FREE;
    _timer->stop();
    nextRequest();
}

// SDO upload initiate
bool SDO::sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex)
{
    QCanBusDevice *lcanDevice = canDevice();
    if (!lcanDevice)
    {
        return false;
    }

    QByteArray sdoWriteReqPayload;
    QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
    data.setByteOrder(QDataStream::LittleEndian);
    data << cmd;
    data << index;
    data << subindex;
    data << static_cast<quint32>(0);

    QCanBusFrame frame;
    frame.setFrameId(_cobIdClientToServer + _nodeId);
    frame.setPayload(sdoWriteReqPayload);

    _timer->start(_time);
    return lcanDevice->writeFrame(frame);
}
// SDO upload segment, SDO block upload initiate, SDO block upload ends
bool SDO::sendSdoRequest(quint8 cmd)
{
    QCanBusDevice *lcanDevice = canDevice();
    if (!lcanDevice)
    {
        return false;
    }

    QByteArray sdoWriteReqPayload;
    QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
    data.setByteOrder(QDataStream::LittleEndian);
    data << cmd;
    data << static_cast<quint32>(0);
    data << static_cast<quint8>(0);
    data << static_cast<quint16>(0);

    QCanBusFrame frame;
    frame.setFrameId(_cobIdClientToServer + _nodeId);
    frame.setPayload(sdoWriteReqPayload);

    _timer->start(_time);
    return lcanDevice->writeFrame(frame);
}

// SDO download initiate
bool SDO::sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex, const QVariant &data)
{
    QCanBusDevice *lcanDevice = canDevice();
    if (!lcanDevice)
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
    frame.setFrameId(_cobIdClientToServer + _nodeId);
    frame.setPayload(sdoWriteReqPayload);

    _timer->start(_time);
    return lcanDevice->writeFrame(frame);
}

// SDO download segment
bool SDO::sendSdoRequest(quint8 cmd, const QByteArray &value)
{
    QCanBusDevice *lcanDevice = canDevice();
    if (!lcanDevice)
    {
        return false;
    }

    QByteArray sdoWriteReqPayload;
    QDataStream request(&sdoWriteReqPayload, QIODevice::WriteOnly);
    request.setByteOrder(QDataStream::LittleEndian);
    request << cmd;
    request << value.toUInt();

    QCanBusFrame frame;
    frame.setFrameId(_cobIdClientToServer + _nodeId);
    frame.setPayload(sdoWriteReqPayload);
    _timer->start(_time);
    return lcanDevice->writeFrame(frame);
}

// SDO block download end
bool SDO::sendSdoRequest(quint8 cmd, quint16 &crc)
{
    QCanBusDevice *lcanDevice = canDevice();
    if (!lcanDevice)
    {
        return false;
    }

    QByteArray sdoWriteReqPayload;
    QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
    data.setByteOrder(QDataStream::LittleEndian);
    data << cmd;
    data << crc;
    data << static_cast<quint32>(0);
    data << static_cast<quint8>(0);

    QCanBusFrame frame;
    frame.setFrameId(_cobIdClientToServer + _nodeId);
    frame.setPayload(sdoWriteReqPayload);

    _timer->start(_time);
    return lcanDevice->writeFrame(frame);
}

// SDO block upload initiate
bool SDO::sendSdoRequest(quint8 cmd, quint16 index, quint8 subindex, quint8 blksize, quint8 pst)
{
    Q_UNUSED(cmd);
    Q_UNUSED(index);
    Q_UNUSED(subindex);
    Q_UNUSED(blksize);
    Q_UNUSED(pst);
    return true;
}

// SDO block upload sub-block
bool SDO::sendSdoRequest(quint8 cmd, quint8 &ackseq, quint8 blksize)
{
    Q_UNUSED(cmd);
    Q_UNUSED(ackseq);
    Q_UNUSED(blksize);
    return true;
}

// SDO block download sub-block
bool SDO::sendSdoRequest(bool moreSegments, quint8 seqno, const QByteArray &segData)
{
    QCanBusDevice *lcanDevice = canDevice();
    if (!lcanDevice)
    {
        return false;
    }

    QByteArray sdoWriteReqPayload;
    QDataStream data(&sdoWriteReqPayload, QIODevice::WriteOnly);
    QCanBusFrame frame;
    data.setByteOrder(QDataStream::LittleEndian);

    if (moreSegments == false)
    {
        seqno |= 0x80;
        data << static_cast<quint8>(seqno);
    }
    else
    {
        data << static_cast<quint8>(seqno);
    }
    //data << segData.toUInt();

    sdoWriteReqPayload.append(segData);
    frame.setFrameId(_cobIdClientToServer + _nodeId);
    frame.setPayload(sdoWriteReqPayload);

    return lcanDevice->writeFrame(frame);
}


QVariant SDO::arrangeData(QByteArray data, QMetaType::Type type)
{
    QDataStream dataStream(&data, QIODevice::ReadOnly);
    dataStream.setByteOrder(QDataStream::LittleEndian);

    switch (type)
    {
    case QMetaType::Int:
        int a;
        dataStream >> a;
        return QVariant(a);

    case QMetaType::UInt:
        unsigned int b;
        dataStream >> b;
        return QVariant(b);

    case QMetaType::LongLong:
        qint64 c;
        dataStream >> c;
        return QVariant(c);

    case QMetaType::ULongLong:
        quint64 d;
        dataStream >> d;
        return QVariant(d);

    case QMetaType::Double:
        double e;
        dataStream >> e;
        return QVariant(e);

        //        case QMetaType::Long:
        //        long f;
        //        return QVariant(f);

    case QMetaType::Short:
        short g;
        dataStream >> g;
        return QVariant(g);

    case QMetaType::Char:
        return QVariant(data);

        //    case QMetaType::ULong:
        //        unsigned long i;
        //        i = static_cast<unsigned long>(data.toUInt());
        //        return QVariant(i);

    case QMetaType::UShort:
        unsigned short j;
        dataStream >> j;
        return QVariant(j);

    case QMetaType::UChar:
        unsigned char k;
        k = static_cast<unsigned char>(data.toUInt());
        return QVariant(k);

    case QMetaType::Float:
        float l;
        dataStream >> l;
        return QVariant(l);

    case QMetaType::SChar:
        signed char m;
        m = static_cast<signed char>(data.toInt());
        return QVariant(m);

    case QMetaType::QString:
        return QVariant(QString(data));

    case QMetaType::QByteArray:
        return QVariant(data);

    default:
        break;
    }

    return QVariant();
}
