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

#include <QWidget>

#include "nodeobjectid.h"
#include "profile/nodeprofile402.h"

#include <QPushButton>
#include <QTabWidget>

class Node;
class NodeObjectId;
class NodeProfile402;
class DataLogger;
class DataLoggerChartsWidget;
class IndexSpinBox;
class QSpinBox;

class UDTGUI_EXPORT PidWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PidWidget(QWidget *parent = nullptr);

    Node *node() const;

    QString title() const;

public slots:
    void setNode(Node *node);
    void setMode(NodeProfile402::Mode mode);

private slots:
    void stopMeasurementTimer();

protected:
    void createWidgets();

    Node *_node;
    NodeProfile402::Mode _mode;

    QTabWidget *_tabWidget;

    DataLogger *_dataLogger;
    DataLoggerChartsWidget *_dataLoggerChartsWidget;
    QTimer _measurementTimer;
    NodeObjectId _pidP_ObjId;
    NodeObjectId _pidI_ObjId;
    NodeObjectId _pidD_ObjId;
    NodeObjectId _period_ObjId;
    NodeObjectId _target_ObjId;

    NodeObjectId _pidInputStatus_ObjId;
    NodeObjectId _pidErrorStatus_ObjId;
    NodeObjectId _pidIntegratorStatus_ObjId;
    NodeObjectId _pidOutputStatus_ObjId;

    IndexSpinBox *_pSpinBox;
    IndexSpinBox *_iSpinBox;
    IndexSpinBox *_dSpinBox;
    IndexSpinBox *_periodSpinBox;
    IndexSpinBox *_targetSpinBox;
    QSpinBox *_windowSpinBox;

    QPushButton *_goTargetPushButton;
    QPushButton *_savePushButton;

    NodeProfile402 *_nodeProfile402;

    void goTargetPosition();
    void savePosition();
};

#endif  // PIDWIDGET_H
