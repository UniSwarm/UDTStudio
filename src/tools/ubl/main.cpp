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
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QObject>
#include <QProcess>
#include <QStandardPaths>
#include <QTextStream>
#include <cstdint>

#include "mainconsole.h"

#include "bootloader/parser/hexparser.h"
#include "bootloader/utility/hexmerger.h"
#include "bootloader/writer/hexwriter.h"
#include "bootloader/writer/ufwwriter.h"

int hexdump(QString fileA)
{
    if (QFileInfo(fileA).suffix() != "bin" && QFileInfo(fileA).suffix() != "hex")
    {
        return -1;
    }

    QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
#if defined(Q_OS_WIN)
    QChar listSep = ';';
#else
    QChar listSep = ':';
#endif

    QString makePath = QDir::toNativeSeparators("");
    env.insert("PATH", makePath + listSep + env.value("PATH"));

    env.insert("TERM", "xterm");

    QProcess *_process = new QProcess();
    _process->setProcessEnvironment(env);
    QString path = QStandardPaths::findExecutable("hexdump", env.value("PATH").split(listSep));

    QFile fileBinA;
    if (QFileInfo(fileA).suffix() == "hex")
    {
        QString file = fileA + ".bin";
        HexParser hexFileA(fileA);
        hexFileA.read();
        fileBinA.setFileName(file);
        fileBinA.open(QFile::WriteOnly);
        fileBinA.write(hexFileA.prog());
        fileBinA.close();

        _process->start(path,
                        QStringList() << "-C"
                                      << "-v" << QString("%1").arg(file));
        _process->waitForFinished(-1);
    }
    else
    {
        _process->start(path,
                        QStringList() << "-C"
                                      << "-v" << QString("%1").arg(fileA));
        _process->waitForFinished(-1);
    }

    QByteArray stdOut = _process->readAllStandardOutput(); // Reads standard output
    _process->terminate();

    QString fileAHexDump = fileA.split(".").at(0) + ".hexdump";
    fileBinA.setFileName(fileAHexDump);
    fileBinA.open(QFile::WriteOnly);
    fileBinA.write(stdOut);
    fileBinA.close();

    // QDir dir;
    // dir.remove(fileA + ".bin");

    return 0;
}

int diff(QString fileA, QString fileB)
{
    if (QFileInfo(fileA).suffix() != "hex" || QFileInfo(fileB).suffix() != "hex")
    {
        return -1;
    }

    QProcessEnvironment env(QProcessEnvironment::systemEnvironment());
#if defined(Q_OS_WIN)
    QChar listSep = ';';
#else
    QChar listSep = ':';
#endif

    QString makePath = QDir::toNativeSeparators("");
    env.insert("PATH", makePath + listSep + env.value("PATH"));

    env.insert("TERM", "xterm");

    QProcess *_process = new QProcess();
    _process->setProcessEnvironment(env);
    QString path = QStandardPaths::findExecutable("hexdump", env.value("PATH").split(listSep));

    HexParser hexFileA(fileA);
    hexFileA.read();
    QFile fileBinA(fileA + ".bin");
    fileBinA.open(QFile::WriteOnly);
    fileBinA.write(hexFileA.prog());
    fileBinA.close();

    _process->start(path,
                    QStringList() << "-C"
                                  << "-v" << QString("%1").arg(fileA + ".bin"));
    _process->waitForFinished(-1);
    QByteArray stdOut = _process->readAllStandardOutput(); // Reads standard output
    _process->terminate();

    QString fileAHexDump = fileA.split(".").at(0) + ".hexdump";
    fileBinA.setFileName(fileAHexDump);
    fileBinA.open(QFile::WriteOnly);
    fileBinA.write(stdOut);
    fileBinA.close();

    QDir dir;
    dir.remove(fileA + ".bin");

    HexParser hexFileB(fileB);
    hexFileB.read();
    QFile fileBinB(fileB + ".bin");
    fileBinB.open(QFile::WriteOnly);
    fileBinB.write(hexFileB.prog());
    fileBinB.close();

    _process = new QProcess();
    _process->start(path,
                    QStringList() << "-C"
                                  << "-v" << QString("%1").arg(fileB + ".bin"));
    _process->waitForFinished(-1);
    stdOut = _process->readAllStandardOutput(); // Reads standard output
    _process->terminate();

    QString fileBHexDump = fileB.split(".").at(0) + ".hexdump";
    fileBinB.setFileName(fileBHexDump);
    fileBinB.open(QFile::WriteOnly);
    fileBinB.write(stdOut);
    fileBinB.close();

    dir.remove(fileB + ".bin");

    path = QStandardPaths::findExecutable("meld", env.value("PATH").split(listSep));
    _process->start(path, QStringList() << QString("%1").arg(fileAHexDump) << QString("%1").arg(fileBHexDump));
    _process->waitForFinished(-1);

    dir.remove(fileAHexDump);
    dir.remove(fileBHexDump);

    return 0;
}

