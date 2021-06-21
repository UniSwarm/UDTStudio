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
#include "programdownload.h"

#include "parser/edsparser.h"
#include <QFileInfo>
#include "writer/hexwriter.h"
#include "utility/hexmerger.h"

ProgramDownload::ProgramDownload(Node *node)
    : _node(node)
{
    _objectIdentityList = {{_node->busId(), _node->nodeId(), 0x1000, 0},
                           {_node->busId(), _node->nodeId(), 0x1018, 1},
                           {_node->busId(), _node->nodeId(), 0x1018, 2},
                           {_node->busId(), _node->nodeId(), 0x1018, 3},
                           {_node->busId(), _node->nodeId(), 0x1018, 4}};

    registerObjId({0x1F56, 1});
    registerObjId({0x1F50, 1});
    registerObjId({0x1021, 0});
    registerObjId({0x1000, 1});
    registerObjId({0x1018, 255});
    setNodeInterrest(node);

    _status = STATE_NONE;
    _state = STATE_FREE;
    _deviceConfiguration = nullptr;
}

quint8 ProgramDownload::nodeId() const
{
    return _node->nodeId();
}

Node *ProgramDownload::node()
{
    return _node;
}

bool ProgramDownload::loadEds(const QString &fileName)
{
    EdsParser parser;
    DeviceDescription *deviceDescription = parser.parse(fileName);
    _deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, _node->nodeId());

    if (_deviceConfiguration->indexExist(0x1F55))
    {
        _state = STATE_CHECK_1F55;
        _fileNameEds = fileName;
    }
    else
    {
        // Error status G
        _state = STATE_NOT_OK;
        return false;
    }
    return true;
}

void ProgramDownload::uploadEds()
{
    _node->readObject(0x1021, 0);
}

void ProgramDownload::saveEds()
{
    QString device(QString("0x%1").arg(QString::number(_node->nodeOd()->value(0x1000, 0).toUInt(), 16).toUpper()));
    QString vendorId(QString("0x%1").arg(QString::number(_node->nodeOd()->value(0x1018, 1).toUInt(), 16).toUpper()));
    QString productCode(QString("0x%1").arg(QString::number(_node->nodeOd()->value(0x1018, 2).toUInt(), 16).toUpper()));
    QString revisionNumber(QString("0x%1").arg(QString::number(_node->nodeOd()->value(0x1018, 3).toUInt(), 16).toUpper()));
    QString serialNumber(QString("0x%1").arg(QString::number(_node->nodeOd()->value(0x1018, 4).toUInt(), 16).toUpper()));

    QString name(device + "_" + vendorId + "_" + productCode + "_" + revisionNumber + "_" + serialNumber + ".eds");
    QFile file(name);
    if (!file.open(QFile::WriteOnly))
    {
        return;
    }
    file.write(_node->nodeOd()->value(0x1021, 0).toByteArray());
}

bool ProgramDownload::openHex(const QString &fileName)
{
    _fileNameHex = fileName;
    hexFile = new HexParser(_fileNameHex);
    hexFile->read();

//    HexWriter hexWriter;
//    hexWriter.write(hexFile->prog(), "/home/julien/Seafile/myLibrary/2_FW/4_UIO_fw/UIO8AD/build/test.hex");


    HexParser *hexBootFile = new HexParser("/home/julien/Seafile/myLibrary/2_FW/4_UIO_fw/UIO8AD/bootloader/build/uio8ad.hex");
    hexBootFile->read();

    //HexMerger *merger = new HexMerger();
    //merger->merge(hexFile->prog(), hexBootFile->prog(), 0x400, 0x6FFF);
    //QByteArray out = merger->prog();

    //HexWriter hexWriter;
    //hexWriter.write(out, "/home/julien/Seafile/myLibrary/2_FW/4_UIO_fw/UIO8AD/build/test.hex");

    // Save file
    QFile file("fileBeforeSend.hex");
    if (!file.open(QFile::WriteOnly))
    {
        return false;
    }
    else
    {
        file.write(hexFile->prog());
        file.close();
    }

    return true;
}

