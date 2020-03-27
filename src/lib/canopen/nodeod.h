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

#ifndef NODEOD_H
#define NODEOD_H

#include "canopen_global.h"
#include "model/index.h"
#include <QMap>
#include "nodeindex.h"

class Node;

class CANOPEN_EXPORT NodeOd : public QObject
{
    Q_OBJECT
public:
    NodeOd(Node *node);
    virtual ~NodeOd();

    const QMap<quint16, NodeIndex *> &indexes() const;
    NodeIndex *index(quint16 index) const;
    void addIndex(NodeIndex *index);

    int indexCount() const;
    bool indexExist(quint16 key) const;


    bool loadEds(const QString &fileName);


private:
    Node *_node;
    QMap<quint16, NodeIndex *> _nodeIndexes;
    QString _fileName;

};

#endif // NODEOD_H