int merge(QStringList aOptionList, QStringList bOptionList, QString type, QString adress, QString outputFile, QTextStream &err)
{
    if (bOptionList.isEmpty() || aOptionList.isEmpty() || type.isEmpty() || adress.isEmpty())
    {
        err << QCoreApplication::translate("main", "error (1): option -a, -b, -t or -k is needed") << "\n";
        return -1;
    }

    aOptionList.sort();
    bOptionList.sort();
    QString fileA = aOptionList.last();
    QString fileB = bOptionList.last();
    aOptionList.removeLast();
    bOptionList.removeLast();

    if (QFileInfo(fileA).suffix() != "hex" || QFileInfo(fileB).suffix() != "hex")
    {
        err << QCoreApplication::translate("main", "error (1): input file is needed") << "\n";
        return -1;
    }

    // MERGE PROCESS
    HexMerger mergeProcess;
    if (mergeProcess.merge(fileA, aOptionList, fileB, bOptionList) < 0)
    {
        err << QCoreApplication::translate("main", "error (1): merge process") << "\n";
        return -1;
    }
    mergeProcess.setValidProgram(adress, type);

    QByteArray prog = mergeProcess.prog();

    // OUTPUT file
    if (outputFile.isEmpty())
    {
        outputFile = "merge.hex";
    }

    HexWriter hexWriter;
    hexWriter.write(prog, outputFile);
}
int program()
{
}
int update()
{
}

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
    cliParser.addPositionalArgument("program", QCoreApplication::translate("main", "-h file.hex -t type -s start:end ..."), "create");
    // DIFF
    cliParser.addPositionalArgument("diff", QCoreApplication::translate("main", "fileA fileB"), "diff");
    // HEX DUMP
    cliParser.addPositionalArgument("hexdump", QCoreApplication::translate("main", "fileA"), "hexdump");

    // MERGE
    QCommandLineOption outOption(QStringList() << "o"
                                               << "out",
                                 QCoreApplication::translate("main", "Output directory or file."),
                                 "out");
    cliParser.addOption(outOption);

    QCommandLineOption aOption(QStringList() << "a", QCoreApplication::translate("main", "FileHex A and memory segment of FileHex A"), "fileA> -a<start:end");
    cliParser.addOption(aOption);
    QCommandLineOption bOption(QStringList() << "b", QCoreApplication::translate("main", "FileHex B and memory segment of FileHex B"), "fileB> -b<start:end");
    cliParser.addOption(bOption);

    QCommandLineOption kOption(QStringList() << "k", QCoreApplication::translate("main", "Adress of key"), "-kadress");
    cliParser.addOption(kOption);

    // update and Flash
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
    QCommandLineOption busOption(QStringList() << "c"
                                               << "busId",
                                 QCoreApplication::translate("main", "CAN bus."),
                                 "busId");
    cliParser.addOption(busOption);
    QCommandLineOption speedOption(QStringList() << "l"
                                                 << "speed",
                                   QCoreApplication::translate("main", "CAN Speed."),
                                   "speed");
    cliParser.addOption(speedOption);

    // CREATE BIN
    QCommandLineOption hOption(QStringList() << "f"
                                             << "file",
                               QCoreApplication::translate("main", "hex file"),
                               "hex");
    cliParser.addOption(hOption);

    QCommandLineOption typeOption(QStringList() << "t"
                                                << "type",
                                  QCoreApplication::translate("main", "type device"),
                                  "type");
    cliParser.addOption(typeOption);

    QCommandLineOption sOption(QStringList() << "s"
                                             << "segment",
                               QCoreApplication::translate("main", "Memory segment of hex file"),
                               "start:end");
    cliParser.addOption(sOption);

    cliParser.process(app);

    const QStringList argument = cliParser.positionalArguments();

    if (argument.isEmpty())
    {
        err << QCoreApplication::translate("main", "error (1): input file is needed") << "\n";
        cliParser.showHelp(-1);
    }
    else if (argument.at(0) == "hexdump")
    {
        if (argument.size() != 2)
        {
            err << QCoreApplication::translate("main", "error (1): miss file .bin ") << "\n";
            cliParser.showHelp(-1);
        }

        int ret = 0;
        ret = hexdump(argument.at(1));
        if (ret < 0)
        {
            err << QCoreApplication::translate("main", "error (1): miss file .bin ") << "\n";
            cliParser.showHelp(-1);
        }

        exit(0);
    }
    else if (argument.at(0) == "diff")
    {
        int ret = 0;
        ret = diff(argument.at(1), argument.at(2));
        if (ret < 0)
        {
            err << QCoreApplication::translate("main", "error (1): miss file to compare two file") << "\n";
            cliParser.showHelp(-1);
        }

        exit(0);
    }
    else if (argument.at(0) == "merge")
    {
        int ret = 0;
        ret = merge(cliParser.values(aOption), cliParser.values(bOption), cliParser.value(typeOption), cliParser.value(kOption), cliParser.value("out"), err);
        if (ret < 0)
        {
            cliParser.showHelp(-1);
        }
    }
    else if (argument.at(0) == "program")
    {
        QString type = cliParser.value("type");
        if (type.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): type of device is needed") << "\n";
            cliParser.showHelp(-1);
        }

        QStringList segmentList = cliParser.values(sOption);
        if (segmentList.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): segment is needed") << "\n";
            cliParser.showHelp(-1);
        }

        QString hexFile = cliParser.value(hOption);
        if (hexFile.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): Hex file is needed") << "\n";
            cliParser.showHelp(-1);
        }

        HexParser *hex = new HexParser(hexFile);
        hex->read();

        UfwWriter *createBinary = new UfwWriter();
        createBinary->create(hex->prog(), type, segmentList);

        // Save file
        QString outputFile = cliParser.value("out");
        if (outputFile.isEmpty())
        {
            outputFile = "binary.uni";
        }
        QFile file(outputFile);
        if (!file.open(QFile::WriteOnly))
        {
            err << QCoreApplication::translate("main", "error (1): Hex file is needed") << "\n";
            cliParser.showHelp(-1);
        }
        else
        {
            file.write(createBinary->binary());
            file.close();
        }
    }
    else if (argument.at(0) == "update")
    {
        quint8 nodeid = static_cast<uint8_t>(cliParser.value("nodeid").toUInt());
        if (nodeid == 0 || nodeid >= 126)
        {
            err << QCoreApplication::translate("main", "error (2): invalid node id, nodeId > 0 && nodeId < 126") << "\n";
            return -2;
        }
        quint8 bus = static_cast<uint8_t>(cliParser.value("busId").toUInt());
        if (bus >= 126)
        {
            err << QCoreApplication::translate("main", "error (3): invalid bus id, busId > 0 && busId < 126") << "\n";
            return -3;
        }

        quint8 speed = static_cast<uint8_t>(cliParser.value("speed").toUInt());
        if (speed == 0 || speed >= 126)
        {
            // err << QCoreApplication::translate("main", "error (4): invalid speed") << "\n";
            // return -4;
        }

        QString binFile = cliParser.value(hOption);
        if (binFile.isEmpty())
        {
            err << QCoreApplication::translate("main", "error (1): Binary file is needed") << "\n";
            cliParser.showHelp(-1);
        }

        UfwParser *p = new UfwParser(binFile);
        p->read();
        MainConsole *mainConsole = new MainConsole(bus, speed, nodeid, binFile);
        QObject::connect(mainConsole, &MainConsole::finished, &app, &QCoreApplication::exit);

        return app.exec();
    }
    else
    {
        err << QCoreApplication::translate("main", "error (6): invalid number of hex inputs file, need more than one") << "\n";
        err << QCoreApplication::translate("main", "error (1): input file is needed for update") << "\n";
        cliParser.showHelp(-1);
    }
}
