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

#include "dataloggermanagerwidget.h"

#include <QHBoxLayout>

DataLoggerManagerWidget::DataLoggerManagerWidget(DataLogger *logger, QWidget *parent)
    : QWidget(parent), _logger(logger)
{
    createWidgets();
}

void DataLoggerManagerWidget::toggleStartLogger(bool start)
{
    if (start)
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-stop.png"));
        _logger->start(_logTimerSpinBox->value());
    }
    else
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
        _logger->stop();
    }
}

void DataLoggerManagerWidget::setLogTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _logger->start(ms);
    }
}

void DataLoggerManagerWidget::createWidgets()
{
    QAction *action;
    QLayout *layout = new QVBoxLayout();

    // toolbar nmt
    _toolBar = new QToolBar(tr("Data logger commands"));

    // start stop
    _startStopAction = _toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &DataLoggerManagerWidget::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(1000);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setStatusTip(tr("Sets the interval of log timer in ms"));
    _toolBar->addWidget(_logTimerSpinBox);
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),[=](int i){ setLogTimer(i); });

    layout->addWidget(_toolBar);

    _dataLoggerTreeView = new DataLoggerTreeView();
    _dataLoggerTreeView->setDataLogger(_logger);
    layout->addWidget(_dataLoggerTreeView);

    setLayout(layout);
}
