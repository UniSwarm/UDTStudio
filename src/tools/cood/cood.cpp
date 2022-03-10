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

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
#   define cendl endl
#else
#   define cendl Qt::endl
#endif

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
    cliParser.addPositionalArgument("file", QCoreApplication::translate("main", "Object dictionary file (.dcf or .eds)"), "file");

    QCommandLineOption outOption(QStringList() << "o"
                                               << "out",
                                 QCoreApplication::translate("main", "Output directory or file"),
                                 "out");
    cliParser.addOption(outOption);

    QCommandLineOption nodeIdOption(QStringList() << "n"
                                                  << "nodeid",
                                    QCoreApplication::translate("main", "CANOpen Node Id"),
                                    "nodeid");
    cliParser.addOption(nodeIdOption);

    QCommandLineOption duplicateOption(QStringList() << "d"
                                                     << "duplicate",
                                       QCoreApplication::translate("main", "Duplicate profile"),
                                       "duplicate");
    cliParser.addOption(duplicateOption);

    QCommandLineOption configurationOption(QStringList() << "c"
                                                         << "configuration",
                                           QCoreApplication::translate("main", "Configuration files"),
                                           "configuration");
    cliParser.addOption(configurationOption);

    cliParser.process(app);

    const QStringList files = cliParser.positionalArguments();
    if (files.isEmpty())
    {
        err << QCoreApplication::translate("main", "error (1): input file is needed") << cendl;
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
            if (nodeid == 0 || nodeid > 127)
            {
                err << QCoreApplication::translate("main", "error (2): invalid node id, nodeId > 0 && nodeId < 126") << cendl;
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
            err << QCoreApplication::translate("main", "error (5): invalid eds file or file does not exist '%1'").arg(inputFile) << cendl;
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
        err << QCoreApplication::translate("main", "error (3): invalid input file format, .eds or .dcf accepted") << cendl;
        return -3;
    }

    for (int fileId = 1; fileId < files.count(); fileId++)
    {
        if (QFileInfo(files.at(fileId)).suffix() != "eds")
        {
            continue;
        }

        EdsParser parser;
        ODMerger merger;

        DeviceDescription *secondDeviceDescription = parser.parse(files.at(fileId));
        if (!secondDeviceDescription)
        {
            delete deviceDescription;
            delete deviceConfiguration;
            err << QCoreApplication::translate("main", "error (5): invalid eds file or file does not exist '%1'").arg(files.at(fileId)) << cendl;
            return -5;
        }
        if (deviceDescription)
        {
            merger.merge(deviceDescription, secondDeviceDescription);
        }
        merger.merge(deviceConfiguration, secondDeviceDescription);
    }

    QStringList cfgFiles = cliParser.values("configuration");
    for (const QString &cfgFile : qAsConst(cfgFiles))
    {
        ConfigurationApply configurationApply;

        if (deviceConfiguration)
        {
            if (!configurationApply.apply(deviceConfiguration, cfgFile))
            {
                return -6;
            }
        }

        if (!configurationApply.apply(deviceDescription, cfgFile))
        {
            return -6;
        }
    }

    /*if (deviceDescription)
    {
        for (Index *index : deviceDescription->indexes())
        {
            for (SubIndex *subIndex : index->subIndexes())
            {
                if (subIndex->accessType() & SubIndex::WRITE
                        && !subIndex->value().isValid())
                {
                    subIndex->setValue(QVariant(0));
                }
            }
        }
    }*/

    // OUTPUT FILE
    if (outSuffix == "c")
    {
        CGenerator cgenerator;
        if (!cgenerator.generateC(deviceConfiguration, outputFile))
        {
            err << cgenerator.errorStr();
            return -4;
        }
    }
    else if (outSuffix == "h")
    {
        CGenerator cgenerator;
        if (!cgenerator.generateH(deviceConfiguration, outputFile))
        {
            err << cgenerator.errorStr();
            return -4;
        }
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
        if (!cgenerator.generateC(deviceConfiguration, QString(outputFile + "/od_data.c")))
        {
            err << cgenerator.errorStr();
            return -4;
        }
        if (!cgenerator.generateH(deviceConfiguration, QString(outputFile + "/od_data.h")))
        {
            err << cgenerator.errorStr();
            return -4;
        }
        dcfWriter.write(deviceConfiguration, QString(outputFile + "/out.dcf"));
    }
    else
    {
        delete deviceDescription;
        delete deviceConfiguration;
        err << QCoreApplication::translate("main", "error (4): invalid output file format, .c, .h, .dcf, .eds, .csv or .tex accepted") << cendl;
        return -4;
    }

    delete deviceDescription;
    delete deviceConfiguration;

    return 0;
}
