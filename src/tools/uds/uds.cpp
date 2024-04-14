/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include <QApplication>
#include <QDebug>
#include <QDir>
#include <QScreen>

#include <parser/edsparser.h>

#include "od/odtreeview.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ODTreeView w;

    if (QApplication::arguments().size() > 1)
    {
        QString fileName = QDir::fromNativeSeparators(QApplication::arguments()[1]);
        EdsParser parser;
        DeviceDescription *deviceDescription = parser.parse(fileName);
        w.setDeviceModel(deviceDescription);
    }
    w.setEditable(true);
    w.show();
    w.resize(QApplication::screens()[0]->size() / 2);
    for (int i = 0; i < 4; i++)
    {
        w.resizeColumnToContents(i);
    }

    return QApplication::exec();
}
