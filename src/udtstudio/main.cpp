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
#include <QTextStream>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setOrganizationName("UniSwarm");
    app.setOrganizationDomain("UniSwarm");
    app.setApplicationName("UDTStudio");

    // apply dark style
    QFile f(":qdarkstyle/style.qss");
    if (f.exists())
    {
        f.open(QFile::ReadOnly | QFile::Text);
        QTextStream ts(&f);
        qApp->setStyleSheet(ts.readAll());

    }

    MainWindow w;
    w.setStyleSheet(" \
        QComboBox::item:checked { \
            height: 12px; \
            border: 1px solid #32414B; \
                margin-top: 0px; \
                margin-bottom: 0px; \
            padding: 4px; \
                padding-left: 0px; \
        }");
    w.show();

    return app.exec();
}
