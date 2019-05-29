#include "index.h"

/**
 * @brief Index::Index
 */
Index::Index()
{

}
/**
 * @brief Index::Index
 * @param index
 * @param nbSubIndex
 */
Index::Index(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName, const uint16_t &index, const uint8_t &nbSubIndex) : SubIndex(dataType, objectType, accessType, parameterName)
{
    _index = index;
    _nbSubIndex = nbSubIndex;
}
/**
 * @brief Index::~Index
 */
Index::~Index()
{
    qDeleteAll(_subIndexes.begin(), _subIndexes.end());
    _subIndexes.clear();
}
/**
 * @brief Index::index
 * @return
 */
uint16_t Index::index() const
{
    return _index;
}
/**
 * @brief Index::setIndex
 * @param index
 */
void Index::setIndex(const uint16_t &index)
{
    _index = index;
}
/**
 * @brief Index::nbSubIndex
 * @return
 */
uint8_t Index::nbSubIndex() const
{
    return _nbSubIndex;
}
/**
 * @brief Index::setNbSubIndex
 * @param nbSubIndex
 */
void Index::setNbSubIndex(const uint8_t &nbSubIndex)
{
    _nbSubIndex = nbSubIndex;
}
/**
 * @brief Index::subIndexes
 * @return
 */
QList<SubIndex*> &Index::subIndexes()
{
    return _subIndexes;
}
/**
 * @brief Index::subIndex
 * @param subIndexKey
 * @return
 */
SubIndex* Index::subIndex(const uint8_t &subIndexKey) const
{
     SubIndex *subIndex = _subIndexes.value(subIndexKey);
     return subIndex;
}
/**
 * @brief Index::addSubIndex
 * @param index
 */
void Index::addSubIndex(SubIndex *subIndex)
{
    _subIndexes.append(subIndex);
}
