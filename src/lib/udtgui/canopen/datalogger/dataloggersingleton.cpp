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
#include <QStackedWidget>
#include <QTabWidget>

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

QMenu *DataLoggerSingleton::loggersMenu()
{
    return instance()->_loggersMenu;
}

QMenu *DataLoggerSingleton::createAddToLoggerMenu(const NodeObjectId &objId)
{
    return instance()->createAddToLoggerMenu(QList<NodeObjectId>() << objId);
}

QMenu *DataLoggerSingleton::createAddToLoggerMenu(const QList<NodeObjectId> &objIds)
{
    QMenu *menu = new QMenu(tr("Add to logger..."));
    DataLoggerSingleton *instance = DataLoggerSingleton::instance();

    for (DataLoggerWidget *loggerWidget : qAsConst(instance->_dataLoggerWidgets))
    {
        QAction *addAction = nullptr;
        switch (loggerWidget->type())
        {
            case DataLoggerWidget::UserType:
            case DataLoggerWidget::DockType:
                addAction = menu->addAction(loggerWidget->title());
                break;

            case DataLoggerWidget::InternalType:
                break;
        }
        if (addAction != nullptr)
        {
            connect(addAction,
                    &QAction::triggered,
                    instance,
                    [=]()
                    {
                        loggerWidget->dataLogger()->addData(objIds);
                        showWidgetRecursive(loggerWidget);
                    });
        }
    }

    menu->addSeparator();
    QAction *addNewAction = menu->addAction(tr("Add to new logger"));
    connect(addNewAction,
            &QAction::triggered,
            instance,
            [=]()
            {
                DataLoggerWidget *newLogger = instance->createNewLoggerWindow();
                newLogger->dataLogger()->addData(objIds);
            });

    return menu;
}

DataLoggerSingleton::DataLoggerSingleton()
{
    _loggersMenu = new QMenu(tr("Data &loggers"));
    connect(_loggersMenu, &QMenu::aboutToShow, this, &DataLoggerSingleton::updateLoggersMenu);
    _loggerWindowCount = 0;
}

DataLoggerSingleton::~DataLoggerSingleton()
{
    delete _loggersMenu;
}

void DataLoggerSingleton::showWidgetRecursive(QWidget *widget)
{
    if (widget->parentWidget() != nullptr)
    {
        DataLoggerSingleton::showWidgetRecursive(widget->parentWidget());
        QStackedWidget *stackWidget = qobject_cast<QStackedWidget *>(widget->parentWidget());
        if (stackWidget != nullptr)
        {
            QTabWidget *tabWidget = qobject_cast<QTabWidget *>(stackWidget->parentWidget());
            if (tabWidget != nullptr)
            {
                tabWidget->setCurrentWidget(widget);
            }
        }
    }
    widget->raise();
    widget->show();
    widget->activateWindow();
}

void DataLoggerSingleton::updateLoggersMenu()
{
    _loggersMenu->clear();

    QAction *newLoggerAction = _loggersMenu->addAction(tr("Create a new logger window"));
    connect(newLoggerAction, &QAction::triggered, this, &DataLoggerSingleton::createNewLoggerWindow);

    QAction *stopAllAction = _loggersMenu->addAction(tr("Stop all"));
    connect(stopAllAction, &QAction::triggered, this, &DataLoggerSingleton::stopAll);

    _loggersMenu->addSection(tr("Loggers list"));
    for (DataLoggerWidget *loggerWidget : qAsConst(_dataLoggerWidgets))
    {
        QString actionText;
        QAction *showAction = nullptr;
        switch (loggerWidget->type())
        {
            case DataLoggerWidget::UserType:
            case DataLoggerWidget::DockType:
                actionText = loggerWidget->title();
                if (loggerWidget->dataLogger()->isStarted())
                {
                    actionText.append(tr(" (Running)"));
                }
                showAction = _loggersMenu->addAction(actionText);
                // action
                break;

            case DataLoggerWidget::InternalType:
                if (loggerWidget->dataLogger()->isStarted())
                {
                    showAction = _loggersMenu->addAction(loggerWidget->title() + tr(" (Running)"));
                    // action
                }
                break;
        }
        if (showAction != nullptr)
        {
            connect(showAction,
                    &QAction::triggered,
                    this,
                    [=]()
                    {
                        showWidgetRecursive(loggerWidget);
                    });
        }
    }
}

void DataLoggerSingleton::stopAll()
{
    for (DataLoggerWidget *logger : qAsConst(_dataLoggerWidgets))
    {
        logger->dataLogger()->stop();
    }
}

DataLoggerWidget *DataLoggerSingleton::createNewLoggerWindow()
{
    DataLogger *dataLogger = new DataLogger();
    DataLoggerWidget *dataLoggerWidget = new DataLoggerWidget(dataLogger);
    dataLoggerWidget->managerWidget()->setAutoStart(true);
    dataLoggerWidget->setTitle(tr("Logger window %1").arg(_loggerWindowCount++));

    dataLoggerWidget->setAttribute(Qt::WA_DeleteOnClose);
    connect(dataLoggerWidget, &QObject::destroyed, dataLogger, &DataLogger::deleteLater);

    dataLoggerWidget->show();
    dataLoggerWidget->raise();
    dataLoggerWidget->activateWindow();

    return dataLoggerWidget;
}
