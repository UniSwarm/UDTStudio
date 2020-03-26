#include "nodesubindex.h"

NodeSubIndex::NodeSubIndex(const quint8 subIndex)
{
    _subIndex = subIndex;
    _flagLimit = 0;
    _accessType = 0;
    _value = QVariant(0);
    _dataType = NONE;
}

NodeSubIndex::NodeSubIndex(const NodeSubIndex &other)
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
uint8_t NodeSubIndex::accessType() const
{
    return _accessType;
}

/**
 * @brief access type setter
 * @param 8 bits access type code
 */
void NodeSubIndex::setAccessType(const uint8_t &accessType)
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
NodeSubIndex::Type NodeSubIndex::dataType() const
{
    switch (_dataType)
    {
    case BOOLEAN:
        return BOOLEAN;
    case INTEGER8:
        return INTEGER8;
    case INTEGER16:
        return INTEGER16;
    case INTEGER32:
        return INTEGER32;
    case UNSIGNED8:
        return UNSIGNED8;
    case UNSIGNED16:
        return UNSIGNED16;
    case UNSIGNED32:
        return UNSIGNED32;
    case REAL32:
        return REAL32;
    case VISIBLE_STRING:
        return VISIBLE_STRING;
    case OCTET_STRING:
        return OCTET_STRING;
    case UNICODE_STRING:
        return UNICODE_STRING;
    case TIME_OF_DAY:
        return TIME_OF_DAY;
    case TIME_DIFFERENCE:
        return TIME_DIFFERENCE;
    case DDOMAIN:
        return DDOMAIN;
    case INTEGER24:
        return INTEGER24;
    case REAL64:
        return REAL64;
    case INTEGER40:
        return INTEGER40;
    case INTEGER48:
        return INTEGER48;
    case INTEGER56:
        return INTEGER56;
    case INTEGER64:
        return INTEGER64;
    case UNSIGNED24:
        return UNSIGNED24;
    case UNSIGNED40:
        return UNSIGNED40;
    case UNSIGNED48:
        return UNSIGNED48;
    case UNSIGNED56:
        return UNSIGNED56;
    case UNSIGNED64:
        return UNSIGNED64;
    default:
        return NONE;
    }
}

/**
 * @brief _dataType setter
 * @param new 16 bits data type code
 */
void NodeSubIndex::setDataType(const Type &dataType)
{
    _dataType = dataType;
}

/**
 * @brief converts an data type code to his corresponding string
 * @param 16 bits data type code
 * @return data type string
 */
QString NodeSubIndex::dataTypeStr(const Type &dataType)
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
int NodeSubIndex::length() const
{
    switch (_dataType)
    {
    case BOOLEAN:
    case UNSIGNED8:
    case INTEGER8:
        return 1;

    case UNSIGNED16:
    case INTEGER16:
        return 2;

    case UNSIGNED32:
    case INTEGER32:
    case REAL32:
        return 4;

    case UNSIGNED64:
    case INTEGER64:
    case REAL64:
        return 8;
    }
    return 0;
}

QTime NodeSubIndex::lastModification()
{
    return _lastModification;
}

