#ifndef EDSPARSER_H
#define EDSPARSER_H

#include "od_global.h"

#include <QString>

#include "model/od.h"

class OD_EXPORT EdsParser
{
public:
    EdsParser(QString path);
    void parse(OD *od);

private:
    QString _edsFile;
};

#endif // EDSPARSER_H
