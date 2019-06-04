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

    EdsParser parser("../../eds/301.eds");
    Generator generator("/home/alexis/Documents/code/testOD");

    parser.parse(od);
    generator.generateH(od);
    generator.generateC(od);
    delete od;

    return 0;
}

