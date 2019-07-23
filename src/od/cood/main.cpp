#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>

#include <stdint.h>
#include "model/devicemodel.h"
#include "parser/dcfparser.h"
#include "generator/cgenerator.h"
#include "writer/dcfwriter.h"

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
    cliParser.addPositionalArgument("file", QCoreApplication::translate("main", "Object dictionary file (dcf)."), "file");

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

    QString path(file);

    DcfParser parser;
    CGenerator generator;
    DcfWriter writer;

//    DeviceDescription *od;
//    od = (DeviceDescription*)parser.parse(path, "eds");

    DeviceConfiguration *od;
    od = (DeviceConfiguration*)parser.parse(path, "dcf");
    generator.generate(od, outDirectory);
    writer.write(od, outDirectory);
    delete od;

    return 0;
}

