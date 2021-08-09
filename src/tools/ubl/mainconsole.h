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

#ifndef MAINCONSOLE_H
#define MAINCONSOLE_H

#include <QObject>

class UpdateProcess;

class MainConsole : public QObject
{
    Q_OBJECT
public:
    MainConsole(quint8 bus, quint8 speed, quint8 nodeid, QString binary);

private slots:
    void nodeConnected(bool connected);

signals:
    void finished(int retcode = 0);

private:
    quint8 _busId;
    quint8 _speed;
    quint8 _nodeId;
    QString _binary;

    UpdateProcess *_updateProcess;
};

#endif // MAINCONSOLE_H
