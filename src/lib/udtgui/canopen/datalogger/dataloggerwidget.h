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

#ifndef DATALOGGERWIDGET_H
#define DATALOGGERWIDGET_H

#include "../../udtgui_global.h"

#include <QWidget>

#include "datalogger/datalogger.h"
#include "dataloggerchartswidget.h"
#include "dataloggermanagerwidget.h"

class UDTGUI_EXPORT DataLoggerWidget : public QWidget
{
    Q_OBJECT
public:
    DataLoggerWidget(QWidget *parent = nullptr);
    DataLoggerWidget(DataLogger *dataLogger, Qt::Orientation orientation = Qt::Horizontal, QWidget *parent = nullptr);
    ~DataLoggerWidget();

    DataLogger *dataLogger() const;

protected:
    DataLogger *_dataLogger;

    void createWidgets(Qt::Orientation orientation);
    DataLoggerManagerWidget *_dataLoggerManagerWidget;
    DataLoggerChartsWidget *_chartView;
};

#endif // DATALOGGERWIDGET_H
