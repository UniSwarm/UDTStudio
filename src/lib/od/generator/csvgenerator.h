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
    bool generate(DeviceConfiguration *deviceConfiguration, const QString &filePath) const;
    bool generate(DeviceDescription *deviceDescription, const QString &filePath) const;

private:
    QString accessToString(int access) const;
    QString pdoToString(uint8_t accessType) const;

    void writeListIndex(const QList<Index *> indexes, QTextStream *out) const;
    void writeIndex(Index *index, QTextStream *out) const;
    void writeRecord(Index *index, QTextStream *out) const;
    void writeArray(Index *index, QTextStream *out) const;
    void writeLimit(const SubIndex *subIndex, QTextStream *out) const;
};

#endif // CSVGENERATOR_H
