#include "nodesubindex.h"

#include "nodeindex.h"
#include "nodeod.h"

NodeSubIndex::NodeSubIndex(const quint8 subIndex)
{
    _nodeIndex = nullptr;

    _subIndex = subIndex;
    _accessType = NOACESS;
    _dataType = NONE;
}

NodeSubIndex::NodeSubIndex(const NodeSubIndex &other)
{
    _nodeIndex = nullptr;

    _subIndex = other.subIndex();
    _name = other.name();
    _accessType = other.accessType();

    _value = other._value;
    _defaultValue = other._defaultValue;
    _dataType = other._dataType;

    _lowLimit = other._lowLimit;
    _highLimit = other._highLimit;

    _lastModification = other._lastModification;
}

/**
 * @brief destructor
 */
NodeSubIndex::~NodeSubIndex()
{
}

quint8 NodeSubIndex::busId() const
{
    if (_nodeIndex)
    {
        return _nodeIndex->busId();
    }
    return 0xFF;
}

quint8 NodeSubIndex::nodeId() const
{
    if (_nodeIndex)
    {
        return _nodeIndex->nodeId();
    }
    return 0xFF;
}

Node *NodeSubIndex::node() const
{
    if (_nodeIndex)
    {
        return _nodeIndex->node();
    }
    return nullptr;
}

NodeOd *NodeSubIndex::nodeOd() const
{
    if (_nodeIndex)
    {
        return _nodeIndex->nodeOd();
    }
    return nullptr;
}

quint16 NodeSubIndex::index() const
{
    if (_nodeIndex)
    {
        return _nodeIndex->index();
    }
    return 0xFFFF;
}

NodeIndex *NodeSubIndex::nodeIndex() const
{
    return _nodeIndex;
}

NodeObjectId NodeSubIndex::objectId() const
{
    return NodeObjectId(busId(), nodeId(), index(), _subIndex, metaType());
}

/**
 * @brief sub-index number getter
 * @return 8 bits sub-index number
 */
uint8_t NodeSubIndex::subIndex() const
{
    return _subIndex;
}

/**
 * @brief sub-index number setter
 * @param 8 bits sub-index number
 */
void NodeSubIndex::setSubIndex(const uint8_t &subIndex)
{
    _subIndex = subIndex;
}

/**
 * @brief name getter
 * @return sub-index name
 */
const QString &NodeSubIndex::name() const
{
    return _name;
}

/**
 * @brief name setter
 * @param new sub-index string name
 */
void NodeSubIndex::setName(const QString &name)
{
    _name = name;
}

/**
 * @brief access type getter
 * @return 8 bits access type code
 */
NodeSubIndex::AccessType NodeSubIndex::accessType() const
{
    return _accessType;
}

/**
 * @brief access type setter
 * @param 8 bits access type code
 */
void NodeSubIndex::setAccessType(const AccessType &accessType)
{
    _accessType = accessType;
}

/**
 * @brief Acces readable
 * @return true is the sub index is readable
 */
bool NodeSubIndex::isReadable() const
{
    return (_accessType & READ);
}

/**
 * @brief Acces writable
 * @return true is the sub index is writable
 */
bool NodeSubIndex::isWritable() const
{
    return (_accessType & WRITE);
}

/**
 * @brief Acces by TPDO
 * @return true is the sub index can be accessed by TPDO
 */
bool NodeSubIndex::hasTPDOAccess() const
{
    return (_accessType & TPDO);
}

/**
 * @brief Acces by RPDO
 * @return true is the sub index can be accessed by RPDO
 */
bool NodeSubIndex::hasRPDOAccess() const
{
    return (_accessType & RPDO);
}

QString NodeSubIndex::accessString() const
{
    QString acces;

    if (_accessType & READ)
    {
        acces += "R";
    }
    if (_accessType & WRITE)
    {
        acces += "W";
    }
    if (_accessType & TPDO)
    {
        acces += " TPDO";
    }
    if (_accessType & RPDO)
    {
        acces += " RPDO";
    }

    return acces;
}

/**
 * @brief _value getter
 * @return return sub-index value
 */
