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

    QMap<QString, QString> fileInfos() const;
    void setFileInfos(const QMap<QString, QString> &fileInfos);
    void setFileInfo(const QString &key, const QString &value);

    QMap<QString, QString> dummyUsages() const;
    void setDummyUsages(const QMap<QString, QString> &dummyUsages);

    QMap<uint16_t, Index *> indexes() const;
    Index *index(const uint16_t &index) const;
    void addIndex(Index *index);
    int indexCount() const;

private:
    QMap<QString, QString> _fileInfos;
    QMap<QString, QString> _dummyUsages;
    QMap<uint16_t, Index *> _indexes;
};

#endif // DEVICEMODEL_H
