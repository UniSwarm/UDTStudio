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

#include "dataloggerchartswidget.h"

#include <QDateTimeAxis>
#include <QDebug>
#include <QMimeData>
#include <QOpenGLWidget>
#include <QScatterSeries>
#include <QValueAxis>
#include <QGraphicsLayout>
#include <qmath.h>
#include <QToolTip>

using namespace QtCharts;

DataLoggerChartsWidget::DataLoggerChartsWidget(DataLogger *dataLogger, QWidget *parent)
    : QChartView(parent)
{
    _dataLogger = nullptr;

    setStyleSheet("QAbstractScrollArea {padding: 0px;}");

    _chart = new QtCharts::QChart();
    _chart->legend()->hide();
    //_chart->setTitle("Logger");
    _chart->legend()->setVisible(true);
    _chart->legend()->setAlignment(Qt::AlignBottom);
    _chart->setTheme(QtCharts::QChart::ChartThemeBlueCerulean);
    setRenderHint(QPainter::Antialiasing);
    _chart->layout()->setContentsMargins(0, 0, 0, 0);
    _chart->setBackgroundBrush(QColor("#19232D"));
    setChart(_chart);

    _axisX = new QtCharts::QDateTimeAxis();
    _axisX->setTickCount(10);
    _axisX->setFormat("hh:mm:ss");
    //_axisX->setTitleText("Time");

    _axisY = new QtCharts::QValueAxis();
    _axisY->setLabelFormat("%i");
    //_axisY->setTitleText("Value");

    setDataLogger(dataLogger);
    _idPending = -1;

    _useOpenGL = false;
    _viewCross = false;
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
        QtCharts::QXYSeries *serie = _series[id];
        if (dlData->values().isEmpty())
        {
            serie->clear();
            return;
        }

        serie->append(dlData->lastDateTime().toMSecsSinceEpoch(), dlData->lastValue());

        if (serie->color() != dlData->color())
        {
            serie->setPen(QPen(dlData->color(), 2));
        }

        qreal min = qFloor(_dataLogger->min());
        qreal max = qCeil(_dataLogger->max());
        qreal border = qMax(qCeil((max - min) * .1), 1);
        qreal range = max - min + 2 * border;
        if (min < _axisY->min() || min + border > _axisY->min() || max > _axisY->max() || max - border < _axisY->max())
        {
            _axisY->setRange(min - border, max + border);
            if (range < 10.0)
            {
                _axisY->setTickCount(range + 1);
            }
            else if (range < 20.0)
            {
                _axisY->setTickCount(range / 2.0 + 1);
            }
        }

        QDateTime firstDateTime = _dataLogger->firstDateTime();
        QDateTime lastDateTime = _dataLogger->lastDateTime();
        if (firstDateTime != _axisX->min() || lastDateTime > _axisX->max())
        {
            qint64 msDiff = firstDateTime.msecsTo(lastDateTime);
            _axisX->setRange(firstDateTime, lastDateTime.addMSecs(msDiff / 5));
        }
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
        serie->setName(dlData->name());
        serie->setPen(QPen(dlData->color(), 2));
        serie->setBrush(QBrush(dlData->color()));
        serie->setPointsVisible(_viewCross);
        serie->setUseOpenGL(_useOpenGL);
        _chart->addSeries(serie);

        if (!_chart->axes(Qt::Horizontal).contains(_axisX))
        {
            _chart->addAxis(_axisX, Qt::AlignBottom);
            _chart->addAxis(_axisY, Qt::AlignLeft);
        }

        serie->attachAxis(_axisX);
        serie->attachAxis(_axisY);
        _series.append(serie);

        connect(serie, &QLineSeries::hovered, this, &DataLoggerChartsWidget::tooltip);
    }
    _idPending = -1;
}

void DataLoggerChartsWidget::removeDataPrepare(int id)
{
    if (id >= 0 && id < _dataLogger->dataList().count())
    {
        QtCharts::QXYSeries *serie = _series.at(id);
        _chart->removeSeries(serie);
        _series.removeOne(serie);
        serie->deleteLater();
    }
}

void DataLoggerChartsWidget::removeDataOk()
{
    _idPending = -1;
}

QList<QtCharts::QXYSeries *> DataLoggerChartsWidget::series() const
{
    return _series;
}

QtCharts::QChart *DataLoggerChartsWidget::chart() const
{
    return _chart;
}

bool DataLoggerChartsWidget::viewCross() const
{
    return _viewCross;
}

void DataLoggerChartsWidget::setViewCross(bool viewCross)
{
    _viewCross = viewCross;
    for (QXYSeries *serie : qAsConst(_series))
    {
        serie->setPointsVisible(viewCross);
    }
}

void DataLoggerChartsWidget::tooltip(QPointF point, bool state)
{
    Q_UNUSED(state)
    QLineSeries *serie = dynamic_cast<QLineSeries *>(sender());

    QToolTip::showText(QCursor::pos(), QString("%1\n%2").arg(serie->name()).arg(point.y()), this, QRect());
}

bool DataLoggerChartsWidget::useOpenGL() const
{
    return _useOpenGL;
}

void DataLoggerChartsWidget::setUseOpenGL(bool useOpenGL)
{
    _useOpenGL = useOpenGL;
    for (QXYSeries *serie : qAsConst(_series))
    {
        serie->setUseOpenGL(useOpenGL);
    }
    QList<QOpenGLWidget *> glWidgets = findChildren<QOpenGLWidget *>();
    for (QOpenGLWidget *glWidget : qAsConst(glWidgets))
    {
        glWidget->update();
    }
    invalidateScene();
    update();
}

void DataLoggerChartsWidget::dropEvent(QDropEvent *event)
{
    QChartView::dropEvent(event);
    if (event->mimeData()->hasFormat("index/subindex"))
    {
        const QStringList &stringListObjId = QString(event->mimeData()->data("index/subindex")).split(':', QString::SkipEmptyParts);
        for (const QString &stringObjId : stringListObjId)
        {
            NodeObjectId objId = NodeObjectId::fromMimeData(stringObjId);
            _dataLogger->addData(objId);
        }
        event->accept();
    }
}

void DataLoggerChartsWidget::dragEnterEvent(QDragEnterEvent *event)
{
    QChartView::dragEnterEvent(event);
    if (event->mimeData()->hasFormat("index/subindex"))
    {
        event->accept();
        event->acceptProposedAction();
    }
}

void DataLoggerChartsWidget::dragMoveEvent(QDragMoveEvent *event)
{
    QChartView::dragMoveEvent(event);
    event->accept();
    event->acceptProposedAction();
}
