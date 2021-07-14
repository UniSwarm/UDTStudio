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

#include "mainwindow.h"

#include <QApplication>
#include <QCanBus>
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>
#include <thread>

#include <QDebug>
#include <cstdint>

#include "utility/hexmerger.h"
#include "writer/hexwriter.h"

#ifdef Q_OS_UNIX
#   include "busdriver/canbussocketcan.h"
#endif

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();

        return a.exec();
    }

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("UBL");
    QCoreApplication::setApplicationVersion("1.0");
    // app.exec();
    QTextStream out(stdout, QIODevice::WriteOnly);
    QTextStream err(stderr, QIODevice::WriteOnly);

    QCommandLineParser cliParser;
    cliParser.setApplicationDescription(QCoreApplication::translate("main", "UBL."));
    cliParser.addHelpOption();
    cliParser.addVersionOption();
    cliParser.addPositionalArgument("merge", QCoreApplication::translate("main", "fileA file B -m -a start:end ... -b start:end ..."), "merge");

    // MERGE
    QCommandLineOption outOption(QStringList() << "o"
                                               << "out",
                                 QCoreApplication::translate("main", "Output directory or file."),
                                 "out");
    cliParser.addOption(outOption);

    QCommandLineOption aOption(QStringList() << "a", QCoreApplication::translate("main", "Keep memory file A (hexa)"), "start:end");
    cliParser.addOption(aOption);
    QCommandLineOption bOption(QStringList() << "b", QCoreApplication::translate("main", "Keep memory file B (hexa)"), "start:end");
    cliParser.addOption(bOption);

    // DOWNLOAD and Flash
    cliParser.addPositionalArgument("download", QCoreApplication::translate("main", "-d file -e eds"), "download");
    QCommandLineOption downloadOption(QStringList() << "d"
                                                    << "download",
                                      QCoreApplication::translate("main", "Program download"),
                                      "download");
    cliParser.addOption(downloadOption);

    QCommandLineOption edsOption(QStringList() << "e"
                                               << "eds",
                                 QCoreApplication::translate("main", "EDS File"),
                                 "eds");
    cliParser.addOption(edsOption);

    QCommandLineOption nodeIdOption(QStringList() << "n"
                                                  << "nodeid",
                                    QCoreApplication::translate("main", "CANOpen Node Id."),
                                    "nodeid");
    cliParser.addOption(nodeIdOption);

    cliParser.process(app);

    const QStringList files = cliParser.positionalArguments();
    if (files.isEmpty())
    {
        err << QCoreApplication::translate("main", "error (1): input file is needed") << Qt::endl;
        cliParser.showHelp(-1);
    }
    else if (files.size() == 2)
    {
        const QString &aFile = files.at(0);
        const QString &bFile = files.at(1);
        QString aSuffix = QFileInfo(aFile).suffix();
        QString bSuffix = QFileInfo(bFile).suffix();

        if (aSuffix != "hex" || bSuffix != "hex")
        {
            err << QCoreApplication::translate("main", "error (1): input file is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }

        QStringList aOptionList = cliParser.values(aOption);
        QStringList bOptionList = cliParser.values(bOption);

        if (bOptionList.isEmpty() || aOptionList.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): option -a or -b is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }

        HexParser *hexAFile = new HexParser(aFile);
        hexAFile->read();
        HexParser *hexBFile = new HexParser(bFile);
        hexBFile->read();

        HexMerger *merger = new HexMerger();
        int ret = merger->merge(hexAFile->prog(), hexBFile->prog(), aOptionList, bOptionList);
        if (ret < 0)
        {
            err << QCoreApplication::translate("main", "error (1): input file is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }

        QByteArray merge = merger->prog();

        // output file
        QString outputFile = cliParser.value("out");
        if (outputFile.isEmpty())
        {
            outputFile = "merge.hex";
        }

        HexWriter hexWriter;
        hexWriter.write(merge, outputFile);
    }
    else if (files.size() == 1)
    {
        quint8 nodeid = static_cast<uint8_t>(cliParser.value("nodeid").toUInt());
        if (nodeid == 0 || nodeid >= 126)
        {
            err << QCoreApplication::translate("main", "error (2): invalid node id, nodeId > 0 && nodeId < 126") << Qt::endl;
            return -2;
        }

        CanOpenBus *bus = nullptr;
#ifdef Q_OS_UNIX
        bus = new CanOpenBus(new CanBusSocketCAN("can0"));
#endif
        if (bus)
        {
            bus->setBusName("Bus 1");
            CanOpen::addBus(bus);

            bus->exploreBus();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            for (int i = 0; i < bus->nodes().size(); ++i)
            {
                qDebug() << bus->nodes().at(i)->busId() << Qt::endl;
            }
        }
        else
        {
            err << QCoreApplication::translate("main", "error (2): bus not connected") << Qt::endl;
            return -2;
        }

        if (!bus->existNode(nodeid))
        {
            err << QCoreApplication::translate("main", "error (1): node not exist") << Qt::endl;
            cliParser.showHelp(-1);
        }

        ProgramDownload *program = new ProgramDownload(bus->node(nodeid));
        const QString file = files.at(0);
        program->openHex(file);
        program->loadEds(cliParser.value("eds"));

        program->update();

        return app.exec();
    }
    else
    {
        err << QCoreApplication::translate("main", "error (6): invalid number of hex inputs file, need more than one") << Qt::endl;
        err << QCoreApplication::translate("main", "error (1): input file is needed for download") << Qt::endl;
        cliParser.showHelp(-1);
    }
}
