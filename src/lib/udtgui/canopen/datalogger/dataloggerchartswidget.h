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
}  // namespace QtCharts

class UDTGUI_EXPORT DataLoggerChartsWidget : public QtCharts::QChartView
{
    Q_OBJECT
public:
    DataLoggerChartsWidget(DataLogger *dataLogger, QWidget *parent = nullptr);
    ~DataLoggerChartsWidget();

    DataLogger *dataLogger() const;
    void setDataLogger(DataLogger *dataLogger);

    QtCharts::QChart *chart() const;
    QList<QtCharts::QXYSeries *> series() const;

    bool useOpenGL() const;
    bool viewCross() const;

    bool isRollingEnabled() const;
    int rollingTimeMs() const;

public slots:
    void setUseOpenGL(bool useOpenGL);
    void setViewCross(bool viewCross);

    void setRollingEnabled(bool rollingEnabled);
    void setRollingTimeMs(int rollingTimeMs);

    void tooltip(QPointF point, bool state);

signals:
    void useOpenGLChanged(bool);
    void viewCrossChanged(bool);
    void rollingChanged(bool);
    void rollingTimeMsChanged(int);

protected slots:
    void updateDlData(int id);

    void addDataPrepare(int id);
    void addDataOk();
    void removeDataPrepare(int id);
    void removeDataOk();

protected:
    void updateYaxis();

private:
    DataLogger *_dataLogger;

    QtCharts::QChart *_chart;
    QList<QtCharts::QXYSeries *> _series;
    QtCharts::QDateTimeAxis *_axisX;
    QtCharts::QValueAxis *_axisY;

    int _idPending;

    bool _useOpenGL;
    bool _viewCross;

    bool _rollingEnabled;
    int _rollingTimeMs;

    // QWidget interface
protected:
    void dropEvent(QDropEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
};

#endif  // DATALOGGERCHARTSWIDGET_H
