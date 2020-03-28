#include "nodeindex.h"
#include "nodesubindex.h"

NodeIndex::NodeIndex(const quint16 &index)
{
    _index = index;
    _objectType = VAR;
}

NodeIndex::NodeIndex(const NodeIndex &other)
    : QObject ()
{
    _index = other.index();
    _objectType = other.objectType();
    _name = other.name();

    foreach (NodeSubIndex *subIndex, other._nodeSubIndexes)
    {
        _nodeSubIndexes.insert(subIndex->subIndex(), new NodeSubIndex(*subIndex));
    }
}

NodeIndex::~NodeIndex()
{
}

quint16 NodeIndex::index() const
{
    return _index;
}

void NodeIndex::setIndex(const quint16 &index)
{
    _index = index;
}

const QString &NodeIndex::name() const
{
    return _name;
}

void NodeIndex::setName(const QString &name)
{
    _name = name;
}

/**
 * @brief object type getter
 * @return 8bits object type code
 */
NodeIndex::ObjectType NodeIndex::objectType() const
{
    return _objectType;
}

/**
 * @brief object type setter
 * @param 8 bits objects type code
 */
void NodeIndex::setObjectType(const ObjectType &objectType)
{
    _objectType = objectType;
}

/**
 * @brief converts an object type code to his corresponding string
 * @param 8 bits object type code
 * @return object type string
 */
QString NodeIndex::objectTypeStr(const ObjectType &objectType)
{
    switch (objectType)
    {
    case NONE:
        return QString("NONE");
    case OBJECT_NULL:
        return QString("NULL");
    case OBJECT_DOMAIN:
        return QString("DOMAIN");
    case DEFTYPE:
        return QString("DEFTYPE");
    case DEFSTRUCT:
        return QString("DEFSTRUCT");
    case VAR:
        return QString("VAR");
    case ARRAY:
        return QString("ARRAY");
    case RECORD:
        return QString("RECORD");
    }
    return QString();
}

/**
 * @brief max sub-index setter
 * @param max sub-index
 */
const QMap<uint8_t, NodeSubIndex *> &NodeIndex::subIndexes() const
{
    return _nodeSubIndexes;
}

/**
 * @brief return the subIndex with the number subIndex
 * @param subi-index number
 * @return a sub-index
 */
NodeSubIndex *NodeIndex::subIndex(uint8_t subIndex) const
{
    if (_nodeSubIndexes.contains(subIndex))
    {
        return _nodeSubIndexes.value(subIndex);
    }

    return nullptr;
}

/**
 * @brief inserts a new sub-index, if the sub-index already exists,
 * replaces it by the new sub-index
 * @param subIndex
 */
void NodeIndex::addSubIndex(NodeSubIndex *subIndex)
{
    _nodeSubIndexes.insert(subIndex->subIndex(), subIndex);
    subIndex->_nodeIndex = this;
}

/**
 * @brief returns the number of sub-indexes.
 * @return nummber of sub-indexes
 */
int NodeIndex::subIndexesCount()
{
    return _nodeSubIndexes.count();
}

/**
 * @brief returns true if the map contains a sub-index with the number subIndex;
 *  otherwise returns false
 * @param subIndex
 * @return true if the map contains a sub-index with the number subIndex,
 * otherwise returns false
 */
bool NodeIndex::subIndexExist(uint8_t subIndex)
{
    return _nodeSubIndexes.contains(subIndex);
}
