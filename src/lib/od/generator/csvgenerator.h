#ifndef CSVGENERATOR_H
#define CSVGENERATOR_H

#include "od_global.h"
#include "generator/generator.h"

#include <QTextStream>
#include <QString>

#include "model/deviceconfiguration.h"
#include "model/devicedescription.h"

class OD_EXPORT CsvGenerator : public Generator
{

public:
    CsvGenerator();
    ~CsvGenerator();

    // Generator interface
    bool generate(DeviceConfiguration *deviceConfiguration, const QString &filePath);
    bool generate(DeviceDescription *deviceDescription, const QString &filePath);

private:
    static QString accessToString(int access);
    static QString pdoToString(uint8_t accessType);

    void writeListIndex(const QList<Index *> indexes, QTextStream *out);
    void writeIndex(Index *index, QTextStream *out);
    void writeRecord(Index *index, QTextStream *out);
    void writeArray(Index *index, QTextStream *out);
    void writeLimit(const SubIndex *subIndex, QTextStream *out);
};

#endif // CSVGENERATOR_H
