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

#include "dataloggersingleton.h"

DataLoggerSingleton *DataLoggerSingleton::_instance = nullptr;

const QList<DataLoggerWidget *> &DataLoggerSingleton::dataLoggerWidgets()
{
    return instance()->_dataLoggerWidgets;
}

void DataLoggerSingleton::addLogger(DataLoggerWidget *logger)
{
    instance()->_dataLoggerWidgets.append(logger);
    emit _instance->listChanged();
}

void DataLoggerSingleton::removeLogger(DataLoggerWidget *logger)
{
    if (instance()->_dataLoggerWidgets.removeOne(logger))
    {
        emit _instance->listChanged();
    }
}

void DataLoggerSingleton::stopAll()
{
    for (DataLoggerWidget *logger : qAsConst(_dataLoggerWidgets))
    {
        logger->dataLogger()->stop();
    }
}

DataLoggerSingleton::DataLoggerSingleton()
{
}
