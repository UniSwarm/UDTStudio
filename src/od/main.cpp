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
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("OD");
    QCoreApplication::setApplicationVersion("1.0");

    QTextStream out(stdout);

    QCommandLineParser cliParser;
    cliParser.setApplicationDescription(QCoreApplication::translate("main", "Object Dicitonary command line interface."));
    cliParser.addHelpOption();
    cliParser.addVersionOption();
    cliParser.addPositionalArgument("file", QCoreApplication::translate("main", "Source file to extract pins (pdf)."), "file");
    cliParser.process(app);

    const QStringList files = cliParser.positionalArguments();
    if (files.isEmpty())
    {
        out << "error (1): input file is needed" << endl;
        cliParser.showHelp(-1);
    }
    const QString &file = files.at(0);

    OD *od;
    od = new OD;

    QString path(file);

    EdsParser parser(path);
    Generator generator("../src/test");

    parser.parse(od);
    generator.generateH(od);
    generator.generateC(od);
    delete od;

    return 0;
}

