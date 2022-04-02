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

#ifndef NODEOD_H
#define NODEOD_H

#include "canopen_global.h"

#include <QObject>

#include <QMap>
#include <QMultiMap>

#include "nodeindex.h"
#include "nodeobjectid.h"
#include "services/sdo.h"

class Node;
class NodeOdSubscriber;

class CANOPEN_EXPORT NodeOd : public QObject
{
    Q_OBJECT
public:
    NodeOd(Node *node);
    virtual ~NodeOd();

    Node *node() const;

    void resetAllObjects();

    // eds
    bool loadEds(const QString &fileName);
    const QString &edsFileName() const;
    const QMap<QString, QString> &edsFileInfos() const;

    bool exportDcf(const QString &fileName) const;
    bool exportConf(const QString &fileName) const;

    // index
    const QMap<quint16, NodeIndex *> &indexes() const;
    NodeIndex *index(quint16 index) const;
    void addIndex(NodeIndex *index);
    int indexCount() const;
    bool indexExist(quint16 index) const;

    // subindex
    NodeSubIndex *subIndex(quint16 index, quint8 subIndex) const;
    NodeSubIndex *subIndex(const NodeObjectId &id) const;
    bool subIndexExist(quint16 index, quint8 subIndex) const;
    int subIndexCount() const;

    // error
    void setErrorObject(quint16 index, quint8 subIndex, quint32 error) const;
    quint32 errorObject(const NodeObjectId &id) const;
    quint32 errorObject(quint16 index, quint8 subIndex = 0x00) const;

    // value
    QVariant value(const NodeObjectId &id) const;
    QVariant value(quint16 index, quint8 subIndex = 0x00) const;

    // dataType
    static QMetaType::Type dataTypeCiaToQt(NodeSubIndex::DataType type);
    QMetaType::Type dataType(const NodeObjectId &id) const;
    QMetaType::Type dataType(quint16 index, quint8 subIndex = 0x00) const;

    // modifications
    QDateTime lastModification(const NodeObjectId &id) const;
    QDateTime lastModification(quint16 index, quint8 subIndex = 0x00) const;

    // subscribe, notifier service
    enum FlagsRequest
    {
        Read = 0x01,
        Write = 0x02,
        Error = 0x04,
        Sdo = 0x08,
        Pdo = 0x10
    };
    void subscribe(NodeOdSubscriber *object, quint16 notifyIndex, quint8 notifySubIndex);
    void unsubscribe(NodeOdSubscriber *object);
    void unsubscribe(NodeOdSubscriber *object, quint16 notifyIndex, quint8 notifySubIndex);
    void updateObjectFromDevice(quint16 index, quint8 subindex, const QVariant &value, NodeOd::FlagsRequest flags, const QDateTime &modificationDate = QDateTime());

    // store / restore
    void store(uint8_t subIndex, uint32_t signature);
    void restore(uint8_t subIndex, uint32_t signature);

    // default objects
    void createMandatoryObjects();
    void createBootloaderObjects();

private:
    Node *_node;
    QMap<quint16, NodeIndex *> _nodeIndexes;
    QString _edsFileName;
    QMap<QString, QString> _edsFileInfos;

    struct Subscriber
    {
        NodeOdSubscriber *object;
        quint16 notifyIndex;
        quint8 notifySubIndex;
    };
    QMultiMap<quint32, Subscriber> _subscribers;
    void notifySubscribers(quint32 key, quint16 notifyIndex, quint8 notifySubIndex, NodeOd::FlagsRequest flags);
};

#endif  // NODEOD_H
