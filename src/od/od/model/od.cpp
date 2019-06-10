#include "od.h"

/**
 * @brief constructor
 */
OD::OD()
{

}

/**
 * @brief destructor
 */
OD::~OD()
{
    qDeleteAll(_indexes);
}

/**
 * @brief find an index
 * @param index number
 * @return index or wrong value if index number does not exist
 */
Index* OD::index(const uint16_t &indexKey) const
{
    Index *index = _indexes.value(indexKey);
    return index;
}

/**
 * @brief adds a index to the object dictionary
 * @param index
 */
void OD::addIndex(Index *index)
{
    _indexes.insert(index->index(), index);
}

/**
 * @brief counts the number of indexes in the object dictionary
 * @return the number of indexes in the object dictionary
 */
int OD::indexCount() const
{
    return _indexes.count();
}

/**
 * @brief _indexes getter
 * @return map of index
 */
QMap<uint16_t, Index*> &OD::indexes()
{
    return _indexes;
}
