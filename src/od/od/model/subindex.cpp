
#include "subindex.h"
#include "od.h"

using namespace std;

/**
 * @brief constructor
 * @param data type
 * @param object type
 * @param access type
 * @param parameter name
 */
SubIndex::SubIndex(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName)
{
    _dataType   = dataType;
    _objectType = objectType;
    _accessType = accessType;
    _parameterName = parameterName;
}

/**
 * @brief destructor
 */
SubIndex::~SubIndex()
{
    qDeleteAll(_datas);
}

/**
 * @brief _dataType getter
 * @return data type
 */
uint16_t SubIndex::dataType() const
{
    return _dataType;
}

/**
 * @brief _dataType setter
 * @param data type
 */
void SubIndex::setDataType(const uint16_t &dataType)
{
    _dataType = dataType;
}

/**
 * @brief _objectType getter
 * @return object type
 */
uint8_t SubIndex::objectType() const
{
    return _objectType;
}

/**
 * @brief _objectType setter
 * @param object type
 */
void SubIndex::setObjectType(const uint8_t &objectType)
{
    _objectType = objectType;
}

/**
 * @brief _accessType getter
 * @return access type
 */
uint8_t SubIndex::accessType() const
{
    return _accessType;
}

/**
 * @brief accessType setter
 * @param access type
 */
void SubIndex::setAccessType(const uint8_t &accessType)
{
    _accessType = accessType;
}

/**
 * @brief _datas getter
 * @return data list
 */
QList<DataType*> &SubIndex::datas()
{
    return _datas;
}

/**
 * @brief find a sub-index data
 * @param sub index number
 * @return data or wrong value u=if sub-index number does not exist
 */
DataType *SubIndex::data(const uint16_t &dataKey) const
{
    DataType *data = _datas.value(dataKey);
    return data;
}

/**
 * @brief adds data to the data list
 * @param data
 */
void SubIndex::addData(DataType *data)
{
    _datas.append(data);
}

/**
 * @brief _parameterName getter
 * @return parameter name
 */
QString SubIndex::parameterName() const
{
    return _parameterName;
}

/**
 * @brief _parameterName setter
 * @param parameter name
 */
void SubIndex::setParameterName(const QString &parameterName)
{
    _parameterName = parameterName;
}

/**
 * @brief returns the length of data
 * @return 0 for records or data size in octets
 */
uint8_t SubIndex::length() const
{
    if (_objectType == OD_OBJECT_RECORD)
        return 0;

    switch (_dataType)
    {
    case OD_TYPE_INTEGER8:
        return 1;

    case OD_TYPE_INTEGER16:
        return 2;

    case OD_TYPE_INTEGER32:
        return 4;

    case OD_TYPE_INTEGER64:
        return 8;

    case OD_TYPE_UNSIGNED8:
        return 1;

    case OD_TYPE_UNSIGNED16:
        return 2;

    case OD_TYPE_UNSIGNED32:
        return 4;

    case OD_TYPE_REAL32:
        return 4;

    case OD_TYPE_REAL64:
        return 8;
    }

    return  0;
}
