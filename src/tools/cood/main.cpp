/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
 **
 ** This program is free software: you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation, either version 3 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program. If not, see <http://www.gnu.org/licenses/>.
 **/

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QFileInfo>

#include <cstdint>
#include "model/devicemodel.h"
#include "parser/edsparser.h"
#include "parser/dcfparser.h"
#include "generator/cgenerator.h"
#include "writer/dcfwriter.h"
#include "writer/edswriter.h"
#include "generator/texgenerator.h"
#include "generator/csvgenerator.h"

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
    QTextStream err(stderr);

    QCommandLineParser cliParser;
    cliParser.setApplicationDescription(QCoreApplication::translate("main", "Object Dicitonary command line interface."));
    cliParser.addHelpOption();
    cliParser.addVersionOption();
    cliParser.addPositionalArgument("file", QCoreApplication::translate("main", "Object dictionary file (dcf)."), "file");

    QCommandLineOption outOption(QStringList() << "o" << "out",
                                     QCoreApplication::translate("main", "Output directory or file."),
                                     "out");
    cliParser.addOption(outOption);

    QCommandLineOption nodeIdOption(QStringList() << "n" << "nodeid",
                                     QCoreApplication::translate("main", "CANOpen Node Id."),
                                     "nodeid");
    nodeIdOption.setDefaultValue("0");
    cliParser.addOption(nodeIdOption);

    cliParser.process(app);

    const QStringList files = cliParser.positionalArguments();
    if (files.isEmpty())
    {
        err << "error (1): input file is needed" << endl;
        cliParser.showHelp(-1);
    }
    const QString &inputFile = files.at(0);
    QString inSuffix = QFileInfo(inputFile).suffix();

    // output file
    QString outputFile = cliParser.value("out");
    if (outputFile.isEmpty())
    {
        outputFile = QFileInfo(inputFile).path();
    }

    // node Id
    uint8_t nodeid = 0;
    if (inSuffix == "eds")
    {
        nodeid = static_cast<uint8_t>(cliParser.value("nodeid").toUInt());
        if (nodeid == 0 || nodeid >= 126)
        {
            err << "error (2): invalid node id, nodeId > 0 && nodeId < 126" << endl;
            return -2;
        }
    }

    // input file opening
    DeviceDescription *deviceDescription = nullptr;
    DeviceConfiguration *deviceConfiguration = nullptr;
    if (inSuffix == "eds")
    {
        EdsParser parser;
        deviceDescription = parser.parse(inputFile);
        deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, nodeid);
    }
    else if (inSuffix == "dcf")
    {
        DcfParser parser;
        deviceConfiguration = parser.parse(inputFile);
    }
    else
    {
        err << "error (3): invalid input file format, .eds or .dcf accepted" << endl;
        return -3;
    }

    QString outSuffix = QFileInfo(outputFile).suffix();
    CGenerator cgenerator;
    DcfWriter dcfWriter;
    if (outSuffix == "c")
    {
        cgenerator.generateC(deviceConfiguration, outputFile);
    }
    else if (outSuffix == "h")
    {
        cgenerator.generateH(deviceConfiguration, outputFile);
    }
    else if (outSuffix == "dcf")
    {
        dcfWriter.write(deviceConfiguration, outputFile);
    }
    else if (outSuffix == "eds" && deviceDescription)
    {
        EdsWriter edsWriter;
        edsWriter.write(deviceDescription, outputFile);
    }
    else if (outSuffix == "tex" && deviceDescription)
    {
        TexGenerator texGenerator;
        texGenerator.generate(deviceDescription, outputFile);
    }
    else if (outSuffix == "csv" && deviceDescription)
    {
        CsvGenerator csvGenerator;
        csvGenerator.generate(deviceDescription, outputFile);
    }
    else if (QFileInfo(outputFile).isDir())
    {
        cgenerator.generateC(deviceConfiguration, QString(outputFile + "/od_data.c"));
        cgenerator.generateH(deviceConfiguration, QString(outputFile + "/od_data.h"));
        dcfWriter.write(deviceConfiguration, QString(outputFile + "/out.dcf"));
    }
    else
    {
        delete deviceDescription;
        delete deviceConfiguration;
        err << "error (4): invalid output file format, .c, .h, .dcf, .eds, .csv or .tex accepted" << endl;
        return -4;
    }

    delete deviceDescription;
    delete deviceConfiguration;

    return 0;
}