bool ProgramDownload::update()
{
    if ((!hexFile) || (!_deviceConfiguration))
    {
        return false;
    }
    _status = STATE_IN_PROGRESS;
    switch (_state)
    {
    case STATE_FREE:
        break;

    case STATE_CHECK_1F55:
        emit downloadState("Status : check before flashing");
        _node->readObject(0x1F56, 1, QMetaType::UInt);
        break;
    case STATE_CHECK_1F56:
        if (_deviceConfiguration->index(0x1F55)->subIndex(1)->value() != _node->nodeOd()->index(0x1F56)->subIndex(1)->value())
        {
            emit downloadState("Status : download programm is starting");
            hexFile->read();
            _node->updateFirmware(hexFile->prog());
        }
        else
        {
            _state = STATE_CHECK_IDENTITY;
            emit downloadState("Status : Program is already up to date");
        }
        break;

    case STATE_CHECK_SW_ID:
    case STATE_STOP_PROGRAM:
    case STATE_CLEAR_PROGRAM:
    case STATE_DOWNLOAD_PROGRAM:
        emit downloadState("Status : download programm finished");
        _node->readObject(0x1F56, 1, QMetaType::UInt);
        _iFsm = 0;
        break;

    case STATE_FLASH_FINISHED:
        emit downloadState("Status : check software identification");
        if (_deviceConfiguration->index(0x1F55)->subIndex(1)->value() != _node->nodeOd()->index(0x1F56)->subIndex(1)->value())
        {
            emit downloadState("Status : check identity");
            _node->readObject(_objectIdentityList.at(_iFsm));
        }
        else
        {
            _state = STATE_NOT_OK;
            emit downloadState("Status : Error, check software identification, not corresponding");
            update();
        }
        break;

    case STATE_START:
    case STATE_CHECK_IDENTITY:

        for (NodeObjectId object : _objectIdentityList)
        {
            quint32 valueDevice = _node->nodeOd()->index(object.index())->subIndex(object.subIndex())->value().toUInt();
            quint32 valueEds = _node->nodeOd()->index(object.index())->subIndex(object.subIndex())->value().toUInt();
            if (valueDevice != valueEds)
            {
                _state = STATE_NOT_OK;
                update();
                break;
            }
        }
        _state = STATE_OK;
        update();
        break;

    case STATE_OK:
        _state = STATE_FREE;
        emit downloadState("Status : Flashing successful");
        emit downloadFinished();
        _status = STATE_SUCCESSFUL;
        break;
    case STATE_NOT_OK:
        // Error status C, D, M, N, O
        _state = STATE_FREE;
        emit downloadState("Status : Error, check identity, not corresponding");
        emit downloadFinished();
        _status = STATE_NOT_SUCCESSFUL;
        break;
    }

    return true;
}

QString ProgramDownload::fileNameEds() const
{
    QFileInfo fileInfo(_fileNameEds);
    return fileInfo.fileName();
}

QString ProgramDownload::fileNameHex() const
{
    QFileInfo fileInfo(_fileNameHex);
    return fileInfo.fileName();
}

ProgramDownload::Status ProgramDownload::status() const
{
    return _status;
}

void ProgramDownload::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if ((flags != SDO::FlagsRequest::Error) && (_state != STATE_FREE))
    {

        if (_state == STATE_CHECK_1F55)
        {
            if ((objId.index() == 0x1F56) && objId.subIndex() == 1)
            {
                _state = STATE_CHECK_1F56;
                update();
            }
        }

        if (_state == STATE_CHECK_1F56)
        {
            if ((objId.index() == 0x1F50) && objId.subIndex() == 1)
            {
                _state = STATE_DOWNLOAD_PROGRAM;
                update();
            }
        }
        if (_state == STATE_DOWNLOAD_PROGRAM)
        {
            if ((objId.index() == 0x1F56) && objId.subIndex() == 1)
            {
                _state = STATE_FLASH_FINISHED;
                update();
            }
        }

        if (_state == STATE_FLASH_FINISHED)
        {
            _iFsm++;
            _state = STATE_FLASH_FINISHED;
            if (_iFsm == _objectIdentityList.size())
            {
                _iFsm = 0;
                _state = STATE_CHECK_IDENTITY;
            }
            update();
        }
    }

    if ((objId.index() == 0x1021) && (objId.subIndex() == 0) && (_state == STATE_FREE))
    {
        if (flags != SDO::FlagsRequest::Error)
        {
            saveEds();
        }
        else
        {
            emit downloadState("Status : Upload EDS not supported by device");
        }
    }
}
