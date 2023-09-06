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

#include "dataloggermanagerwidget.h"

#include <QDir>
#include <QHBoxLayout>
#include <QStandardPaths>

DataLoggerManagerWidget::DataLoggerManagerWidget(DataLogger *logger, QWidget *parent)
    : QWidget(parent),
      _logger(logger)
{
    _chartWidget = nullptr;
    createWidgets();

    connect(_logger,
            &DataLogger::dataAdded,
            this,
            [this]()
            {
                if (!_logger->isStarted() && _logger->dataList().count() == 1)
                {
                    _logger->start(_logTimerSpinBox->value());
                }
            });
}

DataLoggerChartsWidget *DataLoggerManagerWidget::chartWidget() const
{
    return _chartWidget;
}

void DataLoggerManagerWidget::setChartWidget(DataLoggerChartsWidget *chartWidget)
{
    _chartWidget = chartWidget;

    connect(_chartWidget,
            &DataLoggerChartsWidget::useOpenGLChanged,
            this,
            [=](bool changed)
            {
                if (changed != _openGLAction->isChecked())
                {
                    _openGLAction->blockSignals(true);
                    _openGLAction->setChecked(changed);
                    _openGLAction->blockSignals(false);
                }
            });
    connect(_chartWidget,
            &DataLoggerChartsWidget::viewCrossChanged,
            this,
            [=](bool changed)
            {
                if (changed != _crossAction->isChecked())
                {
                    _crossAction->blockSignals(true);
                    _crossAction->setChecked(changed);
                    _crossAction->blockSignals(false);
                }
            });
    connect(_chartWidget,
            &DataLoggerChartsWidget::rollingChanged,
            this,
            [=](bool changed)
            {
                if (changed != _rollAction->isChecked())
                {
                    _rollAction->blockSignals(true);
                    _rollAction->setChecked(changed);
                    _rollAction->blockSignals(false);
                }
            });
    connect(_chartWidget,
            &DataLoggerChartsWidget::rollingTimeMsChanged,
            this,
            [=](int timeMs)
            {
                if (timeMs != _rollingTimeSpinBox->value())
                {
                    _rollingTimeSpinBox->blockSignals(true);
                    _rollingTimeSpinBox->setValue(timeMs);
                    _rollingTimeSpinBox->blockSignals(false);
                }
            });
}

bool DataLoggerManagerWidget::autoStart() const
{
    return _autoStart;
}

void DataLoggerManagerWidget::setAutoStart(bool autoStart)
{
    _autoStart = autoStart;
}

QAction *DataLoggerManagerWidget::startStopAction() const
{
    return _startStopAction;
}


void DataLoggerManagerWidget::toggleStartLogger(bool start)
{
    if (start)
    {
        _logger->start(_logTimerSpinBox->value());
    }
    else
    {
        _logger->stop();
    }
}

void DataLoggerManagerWidget::setLogTimerMs(int ms)
{
    if (_startStopAction->isChecked())
    {
        _logger->start(ms);
    }
}

void DataLoggerManagerWidget::setUseOpenGL(bool useOpenGL)
{
    if (_chartWidget != nullptr)
    {
        _chartWidget->setUseOpenGL(useOpenGL);
    }
    _crossAction->setEnabled(!useOpenGL);
}

void DataLoggerManagerWidget::setViewCross(bool viewCross)
{
    if (_chartWidget != nullptr)
    {
        _chartWidget->setViewCross(viewCross);
    }
}

void DataLoggerManagerWidget::setRollingEnabled(bool enabled)
{
    if (_chartWidget != nullptr)
    {
        _chartWidget->setRollingEnabled(enabled);
    }
}

void DataLoggerManagerWidget::setRollingTimeMs(int ms)
{
    if (_chartWidget != nullptr)
    {
        _chartWidget->setRollingTimeMs(ms);
    }
}

void DataLoggerManagerWidget::takeScreenShot()
{
    if (_chartWidget == nullptr)
    {
        return;
    }

    QWidget *parent = parentWidget();
    QPixmap pixmap(parent->size());
    parent->render(&pixmap, QPoint(), QRegion(QRect(QPoint(0, 0), parent->size())));

    QString path = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/UDTStudio/";
    QDir().mkdir(path);
    path += QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    path += "_datalogger_udtstudio.png";
    pixmap.save(path);
}

void DataLoggerManagerWidget::exportAllCSVData()
{
    if (_logger == nullptr)
    {
        return;
    }

    QString path = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/UDTStudio/";
    QDir().mkdir(path);
    path += QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss");
    path += "_data.csv";
    _logger->exportCSVData(path);
}

