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

#include "dataloggerwidget.h"

#include "dataloggersingleton.h"

#include <QHBoxLayout>
#include <QSplitter>

DataLoggerWidget::DataLoggerWidget(QWidget *parent)
    : DataLoggerWidget(nullptr, Qt::Horizontal, parent)
{
}

DataLoggerWidget::DataLoggerWidget(DataLogger *dataLogger, Qt::Orientation orientation, QWidget *parent)
    : QWidget(parent),
      _dataLogger(dataLogger)
{
    _type = UserType;

    if (_dataLogger == nullptr)
    {
        _dataLogger = new DataLogger(this);
    }
    createWidgets(orientation);
    DataLoggerSingleton::addLogger(this);
}

DataLoggerWidget::~DataLoggerWidget()
{
    DataLoggerSingleton::removeLogger(this);
}

DataLogger *DataLoggerWidget::dataLogger() const
{
    return _dataLogger;
}

const QString &DataLoggerWidget::title() const
{
    return _title;
}

void DataLoggerWidget::setTitle(const QString &title)
{
    _title = title;
    setWindowTitle(title);
}

DataLoggerWidget::Type DataLoggerWidget::type() const
{
    return _type;
}

void DataLoggerWidget::setType(Type type)
{
    _type = type;
}

void DataLoggerWidget::createWidgets(Qt::Orientation orientation)
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);

    QSplitter *splitter = new QSplitter();
    splitter->setStyleSheet(QStringLiteral("QSplitter {background: #19232D;}"));

    QWidget *widgetLogger = new QWidget();
    QVBoxLayout *layoutLogger = new QVBoxLayout();
    _chartView = new DataLoggerChartsWidget(_dataLogger);

    _dataLoggerManagerWidget = new DataLoggerManagerWidget(_dataLogger);
    _dataLoggerManagerWidget->setChartWidget(_chartView);

    splitter->setOrientation(orientation);
    switch (orientation)
    {
        case Qt::Horizontal:
            splitter->addWidget(_dataLoggerManagerWidget);
            splitter->addWidget(widgetLogger);
            splitter->setSizes({70, 130});
            layoutLogger->setContentsMargins(2, 2, 2, 2);
            _dataLoggerManagerWidget->layout()->setContentsMargins(2, 2, 2, 2);
            break;

        case Qt::Vertical:
            splitter->addWidget(widgetLogger);
            splitter->addWidget(_dataLoggerManagerWidget);
            splitter->setSizes({200, 50});
            layoutLogger->setContentsMargins(0, 0, 0, 4);
            _dataLoggerManagerWidget->layout()->setContentsMargins(0, 4, 0, 0);
            break;
    }
    layoutLogger->addWidget(_chartView);
    widgetLogger->setLayout(layoutLogger);

    layout->addWidget(splitter);
    setLayout(layout);
}

DataLoggerChartsWidget *DataLoggerWidget::chartView() const
{
    return _chartView;
}

DataLoggerManagerWidget *DataLoggerWidget::managerWidget() const
{
    return _dataLoggerManagerWidget;
}