const QVariant &NodeSubIndex::value() const
{
    return _value;
}

/**
 * @brief _value setter
 * @param new sub-index value
 */
void NodeSubIndex::setValue(const QVariant &value)
{
    _value.setValue(value);
    _lastModification = QDateTime().currentDateTime();
}

/**
 * @brief _value setter
 * @param new sub-index value
 */
void NodeSubIndex::clearValue()
{
    _value.clear();
    _lastModification = QDateTime().currentDateTime();
}

/**
 * @brief _error getter
 * @return return sub-index value
 */
const quint32 &NodeSubIndex::error() const
{
    return _error;
}

/**
 * @brief _error setter
 * @param new sub-index value
 */
void NodeSubIndex::setError(const quint32 &error)
{
    _error = error;
}

/**
 * @brief _error clear
 * @param new sub-index value
 */
void NodeSubIndex::clearError()
{
    _error = 0;
}

/**
 * @brief _dataType getter
 * @return 16 bits sub-index data type code
 */
NodeSubIndex::DataType NodeSubIndex::dataType() const
{
    return _dataType;
}

/**
 * @brief _dataType setter
 * @param new 16 bits data type code
 */
void NodeSubIndex::setDataType(const DataType &dataType)
{
    _dataType = dataType;
}

/**
 * @brief converts an data type code to his corresponding string
 * @param 16 bits data type code
 * @return data type string
 */
QString NodeSubIndex::dataTypeStr(const DataType &dataType)
{
    switch (dataType)
    {
    case NONE:
        return QString("NONE");
    case BOOLEAN:
        return QString("BOOLEAN");
    case INTEGER8:
        return QString("INT8");
    case INTEGER16:
        return QString("INT16");
    case INTEGER32:
        return QString("INT32");
    case UNSIGNED8:
        return QString("UINT8");
    case UNSIGNED16:
        return QString("UINT16");
    case UNSIGNED32:
        return QString("UINT32");
    case REAL32:
        return QString("REAL32");
    case VISIBLE_STRING:
        return QString("VSTRING");
    case OCTET_STRING:
        return QString("OSTRING");
    case UNICODE_STRING:
        return QString("USTRING");
    case TIME_OF_DAY:
        return QString("DAYTIME");
    case TIME_DIFFERENCE:
        return QString("TIMEDIFF");
    case DDOMAIN:
        return QString("DOMAIN");
    case INTEGER24:
        return QString("INT24");
    case REAL64:
        return QString("REAL64");
    case INTEGER40:
        return QString("INT40");
    case INTEGER48:
        return QString("INT48");
    case INTEGER56:
        return QString("INT56");
    case INTEGER64:
        return QString("INT64");
    case UNSIGNED24:
        return QString("UINT24");
    case UNSIGNED40:
        return QString("UINT40");
    case UNSIGNED48:
        return QString("UINT48");
    case UNSIGNED56:
        return QString("UINT56");
    case UNSIGNED64:
        return QString("UINT64");
    }
    return QString();
}

bool NodeSubIndex::isNumeric() const
{
    switch (_dataType)
    {
    case INTEGER8:
    case INTEGER16:
    case INTEGER32:
    case UNSIGNED8:
    case UNSIGNED16:
    case UNSIGNED32:
    case REAL32:
    case REAL64:
    case INTEGER24:
    case INTEGER40:
    case INTEGER48:
    case INTEGER56:
    case INTEGER64:
    case UNSIGNED24:
    case UNSIGNED40:
    case UNSIGNED48:
    case UNSIGNED56:
    case UNSIGNED64:
        return true;

    default:
        return false;
    }
}

QMetaType::Type NodeSubIndex::metaType() const
{
    return NodeOd::dataTypeCiaToQt(_dataType);
}

/**
 * @brief low limit getter
 * @return low limit value
 */
const QVariant &NodeSubIndex::lowLimit() const
{
    return _lowLimit;
}

/**
 * @brief low limit values
 * @param new low limit value
 */
void NodeSubIndex::setLowLimit(const QVariant &lowLimit)
{
    _lowLimit = lowLimit;
}

/**
 * @brief Low limit set
 * @return true if a low limit is set
 */
