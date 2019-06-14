#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>

#include <stdint.h>
#include "model/od.h"
#include "parser/cdfparser.h"
#include "generator/cgenerator.h"

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
    cliParser.addPositionalArgument("file", QCoreApplication::translate("main", "Object dictionary file (cdf)."), "file");

    QCommandLineOption outOption(QStringList() << "o" << "out",
                                     QCoreApplication::translate("main", "Output directory."),
                                     "out");
    cliParser.addOption(outOption);

    cliParser.process(app);

    const QStringList files = cliParser.positionalArguments();
    if (files.isEmpty())
    {
        out << "error (1): input file is needed" << endl;
        cliParser.showHelp(-1);
    }
    const QString &file = files.at(0);

    // output file
    QString outDirectory = cliParser.value("out");
    if (outDirectory.isEmpty())
        outDirectory = QFileInfo(file).path() + ".lib";

    OD *od;
    QString path(file);

    CdfParser parser;
    CGenerator generator;

    od = parser.parse(path);
    generator.generate(od, outDirectory);
    delete od;

    return 0;
}

