
#include "od.h"

/**
 * @brief OD::OD
 */
OD::OD()
{

}
/**
 * @brief OD::~OD
 */
OD::~OD()
{
    qDeleteAll(_indexes);
}
/**
 * @brief OD::getIndex
 * @param index
 * @return index or defaultValue
 */
Index* OD::index(const uint16_t &indexKey) const
{
    Index *index = _indexes.value(indexKey);
    return index;
}
/**
 * @brief OD::addIndex
 * @param index
 */
void OD::addIndex(Index *index)
{
    _indexes.insert(index->index(), index);
}
/**
 * @brief OD::indexCount
 * @return number of index in the object dictionary
 */
int OD::indexCount() const
{
    return _indexes.count();
}
/**
 * @brief OD::indexes
 * @return
 */
QMap<uint16_t, Index*> &OD::indexes()
{
    return _indexes;
}
