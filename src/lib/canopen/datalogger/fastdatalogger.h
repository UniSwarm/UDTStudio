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

#ifndef FASTDATALOGGER_H
#define FASTDATALOGGER_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"
#include <QObject>

#include "fastdataloggerconfig.h"
#include "node.h"

class CANOPEN_EXPORT FastDataLogger : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    FastDataLogger(Node *node);

    Node *node() const;

    FastDataLoggerConfig &config();
    const FastDataLoggerConfig &config() const;

    void start();
    void stop();

    void commitConfig();

    const QList<qreal> &valuesData1() const;
    const QList<qreal> &valuesData2() const;

    static uint32_t objIdToU32(const NodeObjectId &objId);
    static NodeObjectId u32ToObjId(uint32_t u32);

    enum SourceDataType
    {
        SourceDataInvalid,
        SourceDataU8,
        SourceDataI8,
        SourceDataU16,
        SourceDataI16,
        SourceDataU32,
        SourceDataI32,
        SourceDataFloat,
    };
    QList<qreal> byteArrayToReals(const QByteArray &byteArray, SourceDataType dataType);

    enum Status
    {
        StatusInvalidTrigger = -2,
        StatusInvalidData = -1,
        StatusOff = 0,
        StatusWaitingForTrigger = 1,
        StatusLogging = 2,
        StatusDataReady = 3,
    };
    Status status() const;
    QString statusStr() const;

signals:
    void dataAvailable();
    void statusChanged(FastDataLogger::Status status);

private:
    Node *_node;

    FastDataLoggerConfig _config;

    QList<qreal> _valuesData1;
    SourceDataType _dataType1;
    QList<qreal> _valuesData2;
    SourceDataType _dataType2;
    SourceDataType typeFromObjId(const NodeObjectId &data_objId);

    Status _status;
    void setStatus(Status status);

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif  // FASTDATALOGGER_H
