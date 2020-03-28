#include "nodesubindex.h"

NodeSubIndex::NodeSubIndex(const quint8 subIndex)
{
    _subIndex = subIndex;
    _flagLimit = 0;
    _accessType = NOACESS;
    _value = QVariant();
    _dataType = NONE;
}

NodeSubIndex::NodeSubIndex(const NodeSubIndex &other)
    : QObject ()
{
    _accessType = other.accessType();
    _subIndex = other.subIndex();
    _name = other.name();
    _value = other.value();

    _flagLimit = other.flagLimit();
    _lowLimit = other.lowLimit();
    _highLimit = other.highLimit();

    _dataType = other.dataType();
}

/**
 * @brief destructor
 */
NodeSubIndex::~NodeSubIndex()
{
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
    _lastModification = QTime().currentTime();
}

/**
 * @brief _value setter
 * @param new sub-index value
 */
void NodeSubIndex::clearValue()
{
    _value.clear();
    _lastModification = QTime().currentTime();
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
 * @brief flag limit getter
 * @return 0x1 if sub-index has low limit, 0x2 if has high limit and 0x3 if has both. Else 0.
 */
uint8_t NodeSubIndex::flagLimit() const
{
    return _flagLimit;
}

/**
 * @brief flag limit setter
 * @param new flag limit code
 */
void NodeSubIndex::setFlagLimit(const uint8_t &flagLimit)
{
    _flagLimit = flagLimit;
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
        break;
    case NodeSubIndex::TIME_OF_DAY:
        break;
    case NodeSubIndex::TIME_DIFFERENCE:
        break;
    case NodeSubIndex::DDOMAIN:
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

const QTime &NodeSubIndex::lastModification() const
{
    return _lastModification;
}
