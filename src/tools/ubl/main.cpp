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
#include <QCommandLineParser>
#include <QCoreApplication>
#include <QFileInfo>
#include <QTextStream>
#include <QObject>
#include <QDebug>
#include <cstdint>

#include "mainconsole.h"

#include "process/mergeProcess.h"
#include "utility/createbinary.h"
#include "writer/hexwriter.h"

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

    // MERGE
    cliParser.addPositionalArgument("merge", QCoreApplication::translate("main", "-afileA -bfileB -a start:end ... -b start:end ..."), "merge");
    // UPDATE and Flash
    cliParser.addPositionalArgument("update", QCoreApplication::translate("main", "file -n nodeId"), "update");
    // CREATE BIN
    cliParser.addPositionalArgument("create", QCoreApplication::translate("main", "-h file.hex -t type -s start:end ..."), "create");

    // MERGE
    QCommandLineOption outOption(QStringList() << "o" << "out", QCoreApplication::translate("main", "Output directory or file."), "out");
    cliParser.addOption(outOption);

    QCommandLineOption aOption(QStringList() << "a", QCoreApplication::translate("main", "FileHex A and memory segment of FileHex A"), "fileA> -a<start:end");
    cliParser.addOption(aOption);
    QCommandLineOption bOption(QStringList() << "b", QCoreApplication::translate("main", "FileHex B and memory segment of FileHex B"), "fileB> -b<start:end");
    cliParser.addOption(bOption);

    // update and Flash
    QCommandLineOption edsOption(QStringList() << "e" << "eds", QCoreApplication::translate("main", "EDS File"), "eds");
    cliParser.addOption(edsOption);

    QCommandLineOption nodeIdOption(QStringList() << "n" << "nodeid", QCoreApplication::translate("main", "CANOpen Node Id."), "nodeid");
    cliParser.addOption(nodeIdOption);
    QCommandLineOption busOption(QStringList() << "c" << "busId", QCoreApplication::translate("main", "CAN bus."), "busId");
    cliParser.addOption(busOption);
    QCommandLineOption speedOption(QStringList() << "l" << "speed", QCoreApplication::translate("main", "CAN Speed."), "speed");
    cliParser.addOption(speedOption);

    // CREATE BIN
    QCommandLineOption hOption(QStringList() << "f" << "file", QCoreApplication::translate("main", "hex file"), "hex");
    cliParser.addOption(hOption);

    QCommandLineOption typeOption(QStringList() << "t" << "type", QCoreApplication::translate("main", "type device"), "type");
    cliParser.addOption(typeOption);

    QCommandLineOption sOption(QStringList() << "s" << "segment", QCoreApplication::translate("main", "Memory segment of hex file"), "start:end");
    cliParser.addOption(sOption);

    cliParser.process(app);

    const QStringList argument = cliParser.positionalArguments();
    if (argument.isEmpty() || argument.count() > 1)
    {
        err << QCoreApplication::translate("main", "error (1): input file is needed") << Qt::endl;
        cliParser.showHelp(-1);
    }
    else if (argument.at(0) == "merge")
    {
        QStringList aOptionList = cliParser.values(aOption);
        QStringList bOptionList = cliParser.values(bOption);

        if (bOptionList.isEmpty() || aOptionList.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): option -a or -b is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }

        aOptionList.sort();
        bOptionList.sort();
        QString fileA = aOptionList.last();
        QString fileB = aOptionList.last();
        aOptionList.removeLast();
        bOptionList.removeLast();

        if (QFileInfo(fileA).suffix() != "hex" || QFileInfo(fileB).suffix() != "hex")
        {
            err << QCoreApplication::translate("main", "error (1): input file is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }

        // MERGE PROCESS
        MergeProcess mergeProcess(fileA, aOptionList, fileB, bOptionList);
        if (mergeProcess.merge() < 0)
        {
            err << QCoreApplication::translate("main", "error (1): merge process") << Qt::endl;
            cliParser.showHelp(-1);
        }

        QByteArray prog = mergeProcess.prog();

        // OUTPUT file
        QString outputFile = cliParser.value("out");
        if (outputFile.isEmpty())
        {
            outputFile = "merge.hex";
        }

        HexWriter hexWriter;
        hexWriter.write(prog, outputFile);
    }
    else if (argument.at(0) == "create")
    {
        QString type = cliParser.value("type");
        if (type.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): type of device is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }

        QStringList segmentList = cliParser.values(sOption);
        if (segmentList.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): segment is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }

        QString hexFile = cliParser.value(hOption);
        if (hexFile.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): Hex file is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }

        HexParser *hex = new HexParser(hexFile);
        hex->read();

        CreateBinary createBinary;
        createBinary.create(hex->prog(), type, segmentList);

        // Save file
        QString outputFile = cliParser.value("out");
        if (outputFile.isEmpty())
        {
            outputFile = "binary.uni";
        }
        QFile file(outputFile);
        if (!file.open(QFile::WriteOnly))
        {
            err << QCoreApplication::translate("main", "error (1): Hex file is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }
        else
        {
            file.write(createBinary.binary());
            file.close();
        }

    }
    else if (argument.at(0) == "update")
    {
        quint8 nodeid = static_cast<uint8_t>(cliParser.value("nodeid").toUInt());
        if (nodeid == 0 || nodeid >= 126)
        {
            err << QCoreApplication::translate("main", "error (2): invalid node id, nodeId > 0 && nodeId < 126") << Qt::endl;
            return -2;
        }
        quint8 bus = static_cast<uint8_t>(cliParser.value("busId").toUInt());
        if (bus >= 126)
        {
            err << QCoreApplication::translate("main", "error (3): invalid bus id, busId > 0 && busId < 126") << Qt::endl;
            return -3;
        }

        quint8 speed = static_cast<uint8_t>(cliParser.value("speed").toUInt());
        if (speed == 0 || speed >= 126)
        {
            err << QCoreApplication::translate("main", "error (4): invalid speed") << Qt::endl;
            //return -4;
        }

        QString binFile = cliParser.value(hOption);
        if (binFile.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): Binary file is needed") << Qt::endl;
            cliParser.showHelp(-1);
        }

        MainConsole *mainConsole = new MainConsole(bus, speed, nodeid, binFile);
        QObject::connect(mainConsole, &MainConsole::finished, &app, &QCoreApplication::exit);

        return app.exec();
    }
    else
    {
        err << QCoreApplication::translate("main", "error (6): invalid number of hex inputs file, need more than one") << Qt::endl;
        err << QCoreApplication::translate("main", "error (1): input file is needed for update") << Qt::endl;
        cliParser.showHelp(-1);
    }
}