void DataLoggerManagerWidget::createWidgets()
{
    QAction *action;
    QLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(5);

    // toolbar nmt
    _toolBar = new QToolBar(tr("Data logger commands"));
    _toolBar->setIconSize(QSize(20, 20));

    // start stop
    _startStopAction = _toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    QIcon iconStartStop;
    iconStartStop.addFile(":/icons/img/icons8-stop.png", QSize(), QIcon::Normal, QIcon::On);
    iconStartStop.addFile(":/icons/img/icons8-play.png", QSize(), QIcon::Normal, QIcon::Off);
    _startStopAction->setIcon(iconStartStop);
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &DataLoggerManagerWidget::toggleStartLogger);
    connect(_logger,
            &DataLogger::startChanged,
            this,
            [=](bool changed)
            {
                if (changed != _startStopAction->isChecked())
                {
                    _startStopAction->blockSignals(true);
                    _startStopAction->setChecked(changed);
                    _startStopAction->blockSignals(false);
                }
            });

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setStatusTip(tr("Sets the interval of log timer in ms"));
    _toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox,
            &QSpinBox::editingFinished,
            this,
            [=]()
            {
                setLogTimerMs(_logTimerSpinBox->value());
            });

    // clear
    action = _toolBar->addAction(tr("Clear"));
    action->setIcon(QIcon(":/icons/img/icons8-broom.png"));
    action->setStatusTip(tr("Clear all data"));
    connect(action, &QAction::triggered, _logger, &DataLogger::clear);

    _toolBar->addSeparator();

    // open gl
    _openGLAction = _toolBar->addAction(tr("Open-GL"));
    _openGLAction->setCheckable(true);
    _openGLAction->setChecked(true);
    _openGLAction->setIcon(QIcon(":/icons/img/icons8-speed.png"));
    _openGLAction->setStatusTip(tr("Sets render to open GL for fast rendering"));
    connect(_openGLAction, &QAction::triggered, this, &DataLoggerManagerWidget::setUseOpenGL);

    // linechart
    _crossAction = _toolBar->addAction(tr("Cross"));
    _crossAction->setCheckable(true);
    _crossAction->setEnabled(false);
    _crossAction->setIcon(QIcon(":/icons/img/icons8-line-chart.png"));
    _crossAction->setStatusTip(tr("Adds cross to line chart"));
    connect(_crossAction, &QAction::triggered, this, &DataLoggerManagerWidget::setViewCross);

    _toolBar->addSeparator();

    // rolling
    _rollAction = _toolBar->addAction(tr("Rolling mode"));
    _rollAction->setCheckable(true);
    _rollAction->setEnabled(true);
    _rollAction->setIcon(QIcon(":/icons/img/icons8-video-trimming.png"));
    _rollAction->setStatusTip(tr("Enable the rolling mode"));
    connect(_rollAction, &QAction::triggered, this, &DataLoggerManagerWidget::setRollingEnabled);

    _rollingTimeSpinBox = new QSpinBox();
    _rollingTimeSpinBox->setRange(100, 120000);
    _rollingTimeSpinBox->setValue(1000);
    _rollingTimeSpinBox->setSuffix(" ms");
    _rollingTimeSpinBox->setStatusTip(tr("Sets the rolling mode time in ms"));
    _toolBar->addWidget(_rollingTimeSpinBox);
    connect(_rollingTimeSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            [=](int i)
            {
                setRollingTimeMs(i);
            });

    _toolBar->addSeparator();

    // export CSV
    _exportCSVAction = _toolBar->addAction(tr("Export CSV data"));
    _exportCSVAction->setEnabled(true);
    _exportCSVAction->setIcon(QIcon(":/icons/img/icons8-export.png"));
    _exportCSVAction->setStatusTip(tr("Exports all data entries in '%1' directory as a CSV file")
                                       .arg(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/UDTStudio/"));
    connect(_exportCSVAction, &QAction::triggered, this, &DataLoggerManagerWidget::exportAllCSVData);

    // screenshot
    _screenShotAction = _toolBar->addAction(tr("Screenshot"));
    _screenShotAction->setEnabled(true);
    _screenShotAction->setIcon(QIcon(":/icons/img/icons8-screenshot.png"));
    _screenShotAction->setStatusTip(tr("Takes a screenshot of the full datalogger window in '%1' directory")
                                        .arg(QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/UDTStudio/"));
    connect(_screenShotAction, &QAction::triggered, this, &DataLoggerManagerWidget::takeScreenShot);

    layout->addWidget(_toolBar);

    _dataLoggerTreeView = new DataLoggerTreeView();
    _dataLoggerTreeView->setDataLogger(_logger);
    layout->addWidget(_dataLoggerTreeView);

    setLayout(layout);
}
