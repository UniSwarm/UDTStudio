
#include <iostream>

#include "subindex.h"
#include "od.h"

using namespace std;

/**
 * @brief SubIndex::SubIndex
 */
SubIndex::SubIndex()
{

}
/**
 * @brief SubIndex::SubIndex
 * @param dataType
 * @param objectType
 * @param accessType
 */
SubIndex::SubIndex(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName)
{
    _dataType   = dataType;
    _objectType = objectType;
    _accessType = accessType;
    _parameterName = parameterName;
}
/**
 * @brief SubIndex::~SubIndex
 */
SubIndex::~SubIndex()
{
    qDeleteAll(_datas);
}
/**
 * @brief SubIndex::dataType
 * @return
 */
uint16_t SubIndex::dataType() const
{
    return _dataType;
}
/**
 * @brief SubIndex::setDataType
 * @param dataType
 */
void SubIndex::setDataType(const uint16_t &dataType)
{
    _dataType = dataType;
}
/**
 * @brief SubIndex::objectType
 * @return
 */
uint8_t SubIndex::objectType() const
{
    return _objectType;
}
/**
 * @brief SubIndex::setObjectType
 * @param objectType
 */
void SubIndex::setObjectType(const uint8_t &objectType)
{
    _objectType = objectType;
}
/**
 * @brief SubIndex::accessType
 * @return
 */
uint8_t SubIndex::accessType() const
{
    return _accessType;
}
/**
 * @brief SubIndex::setAccessType
 * @param accessType
 */
void SubIndex::setAccessType(const uint8_t &accessType)
{
    _accessType = accessType;
}
/**
 * @brief SubIndex::datas
 * @return
 */
QList<DataType*> &SubIndex::datas()
{
    return _datas;
}
/**
 * @brief SubIndex::data
 * @param dataKey
 * @return
 */
DataType *SubIndex::data(const uint16_t &dataKey) const
{
    DataType *data = _datas.value(dataKey);
    return data;
}
/**
 * @brief SubIndex::addData
 * @param data
 */
void SubIndex::addData(DataType *data)
{
    _datas.append(data);
}

QString SubIndex::parameterName() const
{
    return _parameterName;
}

void SubIndex::setParameterName(const QString &parameterName)
{
    _parameterName = parameterName;
}

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
}
