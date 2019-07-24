#ifndef DEVICEINIWRITER_H
#define DEVICEINIWRITER_H

#include "od_global.h"

#include <QTextStream>

#include "model/index.h"

class DeviceIniWriter
{
public:
    DeviceIniWriter();

    void writeFileInfo(QMap <QString, QString> fileInfos, QTextStream &file) const;
    void writeDummyUsage(QTextStream &file) const;
    void writeSupportedIndexes(QList<Index *> indexes, QTextStream &file) const;
    void writeListIndex(QList<Index *> indexes, QTextStream &file) const;
    void writeIndex(Index *index, QTextStream &file) const;
    void writeRecord(Index *index, QTextStream &file) const;
    void writeArray(Index *index, QTextStream &file) const;
    void writeLimit(SubIndex *subIndex, QTextStream &file) const;

private:
    QString valueToString(int value, int base = 10) const;
    QString accessToString(int access) const;
    QString dataToString(DataStorage data) const;
    QString pdoToString(uint8_t accessType) const;
};

#endif // DEVICEINIWRITER_H
