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

#include <QDateTimeAxis>
#include <QValueAxis>
#include <QDebug>

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

    _axisX = new QtCharts::QDateTimeAxis();
    _axisX->setTickCount(10);
    _axisX->setFormat("hh:mm:ss");
    _axisX->setTitleText("Time");

    _axisY = new QtCharts::QValueAxis();
    _axisY->setLabelFormat("%i");
    _axisY->setTitleText("Sunspots count");

    setDataLogger(dataLogger);
    _idPending = -1;
}

DataLoggerChartsWidget::~DataLoggerChartsWidget()
{
    delete _chart;
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
            connect(dataLogger, &DataLogger::dataAboutToBeAdded, this, &DataLoggerChartsWidget::addDataPrepare);
            connect(dataLogger, &DataLogger::dataAdded, this, &DataLoggerChartsWidget::addDataOk);
            connect(dataLogger, &DataLogger::dataAboutToBeRemoved, this, &DataLoggerChartsWidget::removeDataPrepare);
            connect(dataLogger, &DataLogger::dataRemoved, this, &DataLoggerChartsWidget::removeDataOk);
            connect(dataLogger, &DataLogger::dataChanged, this, &DataLoggerChartsWidget::updateDlData);
        }
    }
    _dataLogger = dataLogger;
}

void DataLoggerChartsWidget::updateDlData(int id)
{
    if (id < _series.count())
    {
        DLData *dlData = _dataLogger->data(id);
        _series[id]->append(dlData->lastDateTime().toMSecsSinceEpoch(), dlData->lastValue());

        _axisY->setRange(_dataLogger->min(), _dataLogger->max());
        _axisX->setRange(_dataLogger->firstDateTime(), _dataLogger->lastDateTime());
    }
}

void DataLoggerChartsWidget::addDataPrepare(int id)
{
    _idPending = id;
}

void DataLoggerChartsWidget::addDataOk()
{
    if (_idPending >= 0 && _idPending < _dataLogger->dataList().count())
    {
        DLData *dlData = _dataLogger->data(_idPending);
        QtCharts::QLineSeries *serie = new QtCharts::QLineSeries();
        serie->setUseOpenGL(false);
        serie->setName(dlData->name());
        _chart->addSeries(serie);

        if (!_chart->axes(Qt::Horizontal).contains(_axisX))
        {
            _chart->addAxis(_axisX, Qt::AlignBottom);
            _chart->addAxis(_axisY, Qt::AlignLeft);
        }

        serie->attachAxis(_axisX);
        serie->attachAxis(_axisY);
        _series.append(serie);
    }
    _idPending = -1;
}

void DataLoggerChartsWidget::removeDataPrepare(int id)
{
    if (id >= 0 && id < _dataLogger->dataList().count())
    {
        QtCharts::QLineSeries *serie = _series.at(id);
        _chart->removeSeries(serie);
        _series.removeOne(serie);
        serie->deleteLater();
    }
}

void DataLoggerChartsWidget::removeDataOk()
{
    _idPending = -1;
}
