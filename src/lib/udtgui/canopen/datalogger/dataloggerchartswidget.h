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
#include <QTimer>

#include "datalogger/datalogger.h"

#if QT_VERSION <= QT_VERSION_CHECK(6, 0, 0)
namespace QtCharts
{
class QDateTimeAxis;
class QValueAxis;
}  // namespace QtCharts
#define QChartView QtCharts::QChartView
#define QDateTimeAxis QtCharts::QDateTimeAxis
#define QValueAxis QtCharts::QValueAxis
#else
class QDateTimeAxis;
class QValueAxis;
#endif

class UDTGUI_EXPORT DataLoggerChartsWidget : public QChartView
{
    Q_OBJECT
public:
    DataLoggerChartsWidget(QWidget *parent = nullptr);
    DataLoggerChartsWidget(DataLogger *dataLogger, QWidget *parent = nullptr);
    ~DataLoggerChartsWidget() override;

    DataLogger *dataLogger() const;
    void setDataLogger(DataLogger *dataLogger);

    QChart *chart() const;
    QList<QXYSeries *> series() const;

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
    void updateSeries();
    void updateDlData(int id);

    void addDataPrepare(int id);
    void addDataOk();
    void removeDataPrepare(int id);
    void removeDataOk();

protected:
    void updateYaxis();

private:
    DataLogger *_dataLogger;

    QChart *_chart;
    QDateTimeAxis *_axisX;
    QValueAxis *_axisY;

    QList<QXYSeries *> _series;
    QList<qint64> _serieLastDates;
    QTimer _updateTimer;

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
