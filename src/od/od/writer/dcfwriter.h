#ifndef DCFWRITER_H
#define DCFWRITER_H

#include "od_global.h"

#include <QTextStream>
#include <QString>
#include <QList>

#include "writer.h"

class OD_EXPORT DcfWriter : public Writer
{
public:
    DcfWriter();

    void write(OD *od, const QString &dir) const;

protected:
    void writeSupportedIndexes(QList<Index *> indexes, QTextStream &file) const;
    void writeListIndex(QList<Index *> indexes, QTextStream &file) const;
    void writeIndex(Index *index, QTextStream &file) const;
    void writeRecord(Index *index, QTextStream &file) const;
    void writeArray(Index *index, QTextStream &file) const;

private:
    QString valueToString(int value, int base = 10) const;
    QString accessToString(int access) const;
};

#endif // DCFWRITER_H
