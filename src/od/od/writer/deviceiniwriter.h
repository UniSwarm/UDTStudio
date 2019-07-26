#ifndef DEVICEINIWRITER_H
#define DEVICEINIWRITER_H

#include "od_global.h"

#include <QTextStream>

#include "model/devicemodel.h"

class DeviceIniWriter
{
public:
    DeviceIniWriter(QTextStream *file);

    void writeObjects(const DeviceModel *deviceModel) const;
    void writeFileInfo(const QMap <QString, QString> &fileInfos) const;
    void writeDeviceComissioning(const QMap <QString, QString> &deviceComissionings) const;
    void writeDeviceInfo(const QMap <QString, QString> &deviceInfos) const;
    void writeDummyUsage(const QMap <QString, QString> &dummyUsages) const;
    void writeSupportedIndexes(const QList<Index *> &indexes) const;
    void writeListIndex(const QList<Index *> indexes) const;
    void writeIndex(Index *index) const;
    void writeRecord(Index *index) const;
    void writeArray(Index *index) const;
    void writeLimit(const SubIndex *subIndex) const;
    void writeStringMap(const QMap <QString, QString> &map) const;

private:
    QString valueToString(int value, int base = 10) const;
    QString accessToString(int access) const;
    QString dataToString(const QVariant &value) const;
    QString pdoToString(uint8_t accessType) const;

    QTextStream *_file;
};

#endif // DEVICEINIWRITER_H
