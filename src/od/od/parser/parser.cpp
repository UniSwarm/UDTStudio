#include "parser.h"
#include "cdfparser.h"

Parser::Parser()
{

}

Parser *Parser::getParser(const QString &type)
{
    if (type == "dcf")
        return new CdfParser;
    return nullptr;
}
