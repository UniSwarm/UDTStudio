#ifndef EDSPARSOR_H
#define EDSPARSOR_H

#include <QString>

#include "../model/od.h"

class EdsParsor
{
public:
    EdsParsor(QString path);
    void parse(OD *od);

private:
    QString _edsFile;
};

#endif // EDSPARSOR_H
