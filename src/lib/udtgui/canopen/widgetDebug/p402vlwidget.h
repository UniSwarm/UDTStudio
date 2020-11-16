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

#ifndef P402VL_H
#define P402VL_H

#include "../../udtgui_global.h"

#include "node.h"
#include "nodeodsubscriber.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

class NodeProfile402;
class IndexSpinBox;

class P402VlWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P402VlWidget(QWidget *parent = nullptr);
    ~P402VlWidget() override;

    Node *node() const;

    void readData();
signals:

public slots:
    void setNode(Node *value);
    void updateData();

private:
    Node *_node;

    NodeProfile402 *_nodeProfile402;

    NodeObjectId _vlTargetVelocityObjectId;
    NodeObjectId _vlVelocityDemandObjectId;
    NodeObjectId _vlVelocityActualObjectId;

    QButtonGroup *_vlEnableRampButtonGroup;
    QButtonGroup *_vlUnlockRampButtonGroup;
    QButtonGroup *_vlReferenceRampButtonGroup;

    QSpinBox *_vlTargetVelocitySpinBox;
    QSlider *_vlTargetVelocitySlider;
    QLabel *_vlVelocityDemandLabel;
    QLabel *_vlVelocityActualLabel;

    IndexSpinBox *_vlMinVelocityMinMaxAmountSpinBox;
    IndexSpinBox *_vlMaxVelocityMinMaxAmountSpinBox;
    IndexSpinBox *_vlAccelerationDeltaSpeedSpinBox;
    IndexSpinBox *_vlAccelerationDeltaTimeSpinBox;
    IndexSpinBox *_vlDecelerationDeltaSpeedSpinBox;
    IndexSpinBox *_vlDecelerationDeltaTimeSpinBox;
    IndexSpinBox *_vlQuickStopDeltaSpeedSpinBox;
    IndexSpinBox *_vlQuickStopDeltaTimeSpinBox;
    IndexSpinBox *_vlSetPointFactorNumeratorSpinBox;
    IndexSpinBox *_vlSetPointFactorDenominatorSpinBox;
    IndexSpinBox *_vlDimensionFactorNumeratorSpinBox;
    IndexSpinBox *_vlDimensionFactorDenominatorSpinBox;

    void vlTargetVelocitySpinboxFinished();
    void vlTargetVelocitySliderChanged();
    void vlMinVelocityMinMaxAmountSpinboxFinished();
    void vllMaxVelocityMinMaxAmountSpinboxFinished();

    void vlEnableRampClicked(int id);
    void vlUnlockRampClicked(int id);
    void vlReferenceRampClicked(int id);

    void vlEnableRampEvent(bool ok);
    void vlUnlockRampEvent(bool ok);
    void vlReferenceRamp(bool ok);

    void dataLogger();
    void pdoMapping();
    void refreshData(NodeObjectId object);

    void createWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif  // P402VL_H
