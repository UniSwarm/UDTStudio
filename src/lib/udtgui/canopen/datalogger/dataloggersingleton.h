/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2021 UniSwarm
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

#ifndef DATALOGGERSINGLETON_H
#define DATALOGGERSINGLETON_H

#include "../../udtgui_global.h"

#include <QObject>

#include "dataloggerwidget.h"

#include <QList>

class UDTGUI_EXPORT DataLoggerSingleton : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(DataLoggerSingleton)
public:
    static const QList<DataLoggerWidget *> &dataLoggerWidgets();
    static void addLogger(DataLoggerWidget *logger);
    static void removeLogger(DataLoggerWidget *logger);

    static QMenu *loggersMenu();

    static inline DataLoggerSingleton *instance()
    {
        if (!DataLoggerSingleton::_instance)
        {
            DataLoggerSingleton::_instance = new DataLoggerSingleton();
        }
        return DataLoggerSingleton::_instance;
    }

    static inline void release()
    {
        delete DataLoggerSingleton::_instance;
    }

public slots:
    void stopAll();

signals:
    void listChanged();

protected:
    DataLoggerSingleton();
    ~DataLoggerSingleton();
    QList<DataLoggerWidget *> _dataLoggerWidgets;

    QMenu *_loggersMenu;

    static DataLoggerSingleton *_instance;

protected slots:
    void updateLoggersMenu();
};

#define dataLoggers() (DataLoggerSingleton::instance())

#endif  // DATALOGGERSINGLETON_H
