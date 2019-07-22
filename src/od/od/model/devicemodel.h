#ifndef DEVICEMODEL_H
#define DEVICEMODEL_H

#include "od_global.h"

#include <QMap>

#include "index.h"

class OD_EXPORT DeviceModel
{
public:
    DeviceModel();
    ~DeviceModel();

    QMap<QString, QString *> fileInfos() const;
    void setFileInfos(const QMap<QString, QString *> &fileInfos);

    QMap<QString, QString *> dummyUsages() const;
    void setDummyUsages(const QMap<QString, QString *> &dummyUsages);

    QMap<uint16_t, Index *> indexes() const;
    void setIndexes(const QMap<uint16_t, Index *> &indexes);

private:
    QMap<QString, QString *> _fileInfos;
    QMap<QString, QString *> _dummyUsages;
    QMap<uint16_t, Index *> _indexes;
};

#endif // DEVICEMODEL_H