bool NodeSubIndex::hasLowLimit() const
{
    return _lowLimit.isValid();
}

/**
 * @brief high limit getter
 * @return high limit value
 */
const QVariant &NodeSubIndex::highLimit() const
{
    return _highLimit;
}

/**
 * @brief high limit setter
 * @param new high limit value
 */
void NodeSubIndex::setHighLimit(const QVariant &highLimit)
{
    _highLimit = highLimit;
}

/**
 * @brief High limit set
 * @return true if a high limit is set
 */
bool NodeSubIndex::hasHighLimit() const
{
    return _highLimit.isValid();
}

/**
 * @brief return the length of the sub-index value
 * @return octet sub-index value length
 */
int NodeSubIndex::byteLength() const
{
    switch (_dataType)
    {
    case NodeSubIndex::NONE:
        break;
    case NodeSubIndex::VISIBLE_STRING:
    case NodeSubIndex::OCTET_STRING:
    case NodeSubIndex::UNICODE_STRING:
        // TODO
        break;
    case NodeSubIndex::TIME_OF_DAY:
        // TODO
        break;
    case NodeSubIndex::TIME_DIFFERENCE:
        // TODO
        break;
    case NodeSubIndex::DDOMAIN:
        // TODO
        break;

    case NodeSubIndex::BOOLEAN:
    case NodeSubIndex::UNSIGNED8:
    case NodeSubIndex::INTEGER8:
        return 1;

    case NodeSubIndex::UNSIGNED16:
    case NodeSubIndex::INTEGER16:
        return 2;

    case NodeSubIndex::UNSIGNED24:
    case NodeSubIndex::INTEGER24:
        return 3;

    case NodeSubIndex::UNSIGNED32:
    case NodeSubIndex::INTEGER32:
    case NodeSubIndex::REAL32:
        return 4;

    case NodeSubIndex::UNSIGNED40:
    case NodeSubIndex::INTEGER40:
        return 5;

    case NodeSubIndex::UNSIGNED48:
    case NodeSubIndex::INTEGER48:
        return 6;

    case NodeSubIndex::UNSIGNED56:
    case NodeSubIndex::INTEGER56:
        return 7;

    case NodeSubIndex::UNSIGNED64:
    case NodeSubIndex::INTEGER64:
    case NodeSubIndex::REAL64:
        return 8;
    }
    return 0;
}

int NodeSubIndex::bitLength() const
{
    switch (_dataType)
    {
    case NodeSubIndex::NONE:
        break;
    case NodeSubIndex::VISIBLE_STRING:
    case NodeSubIndex::OCTET_STRING:
    case NodeSubIndex::UNICODE_STRING:
        // TODO
        break;
    case NodeSubIndex::TIME_OF_DAY:
        // TODO
        break;
    case NodeSubIndex::TIME_DIFFERENCE:
        // TODO
        break;
    case NodeSubIndex::DDOMAIN:
        // TODO
        break;

    case NodeSubIndex::BOOLEAN:
        return 1;

    case NodeSubIndex::UNSIGNED8:
    case NodeSubIndex::INTEGER8:
        return 8;

    case NodeSubIndex::UNSIGNED16:
    case NodeSubIndex::INTEGER16:
        return 16;

    case NodeSubIndex::UNSIGNED24:
    case NodeSubIndex::INTEGER24:
        return 24;

    case NodeSubIndex::UNSIGNED32:
    case NodeSubIndex::INTEGER32:
    case NodeSubIndex::REAL32:
        return 32;

    case NodeSubIndex::UNSIGNED40:
    case NodeSubIndex::INTEGER40:
        return 40;

    case NodeSubIndex::UNSIGNED48:
    case NodeSubIndex::INTEGER48:
        return 48;

    case NodeSubIndex::UNSIGNED56:
    case NodeSubIndex::INTEGER56:
        return 56;

    case NodeSubIndex::UNSIGNED64:
    case NodeSubIndex::INTEGER64:
    case NodeSubIndex::REAL64:
        return 64;
    }
    return 0;
}

const QDateTime &NodeSubIndex::lastModification() const
{
    return _lastModification;
}
