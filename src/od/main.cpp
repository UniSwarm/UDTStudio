#include <QCoreApplication>
#include <QCommandLineParser>

#include <stdint.h>
#include "model/od.h"
#include "parser/edsparser.h"
#include "generator/generator.h"

/**
 * @brief main
 * @return
 */
int main()
{
    OD *od;
    od = new OD;

    QString path("../eds/301.eds");

    EdsParser parser(path);
    Generator generator("../src/test");

    parser.parse(od);
    generator.generateH(od);
    generator.generateC(od);
    delete od;

    return 0;
}

