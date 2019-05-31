#ifndef GENERATOR_H
#define GENERATOR_H

#include <QString>
#include <QTextStream>

#include "../model/od.h"

class Generator
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

    void writeRecordLineH(Index *index, QTextStream &out) const;
    void writeRamLineH(Index *index, QTextStream &out) const;
    void writeRamLineC(Index *index, QTextStream &out) const;
    void writeRecordCompletion(Index *index, QTextStream &out) const;

    QString _dir;
};

#endif // GENERATOR_H
