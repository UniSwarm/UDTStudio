#ifndef GENERATOR_H
#define GENERATOR_H

#include "od_global.h"

#include <QString>
#include <QTextStream>

#include "model/od.h"

class OD_EXPORT Generator
{
public:
    Generator(QString path);
    void generateH(OD *od) const;
    void generateC(OD *od) const;

private:
    QString typeToString(const uint16_t &type) const;
    QString varNameToString(const QString &name) const;
    QString structNameToString(const QString &name) const;
    QString dataToString(const SubIndex *index) const;
    QString typeObjectToString(const SubIndex *subIndex) const;

    void writeRecordDefinitionH(Index *index, QTextStream &hFile) const;
    void writeIndexH(Index *index, QTextStream &hFile) const;
    void writeRamLineC(Index *index, QTextStream &cFile) const;
    void writeRecordCompletionC(Index *index, QTextStream &cFile) const;
    void writeOdCompletionC(Index *index, QTextStream &cFile) const;

    QString _dir;
};

#endif // GENERATOR_H
