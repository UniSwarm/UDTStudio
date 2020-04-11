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

#include "dataloggerchartswidget.h"

using namespace QtCharts;

DataLoggerChartsWidget::DataLoggerChartsWidget(DataLogger *dataLogger, QWidget *parent)
    : QChartView(parent)
{
    _dataLogger = nullptr;

    _chart = new QtCharts::QChart();
    _chart->legend()->hide();
    _chart->setTitle("Logger");
    _chart->legend()->setVisible(true);
    _chart->legend()->setAlignment(Qt::AlignBottom);
    //_chart->setTheme(QtCharts::QChart::ChartThemeBlueCerulean);
    setRenderHint(QPainter::Antialiasing);
    setChart(_chart);

    setDataLogger(dataLogger);
}

DataLogger *DataLoggerChartsWidget::dataLogger() const
{
    return _dataLogger;
}

void DataLoggerChartsWidget::setDataLogger(DataLogger *dataLogger)
{
    if (dataLogger != _dataLogger)
    {
        if (dataLogger)
        {
            connect(dataLogger, &DataLogger::dataListChanged, this, &DataLoggerChartsWidget::updateDataLoggerList);
            connect(dataLogger, &DataLogger::dataChanged, this, &DataLoggerChartsWidget::updateDlData);
        }
    }
    _dataLogger = dataLogger;
}

void DataLoggerChartsWidget::updateDataLoggerList()
{
    for (int i = _series.count(); i < _dataLogger->dataList().count(); i++)
    {
        QtCharts::QLineSeries *serie = new QtCharts::QLineSeries();
        serie->setUseOpenGL(true);
        serie->setName(QString("Axe %1").arg(i+1));
        _chart->addSeries(serie);
        _series.append(serie);
    }
    if (!_series.isEmpty())
    {
        _chart->createDefaultAxes();
        _chart->axes(Qt::Vertical).first()->setRange(-180, 180);
        _chart->axes(Qt::Horizontal).first()->setRange(0, 100);
    }
}

void DataLoggerChartsWidget::updateDlData(int id)
{
    if (id < _series.count())
    {
        _time += 0.1;
        _series[id]->append(_time, _dataLogger->data(id)->lastValue());
    }
}
