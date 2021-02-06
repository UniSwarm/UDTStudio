/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2021 UniSwarm
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

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFileInfo>

#include "generator/cgenerator.h"
#include "generator/csvgenerator.h"
#include "generator/texgenerator.h"
#include "model/devicemodel.h"
#include "parser/dcfparser.h"
#include "parser/edsparser.h"
#include "utility/configurationapply.h"
#include "utility/odmerger.h"
#include "utility/profileduplicate.h"
#include "writer/dcfwriter.h"
#include "writer/edswriter.h"

#include <cstdint>

/**
 * @brief main
 * @return
 */
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("OD");
    QCoreApplication::setApplicationVersion("1.0");

    QTextStream out(stdout, QIODevice::WriteOnly);
    QTextStream err(stderr, QIODevice::WriteOnly);

    QCommandLineParser cliParser;
    cliParser.setApplicationDescription(QCoreApplication::translate("main", "Object dictionary command line interface."));
    cliParser.addHelpOption();
    cliParser.addVersionOption();
    cliParser.addPositionalArgument("file", QCoreApplication::translate("main", "Object dictionary file (dcf or eds)."), "file");

    QCommandLineOption outOption(QStringList() << "o"
                                               << "out",
                                 QCoreApplication::translate("main", "Output directory or file."),
                                 "out");
    cliParser.addOption(outOption);

    QCommandLineOption nodeIdOption(QStringList() << "n"
                                                  << "nodeid",
                                    QCoreApplication::translate("main", "CANOpen Node Id."),
                                    "0");
    cliParser.addOption(nodeIdOption);

    QCommandLineOption duplicateOption(QStringList() << "d"
                                                     << "duplicate",
                                       QCoreApplication::translate("main", "Duplicate profile"),
                                       "0");
    cliParser.addOption(duplicateOption);

    QCommandLineOption configurationOption(QStringList() << "c"
                                                         << "configuration",
                                           QCoreApplication::translate("main", "Configuration apply"));
    cliParser.addOption(configurationOption);

    QCommandLineOption mergeOption(QStringList() << "m"
                                                 << "merge",
                                   QCoreApplication::translate("main", "Merge 2 eds files"));
    cliParser.addOption(mergeOption);

    cliParser.process(app);

    const QStringList files = cliParser.positionalArguments();
    if (files.isEmpty())
    {
        err << QCoreApplication::translate("main", "error (1): input file is needed") << endl;
        cliParser.showHelp(-1);
    }
    const QString &inputFile = files.at(0);
    QString inSuffix = QFileInfo(inputFile).suffix();

    // node Id
    uint8_t nodeid = 0;

    // output file
    QString outputFile = cliParser.value("out");
    QString outSuffix = QFileInfo(outputFile).suffix();
    if (outputFile.isEmpty())
    {
        outputFile = inputFile;
        outSuffix = QFileInfo(outputFile).suffix();
    }
    else
    {
        if (inSuffix == "eds" && outSuffix != "eds")
        {
            nodeid = static_cast<uint8_t>(cliParser.value("nodeid").toUInt());
            if (nodeid == 0 || nodeid >= 126)
            {
                err << QCoreApplication::translate("main", "error (2): invalid node id, nodeId > 0 && nodeId < 126") << endl;
                return -2;
            }
        }
    }

    // INPUT FILE OPENING
    DeviceDescription *deviceDescription = nullptr;
    DeviceConfiguration *deviceConfiguration = nullptr;
    if (inSuffix == "eds")
    {
        EdsParser parser;
        deviceDescription = parser.parse(inputFile);
        if (!deviceDescription)
        {
            err << QCoreApplication::translate("main", "error (5): invalid eds file or file does not exist") << endl;
            return -5;
        }
        deviceConfiguration = DeviceConfiguration::fromDeviceDescription(deviceDescription, nodeid);
    }
    else if (inSuffix == "dcf")
    {
        DcfParser parser;
        deviceConfiguration = parser.parse(inputFile);
    }
    else
    {
        err << QCoreApplication::translate("main", "error (3): invalid input file format, .eds or .dcf accepted") << endl;
        return -3;
    }

    QString iniFile = cliParser.value("configuration");
    if (!iniFile.isEmpty())
    {
        ConfigurationApply configurationApply;
        configurationApply.apply(deviceConfiguration, iniFile);
    }

    // OUTPUT FILE
    if (outSuffix == "c")
    {
        CGenerator cgenerator;
        cgenerator.generateC(deviceConfiguration, outputFile);
    }
    else if (outSuffix == "h")
    {
        CGenerator cgenerator;
        cgenerator.generateH(deviceConfiguration, outputFile);
    }
    else if (outSuffix == "dcf")
    {
        DcfWriter dcfWriter;
        dcfWriter.write(deviceConfiguration, outputFile);
    }
    else if (outSuffix == "eds" && deviceDescription)
    {
        uint8_t duplicate = 0;
        duplicate = static_cast<uint8_t>(cliParser.value("duplicate").toUInt());
        if (duplicate != 0)
        {
            ProfileDuplicate profileDuplicate;
            profileDuplicate.duplicate(deviceDescription, duplicate);
        }

        if (cliParser.isSet("merge"))
        {
            if (files.size() < 2)
            {
                delete deviceDescription;
                delete deviceConfiguration;
                err << QCoreApplication::translate("main", "error (6): invalid number of eds inputs file, need more than one") << endl;
                return -6;
            }
            EdsParser parser;
            DeviceDescription *secondDeviceDescription = parser.parse(files.at(1));
            if (!secondDeviceDescription)
            {
                delete deviceDescription;
                delete deviceConfiguration;
                err << QCoreApplication::translate("main", "error (5): invalid eds file or file does not exist") << endl;
                return -5;
            }
            ODMerger merger;
            merger.merge(deviceDescription, secondDeviceDescription);
        }

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
        CGenerator cgenerator;
        DcfWriter dcfWriter;
        cgenerator.generateC(deviceConfiguration, QString(outputFile + "/od_data.c"));
        cgenerator.generateH(deviceConfiguration, QString(outputFile + "/od_data.h"));
        dcfWriter.write(deviceConfiguration, QString(outputFile + "/out.dcf"));
    }
    else
    {
        delete deviceDescription;
        delete deviceConfiguration;
        err << QCoreApplication::translate("main", "error (4): invalid output file format, .c, .h, .dcf, .eds, .csv or .tex accepted") << endl;
        return -4;
    }

    delete deviceDescription;
    delete deviceConfiguration;

    return 0;
}
