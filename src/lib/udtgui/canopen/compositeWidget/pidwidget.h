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

#ifndef PIDWIDGET_H
#define PIDWIDGET_H

#include "../../udtgui_global.h"

#include "profile/p402/nodeprofile402.h"

#include <QPushButton>
#include <QSpinBox>
#include <QTabWidget>
#include <QTimer>
#include <QWidget>

class Node;
class DataLogger;
class DataLoggerChartsWidget;
class IndexSpinBox;

class UDTGUI_EXPORT PidWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PidWidget(QWidget *parent = nullptr);

    Node *node() const;

    QString title() const;

    enum ModePid
    {
        MODE_PID_NONE,
        MODE_PID_VELOCITY,
        MODE_PID_POSITION,
        MODE_PID_TORQUE,
    };

public slots:
    void setNode(Node *node);
    void setMode(PidWidget::ModePid mode);

private slots:
    void stopMeasurementTimer();

protected:
    void createWidgets();

    Node *_node;
    ModePid _modePid;

    QTabWidget *_tabWidget;

    DataLogger *_dataLogger;
    DataLoggerChartsWidget *_dataLoggerChartsWidget;
    QTimer _measurementTimer;

    IndexSpinBox *_pSpinBox;
    IndexSpinBox *_iSpinBox;
    IndexSpinBox *_dSpinBox;
    IndexSpinBox *_periodSpinBox;
    QSpinBox *_targetSpinBox;
    QSpinBox *_windowSpinBox;

    QPushButton *_goTargetPushButton;
    QPushButton *_savePushButton;

    NodeProfile402 *_nodeProfile402;

    void statusNodeChanged(Node::Status status);

    void goTargetPosition();
    void savePosition();
    void mode402Changed(NodeProfile402::Mode modeNew);
};

#endif  // PIDWIDGET_H
