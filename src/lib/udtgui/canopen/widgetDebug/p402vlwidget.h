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

class NodeProfile402Vl;

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

    void createWidgets();

    NodeProfile402Vl *_nodeProfile402Vl;

    // VL mode
    quint16 _vlTargetVelocityObjectId;
    quint16 _vlVelocityDemandObjectId;
    quint16 _vlVelocityActualObjectId;
    quint16 _vlVelocityMinMaxAmountObjectId;
    quint16 _vlAccelerationObjectId;
    quint16 _vlDecelerationObjectId;
    quint16 _vlQuickStopObjectId;
    quint16 _vlSetPointFactorObjectId;
    quint16 _vlDimensionFactorObjectId;

    // VL MODE
    QButtonGroup *_vlEnableRampButtonGroup;
    QButtonGroup *_vlUnlockRampButtonGroup;
    QButtonGroup *_vlReferenceRampButtonGroup;

    QSpinBox *_vlTargetVelocitySpinBox;
    QSlider *_vlTargetVelocitySlider;
    QLabel *_vlVelocityDemandLabel;
    QLabel *_vlVelocityActualLabel;
    QSpinBox *_vlMinVelocityMinMaxAmountSpinBox;
    QSpinBox *_vlMaxVelocityMinMaxAmountSpinBox;
    QSpinBox *_vlAccelerationDeltaSpeedSpinBox;
    QSpinBox *_vlAccelerationDeltaTimeSpinBox;
    QSpinBox *_vlDecelerationDeltaSpeedSpinBox;
    QSpinBox *_vlDecelerationDeltaTimeSpinBox;
    QSpinBox *_vlQuickStopDeltaSpeedSpinBox;
    QSpinBox *_vlQuickStopDeltaTimeSpinBox;
    QSpinBox *_vlSetPointFactorNumeratorSpinBox;
    QSpinBox *_vlSetPointFactorDenominatorSpinBox;
    QSpinBox *_vlDimensionFactorNumeratorSpinBox;
    QSpinBox *_vlDimensionFactorDenominatorSpinBox;

    void vlTargetVelocitySpinboxFinished();
    void vlTargetVelocitySliderChanged();
    void vlMinAmountEditingFinished();
    void vlMaxAmountEditingFinished();
    void vlAccelerationDeltaSpeedEditingFinished();
    void vlAccelerationDeltaTimeEditingFinished();
    void vlDecelerationDeltaSpeedEditingFinished();
    void vlDecelerationDeltaTimeEditingFinished();
    void vlQuickStopDeltaSpeedEditingFinished();
    void vlQuickStopDeltaTimeEditingFinished();
    void vlSetPointFactorNumeratorEditingFinished();
    void vlSetPointFactorDenominatorEditingFinished();
    void vlDimensionFactorNumeratorEditingFinished();
    void vlDimensionFactorDenominatorEditingFinished();

    void vlEnableRampClicked(int id);
    void vlUnlockRampClicked(int id);
    void vlReferenceRampClicked(int id);

    void enableRampEvent(bool ok);
    void unlockRampEvent(bool ok);
    void referenceRamp(bool ok);

    void dataLogger();
    void pdoMapping();
    void manageNotificationControlWordObject(SDO::FlagsRequest flags);
    void refreshData(quint16 object);



    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402VL_H
