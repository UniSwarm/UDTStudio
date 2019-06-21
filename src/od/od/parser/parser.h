#ifndef PARSER_H
#define PARSER_H

#include <QString>

#include "od_global.h"

#include "model/od.h"

class OD_EXPORT Parser
{
public:
    Parser();

    virtual OD *parse(const QString &path) const = 0;

    static Parser *getParser(const QString &type);
};

#endif // PARSER_H
