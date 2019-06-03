#ifndef EDSPARSER_H
#define EDSPARSER_H

#include <QString>

#include "model/od.h"

class EdsParser
{
public:
    EdsParser(QString path);
    void parse(OD *od);

private:
    QString _edsFile;
};

#endif // EDSPARSER_H
