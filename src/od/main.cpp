
#include <stdint.h>
#include <iostream>
#include "model/od.h"
#include "parsor/edsparsor.h"
#include "generator/generator.h"

using namespace std;

/**
 * @brief main
 * @return
 */
int main()
{
    OD *od;
    od = new OD;
    EdsParsor parsor;
    Generator generator;

    parsor.parse(od);
    generator.generateH(od);
    generator.generateC(od);
    delete od;

    return 0;
}

