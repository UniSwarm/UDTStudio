/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

#include <QHBoxLayout>
#include <QSplitter>

DataLoggerWidget::DataLoggerWidget(QWidget *parent)
    : DataLoggerWidget(nullptr, parent)
{
}

DataLoggerWidget::DataLoggerWidget(DataLogger *dataLogger, QWidget *parent)
    : QWidget(parent), _dataLogger(dataLogger)
{
    if (!_dataLogger)
    {
        _dataLogger = new DataLogger(this);
    }
    createWidgets();
}

DataLogger *DataLoggerWidget::dataLogger() const
{
    return _dataLogger;
}

void DataLoggerWidget::createWidgets()
{
    QLayout *layout = new QHBoxLayout();
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);

    QSplitter *splitter = new QSplitter();

    _dataLoggerManagerWidget = new DataLoggerManagerWidget(_dataLogger);
    splitter->addWidget(_dataLoggerManagerWidget);

    _chartView = new DataLoggerChartsWidget(_dataLogger);
    splitter->addWidget(_chartView);

    _dataLoggerManagerWidget->setChartWidget(_chartView);

    splitter->setSizes({70, 130});
    layout->addWidget(splitter);
    setLayout(layout);
}
