#ifndef DEVICEINIWRITER_H
#define DEVICEINIWRITER_H

#include "od_global.h"

#include <QTextStream>

#include "model/index.h"

class DeviceIniWriter
{
public:
    DeviceIniWriter(QTextStream *file);

    void writeFileInfo(QMap <QString, QString> fileInfos) const;
    void writeDeviceComissioning(QMap <QString, QString> fileComissionings) const;
    void writeDummyUsage(QMap <QString, QString> dummyUsages) const;
    void writeSupportedIndexes(QList<Index *> indexes) const;
    void writeListIndex(QList<Index *> indexes) const;
    void writeIndex(Index *index) const;
    void writeRecord(Index *index) const;
    void writeArray(Index *index) const;
    void writeLimit(SubIndex *subIndex) const;

private:
    QString valueToString(int value, int base = 10) const;
    QString accessToString(int access) const;
    QString dataToString(DataStorage data) const;
    QString pdoToString(uint8_t accessType) const;

    QTextStream *_file;
};

#endif // DEVICEINIWRITER_H
