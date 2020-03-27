#ifndef NODEINDEX_H
#define NODEINDEX_H

#include "canopen_global.h"
#include <QObject>
#include "nodesubindex.h"

class NodeOd;

class CANOPEN_EXPORT NodeIndex
{
  public:
    NodeIndex(const quint16 &index);
    NodeIndex(const NodeIndex &other);
    ~NodeIndex();

    quint16 index() const;
    void setIndex(const quint16 &index);

    const QString &name() const;
    void setName(const QString &name);

    // =========== Object type ====================
    enum Object
    {
        NONE = 0x01,
        OBJECT_NULL = 0x00,
        OBJECT_DOMAIN = 0x02,
        DEFTYPE = 0x05,
        DEFSTRUCT = 0x06,
        VAR = 0x07,
        ARRAY = 0x08,
        RECORD = 0x09
    };

    quint8 objectType() const;
    void setObjectType(const quint8 &objectType);
    static QString objectTypeStr(const quint8 &objectType);

    const QMap<quint8, NodeSubIndex *> &subIndexes() const;

    NodeSubIndex *subIndex(quint8 subIndex) const;
    void addSubIndex(NodeSubIndex *subIndex);
    int subIndexesCount();
    bool subIndexExist(quint8 subIndex);

  private:
    NodeOd *_nodeOd;

    quint16 _index;
    QString _name;
    quint8 _objectType;

    QMap<quint8, NodeSubIndex *> _nodeSubIndexes;

};

#endif // NODEINDEX_H
