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

#include <QCoreApplication>
#include <QCommandLineParser>
#include <QApplication>
#include <QFileInfo>

#include <QTextStream>

#include <cstdint>

#include "utility/hexmerger.h"
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

    QTextStream out(stdout, QIODevice::WriteOnly);
    QTextStream err(stderr, QIODevice::WriteOnly);

    QCommandLineParser cliParser;
    cliParser.setApplicationDescription(QCoreApplication::translate("main", "UBL."));
    cliParser.addHelpOption();
    cliParser.addVersionOption();
    //cliParser.addPositionalArgument("file", QCoreApplication::translate("main", "---"), "file");


    QCommandLineOption mergeOption(QStringList() << "m"
                                                 << "merge",
                                   QCoreApplication::translate("main", "Merge 2 hex files"),
                                   "merge");
    cliParser.addOption(mergeOption);

    QCommandLineOption aOption(QStringList() << "a",
                                   QCoreApplication::translate("main", "Hex file a, with adresses a start:end (hexa)"),
                                   "a");
    cliParser.addOption(aOption);
    QCommandLineOption bOption(QStringList() << "b",
                               QCoreApplication::translate("main", "Hex file b, with adresses b start:end (hexa)"),
                               "b");
    cliParser.addOption(bOption);
    cliParser.process(app);

    const QStringList files = cliParser.positionalArguments();
    if (files.isEmpty())
    {
//        err << QCoreApplication::translate("main", "error (1): input file is needed") << endl;
//        cliParser.showHelp(-1);
    }
//    const QString &inputFile = files.at(0);
//    QString inSuffix = QFileInfo(inputFile).suffix();



    QStringList aOptionList = cliParser.values(aOption);
    QStringList bOptionList = cliParser.values(bOption);

    if ((!aOptionList.isEmpty() && bOptionList.isEmpty()) || (aOptionList.isEmpty() && !bOptionList.isEmpty()))
    {
            err << QCoreApplication::translate("main", "error (1): input file is needed") << Qt::endl;
            cliParser.showHelp(-1);
    }

    //const QString &aFile = aOptionList.at(0);
    const QString aFile("/home/julien/Seafile/myLibrary/2_FW/4_UIO_fw/UIO8AD/build/uio8ad.hex");
    //const QString &bFile = bOptionList.at(0);
    const QString bFile("/home/julien/Seafile/myLibrary/2_FW/4_UIO_fw/UIO8AD/bootloader/build/uio8ad_boot.hex");
    QString aSuffix = QFileInfo(aFile).suffix();
    QString bSuffix = QFileInfo(bFile).suffix();

    if (aSuffix != "hex" || bSuffix != "hex")
    {
        err << QCoreApplication::translate("main", "error (1): input file is needed") << Qt::endl;
        cliParser.showHelp(-1);
    }

    HexParser *hexAFile = new HexParser(aFile);
    hexAFile->read();
    HexParser *hexBFile = new HexParser(bFile);
    hexBFile->read();

    aOptionList.removeFirst();
    bOptionList.removeFirst();
    HexMerger *merger = new HexMerger();
    int ret = merger->merge(hexAFile->prog(), hexBFile->prog(), aOptionList, bOptionList);
    if (ret < 0)
    {
        err << QCoreApplication::translate("main", "error (1): input file is needed") << Qt::endl;
        cliParser.showHelp(-1);
    }

    QByteArray merge = merger->prog();

    HexWriter hexWriter;
    hexWriter.write(merge, "fileMerge.hex");

    QFile file("fileMerge.bin");
    if (!file.open(QFile::WriteOnly))
    {
        return -1;
    }
    else
    {
        file.write(merge);
        file.close();
    }
}
