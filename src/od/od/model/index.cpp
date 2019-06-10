#include "index.h"

/**
 * @brief constructor
 * @param data type
 * @param object type
 * @param access type
 * @param parameter name
 * @param index
 * @param sub-index number
 */
Index::Index(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName, const uint16_t &index, const uint8_t &nbSubIndex)
    : SubIndex(dataType, objectType, accessType, parameterName)
{
    _index = index;
    _nbSubIndex = nbSubIndex;
}

/**
 * @brief destuctor
 */
Index::~Index()
{
    qDeleteAll(_subIndexes.begin(), _subIndexes.end());
    _subIndexes.clear();
}

/**
 * @brief _index getter
 * @return index number
 */
uint16_t Index::index() const
{
    return _index;
}

/**
 * @brief _index setter
 * @param index number
 */
void Index::setIndex(const uint16_t &index)
{
    _index = index;
}

/**
 * @brief _nbSubIndex setter
 * @return number of sub-indexes
 */
uint8_t Index::nbSubIndex() const
{
    return _nbSubIndex;
}

/**
 * @brief _nbSubIndex setter
 * @param number of sub-indexes
 */
void Index::setNbSubIndex(const uint8_t &nbSubIndex)
{
    _nbSubIndex = nbSubIndex;
}

/**
 * @brief _subIndexes getter
 * @return list of sub-index
 */
QList<SubIndex*> &Index::subIndexes()
{
    return _subIndexes;
}

/**
 * @brief finds a sub-index
 * @param sub-index number
 * @return sub-index
 */
SubIndex* Index::subIndex(const uint8_t &subIndexKey) const
{
     SubIndex *subIndex = _subIndexes.value(subIndexKey);
     return subIndex;
}

/**
 * @brief adds a sub-index
 * @param sub-index
 */
void Index::addSubIndex(SubIndex *subIndex)
{
    _subIndexes.append(subIndex);
}
