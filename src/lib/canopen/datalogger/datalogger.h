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

#ifndef DATALOGGER_H
#define DATALOGGER_H

#include "canopen_global.h"

#include "nodeodsubscriber.h"

#include "dldata.h"
#include <QMap>

class CANOPEN_EXPORT DataLogger : public QObject, public NodeOdSubscriber
{
    Q_OBJECT
public:
    DataLogger(QObject *parent = nullptr);
    virtual ~DataLogger();

    bool isStarted() const;

    void addData(const NodeObjectId &objId);
    void addData(const QList<NodeObjectId> &objIds);
    void removeData(const NodeObjectId &objId);
    void removeAllData();
    QList<DLData *> &dataList();
    DLData *data(int index) const;
    DLData *data(const NodeObjectId &objId) const;

    qreal min() const;
    qreal max() const;
    void range(qreal &min, qreal &max) const;

    QDateTime firstDateTime() const;
    QDateTime lastDateTime() const;

    void addDataValue(DLData *dlData, const QVariant &value, const QDateTime &dateTime);

signals:
    void dataChanged(int id);
    void dataAboutToBeAdded(int id);
    void dataAdded();
    void dataAboutToBeRemoved(int id);
    void dataRemoved();

    void startChanged(bool);

public slots:
    void start(int ms);
    void stop();
    void clear();

protected slots:
    void readData();

protected:
    QMap<quint64, DLData *> _dataMap;
    QList<DLData *> _dataList;
    QTimer _timer;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif  // DATALOGGER_H
