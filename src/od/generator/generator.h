#ifndef GENERATOR_H
#define GENERATOR_H

#include <QString>
#include <QTextStream>

#include "../model/od.h"

class Generator
{
public:
    Generator();
    void generateH(OD *od) const;

private:
    QString typeToString(const uint16_t &type) const;
    QString varNameToString(const QString &name) const;
    QString structNameToString(const QString &name) const;

    QString writeRecordLineH(Index *index) const;
    QString writeRamLineH(Index *index) const;
};

#endif // GENERATOR_H
