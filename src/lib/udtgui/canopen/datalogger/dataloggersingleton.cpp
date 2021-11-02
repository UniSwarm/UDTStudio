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

#include <QMenu>

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

QMenu *DataLoggerSingleton::loggersMenu()
{
    return instance()->_loggersMenu;
}

DataLoggerSingleton::DataLoggerSingleton()
{
    _loggersMenu = new QMenu(tr("Data loggers"));
    connect(_loggersMenu, &QMenu::aboutToShow, this, &DataLoggerSingleton::updateLoggersMenu);
}

DataLoggerSingleton::~DataLoggerSingleton()
{
    delete _loggersMenu;
}

void DataLoggerSingleton::updateLoggersMenu()
{
    QAction *action;
    QString actionText;

    _loggersMenu->clear();
    action = _loggersMenu->addAction(tr("Stop all"));
    connect(action, &QAction::triggered, this, &DataLoggerSingleton::stopAll);

    _loggersMenu->addSection(tr("Loggers list"));
    for (DataLoggerWidget *loggerWidget : qAsConst(_dataLoggerWidgets))
    {
        switch (loggerWidget->type())
        {
            case DataLoggerWidget::UserType:
            case DataLoggerWidget::DockType:
                actionText = loggerWidget->title();
                if (loggerWidget->dataLogger()->isStarted())
                {
                    actionText.append(tr(" (Running)"));
                }
                action = _loggersMenu->addAction(actionText);
                // action
                break;

            case DataLoggerWidget::InternalType:
                if (loggerWidget->dataLogger()->isStarted())
                {
                    action = _loggersMenu->addAction(loggerWidget->title() + tr(" (Running)"));
                    // action
                }
                break;
        }
    }
}
