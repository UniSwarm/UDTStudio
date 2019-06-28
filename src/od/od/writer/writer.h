#ifndef WRITER_H
#define WRITER_H

#include "od_global.h"

#include <QString>

#include "model/od.h"

class OD_EXPORT Writer
{
public:
    Writer();

    virtual void write(OD *od, const QString &dir) const = 0;

    static Writer *getWriter(const QString &type);
};

#endif // WRITER_H
