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
#    define cendl endl
#else
#    define cendl Qt::endl
#endif

/**
 * @brief main
 * @return
 */
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("COOD");
    QCoreApplication::setApplicationVersion("1.0");

    QTextStream out(stdout, QIODevice::WriteOnly);
    QTextStream err(stderr, QIODevice::WriteOnly);

    QCommandLineParser cliParser;
    cliParser.setApplicationDescription(QCoreApplication::translate("cood", "Object dictionary command line interface."));
    cliParser.addHelpOption();
    cliParser.addVersionOption();
    cliParser.addPositionalArgument("file", QCoreApplication::translate("cood", "Object dictionary file (.dcf or .eds)"), "file");

    QCommandLineOption outOption(QStringList() << "o"
                                               << "out",
                                 QCoreApplication::translate("cood", "Output directory or file"),
                                 "out");
    cliParser.addOption(outOption);

    QCommandLineOption nodeIdOption(QStringList() << "n"
                                                  << "nodeid",
                                    QCoreApplication::translate("cood", "CANOpen Node Id"),
                                    "nodeid");
    cliParser.addOption(nodeIdOption);

    QCommandLineOption duplicateOption(QStringList() << "d"
                                                     << "duplicate",
                                       QCoreApplication::translate("cood", "Duplicate profile"),
                                       "duplicate");
    cliParser.addOption(duplicateOption);

    QCommandLineOption configurationOption(QStringList() << "c"
                                                         << "configuration",
                                           QCoreApplication::translate("cood", "Configuration files"),
                                           "configuration");
    cliParser.addOption(configurationOption);

    QCommandLineOption rangeOption(QStringList() << "r"
                                                 << "range",
                                   QCoreApplication::translate("cood", "Partial OD range index (min and max separated with ':')"),
                                   "range");
    cliParser.addOption(rangeOption);

    QCommandLineOption structOption(QStringList() << "s"
                                                  << "structName",
                                    QCoreApplication::translate("cood", "Partial OD struct name (use with range option)"),
                                    "structName");
    cliParser.addOption(structOption);

    cliParser.process(app);

    const QStringList files = cliParser.positionalArguments();
    if (files.isEmpty())
    {
        err << QCoreApplication::translate("cood", "error (1): input file is needed") << cendl;
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
        if (inSuffix == "eds" && outSuffix != "eds" && cliParser.value("range").isEmpty() && cliParser.value("structName").isEmpty())
        {
            nodeid = static_cast<uint8_t>(cliParser.value("nodeid").toUInt());
            if (nodeid == 0 || nodeid > 127)
            {
                err << cliParser.value("nodeid").toUInt() << QCoreApplication::translate("cood", "error (2A): invalid node id, nodeId > 0 && nodeId < 126") << cendl;
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
        if (deviceDescription == nullptr)
        {
            err << QCoreApplication::translate("cood", "error (5): invalid eds file or file does not exist '%1'").arg(inputFile) << cendl;
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
        err << QCoreApplication::translate("cood", "error (3): invalid input file format, .eds or .dcf accepted") << cendl;
        return -3;
    }

    for (int fileId = 1; fileId < files.count(); fileId++)
    {
        if (QFileInfo(files.at(fileId)).suffix() != "eds")
        {
            continue;
        }

        EdsParser parser;

        DeviceDescription *secondDeviceDescription = parser.parse(files.at(fileId));
        if (secondDeviceDescription == nullptr)
        {
            delete deviceDescription;
            delete deviceConfiguration;
            err << QCoreApplication::translate("cood", "error (5): invalid eds file or file does not exist '%1'").arg(files.at(fileId)) << cendl;
            return -5;
        }
        if (deviceDescription != nullptr)
        {
            ODMerger::merge(deviceDescription, secondDeviceDescription);
        }
        ODMerger::merge(deviceConfiguration, secondDeviceDescription);
    }

    QStringList cfgFiles = cliParser.values("configuration");
    for (const QString &cfgFile : qAsConst(cfgFiles))
    {
        if (deviceConfiguration != nullptr)
        {
            ConfigurationApply::apply(deviceConfiguration, cfgFile);
        }
        ConfigurationApply::apply(deviceDescription, cfgFile);
    }

    uint8_t duplicate = 0;
    duplicate = static_cast<uint8_t>(cliParser.value("duplicate").toUInt());
    if (duplicate != 0)
    {
        if (deviceConfiguration != nullptr)
        {
            ProfileDuplicate::duplicate(deviceConfiguration, duplicate);
        }
        ProfileDuplicate::duplicate(deviceDescription, duplicate);
    }

    for (const QString &cfgFile : qAsConst(cfgFiles))
    {
        if (deviceConfiguration != nullptr)
        {
            if (!ConfigurationApply::apply(deviceConfiguration, cfgFile))
            {
                return -6;
            }
        }

        if (!ConfigurationApply::apply(deviceDescription, cfgFile))
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
        bool noError = true;
        QString rangeStr = cliParser.value("range");
        if (rangeStr.isEmpty())
        {
            noError = cgenerator.generateH(deviceConfiguration, outputFile);
        }
        else
        {
            bool ok;
            QStringList rangeList = rangeStr.split(':');
            uint16_t min;
            uint16_t max;
            QString structName = cliParser.value("structName");
            if (rangeList.size() != 2)
            {
                err << QCoreApplication::translate("cood", "error (4): invalid range option value") << cendl;
                return -4;
            }

            min = rangeList[0].toUInt(&ok, 0);
            if (!ok)
            {
                noError = false;
            }
            max = rangeList[1].toUInt(&ok, 0);
            if (!ok)
            {
                noError = false;
            }

            if (structName.isEmpty())
            {
                noError = false;
            }
            if (noError)
            {
                noError = cgenerator.generateHStruct(deviceConfiguration, outputFile, min, max, structName);
            }
        }

        if (!noError)
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
    else if (outSuffix == "eds" && (deviceDescription != nullptr))
    {
        EdsWriter edsWriter;
        edsWriter.write(deviceDescription, outputFile);
    }
    else if (outSuffix == "tex" && (deviceDescription != nullptr))
    {
        TexGenerator texGenerator;
        texGenerator.generate(deviceDescription, outputFile);
    }
    else if (outSuffix == "csv" && (deviceDescription != nullptr))
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
        err << QCoreApplication::translate("cood", "error (4): invalid output file format, .c, .h, .dcf, .eds, .csv or .tex accepted") << cendl;
        return -4;
    }

    delete deviceDescription;
    delete deviceConfiguration;

    return 0;
}
