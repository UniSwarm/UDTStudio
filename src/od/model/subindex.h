#ifndef SUBINDEX_H
#define SUBINDEX_H

#include "od_global.h"

#include<QList>
#include<QString>

#include "datatype.h"

class OD_EXPORT SubIndex
{
public:

    SubIndex(const uint16_t &dataType, const uint8_t &objectType, const uint8_t &accessType, const QString &parameterName);
    ~SubIndex();

    uint16_t dataType() const;
    void setDataType(const uint16_t &dataType);

    uint8_t objectType() const;
    void setObjectType(const uint8_t &objectType);

    uint8_t accessType() const;
    void setAccessType(const uint8_t &accessType);

    QList<DataType*> &datas();
    DataType* data(const uint16_t &dataKey) const;
    void addData(DataType *data);

    QString parameterName() const;
    void setParameterName(const QString &parameterName);

    uint8_t length() const;

private:

    uint16_t        _dataType;
    uint8_t         _objectType;
    uint8_t         _accessType;
    QString         _parameterName;
    QList<DataType*> _datas;
};

#endif // SUBINDEX_H
