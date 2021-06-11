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
#ifndef PROGRAMDOWNLOAD_H
#define PROGRAMDOWNLOAD_H

#include "parser/hexparser.h"
#include "model/deviceconfiguration.h"
#include "node.h"
#include "nodeodsubscriber.h"
#include <QWidget>

class ProgramDownload : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    ProgramDownload(Node *node);

    quint8 nodeId() const;
    Node *node();
    bool loadEds(const QString &fileName);
    void uploadEds();

    bool openHex(QString &fileName);

    bool update();

    QString fileNameEds() const;
    QString fileNameHex() const;

    enum Status
    {
        STATE_NONE,
        STATE_IN_PROGRESS,
        STATE_SUCCESSFUL,
        STATE_NOT_SUCCESSFUL
    };

    Status _status;
    Status status() const;

signals:
    void downloadState(QString state);
    void downloadFinished();

private:
    Node *_node;

    QList<NodeObjectId> _objectIdentityList;

    QString _fileNameEds;
    QString _fileNameHex;
    DeviceConfiguration *_deviceConfiguration;
    HexParser *hexFile;

    enum ProgramState
    {
        STATE_FREE,
        STATE_CHECK_1F55,
        STATE_CHECK_1F56,
        STATE_CHECK_SW_ID,
        STATE_STOP_PROGRAM,
        STATE_CLEAR_PROGRAM,
        STATE_DOWNLOAD_PROGRAM,
        STATE_FLASH_FINISHED,
        STATE_START,
        STATE_CHECK_IDENTITY,
        STATE_OK,
        STATE_NOT_OK
    };

    quint8 _iFsm;
    ProgramState _state;

    void saveEds();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // PROGRAMDOWNLOAD_H
