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

#ifndef DATALOGGERCHARTSWIDGET_H
#define DATALOGGERCHARTSWIDGET_H

#include "../../udtgui_global.h"

#include <QChartView>
#include <QLineSeries>

#include "datalogger/datalogger.h"

namespace QtCharts
{
    class QDateTimeAxis;
    class QValueAxis;
}

class UDTGUI_EXPORT DataLoggerChartsWidget : public QtCharts::QChartView
{
    Q_OBJECT
public:
    DataLoggerChartsWidget(DataLogger *dataLogger, QWidget *parent = nullptr);
    ~DataLoggerChartsWidget();

    DataLogger *dataLogger() const;
    void setDataLogger(DataLogger *dataLogger);

protected slots:
    void updateDlData(int id);

    void addDataPrepare(int id);
    void addDataOk();
    void removeDataPrepare(int id);
    void removeDataOk();

private:
    DataLogger *_dataLogger;

    QtCharts::QChart *_chart;
    QList<QtCharts::QXYSeries *> _series;
    QtCharts::QDateTimeAxis *_axisX;
    QtCharts::QValueAxis *_axisY;

    int _idPending;
};

#endif // DATALOGGERCHARTSWIDGET_H
