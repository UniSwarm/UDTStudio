#ifndef INDEX_H
#define INDEX_H

#include "od_global.h"

#include <QList>
#include <QString>

#include "subindex.h"
#include "datatype.h"

#include <stdint.h>

class OD_EXPORT Index : public SubIndex
{
public:
    Index(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName, const uint16_t &index, const uint8_t &nbSubIndex);
    ~Index();

    uint16_t index() const;
    void setIndex(const uint16_t &index);

    uint8_t nbSubIndex() const;
    void setNbSubIndex(const uint8_t &nbSubIndex);

    QList<SubIndex *> &subIndexes();
    SubIndex* subIndex(const uint8_t &subIndexKey) const;
    void addSubIndex(SubIndex *subIndex);

private:
    uint16_t _index;
    uint8_t _nbSubIndex;
    QList<SubIndex*> _subIndexes;
};

#endif // INDEX_H
