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

#ifndef P402VL_H
#define P402VL_H

#include "../../../udtgui_global.h"

#include "node.h"
#include "nodeodsubscriber.h"
#include "p402mode.h"

#include <QCheckBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QGroupBox>
#include <QFormLayout>

class NodeProfile402;
class ModeVl;
class IndexSpinBox;
class IndexLabel;

class UDTGUI_EXPORT P402VlWidget : public P402Mode
{
    Q_OBJECT
public:
    P402VlWidget(QWidget *parent = nullptr);
    ~P402VlWidget() override;

private:
    NodeProfile402 *_nodeProfile402;
    ModeVl *_modeVl;

    NodeObjectId _velocityTargetObjectId;
    NodeObjectId _velocityDemandObjectId;
    NodeObjectId _velocityActualObjectId;
    NodeObjectId _maxVelocityMinMaxAmountObjectId;

    QSpinBox *_targetVelocitySpinBox;
    QSlider *_targetVelocitySlider;
    QLabel *_sliderMinLabel;
    QLabel *_sliderMaxLabel;
    QLabel *_infoLabel;

    IndexLabel *_velocityDemandLabel;
    IndexLabel *_velocityActualLabel;

    IndexSpinBox *_minVelocityMinMaxAmountSpinBox;
    IndexSpinBox *_maxVelocityMinMaxAmountSpinBox;
    IndexSpinBox *_accelerationDeltaSpeedSpinBox;
    IndexSpinBox *_accelerationDeltaTimeSpinBox;
    IndexSpinBox *_decelerationDeltaSpeedSpinBox;
    IndexSpinBox *_decelerationDeltaTimeSpinBox;
    IndexSpinBox *_quickStopDeltaSpeedSpinBox;
    IndexSpinBox *_quickStopDeltaTimeSpinBox;
    IndexSpinBox *_setPointFactorNumeratorSpinBox;
    IndexSpinBox *_setPointFactorDenominatorSpinBox;
    IndexSpinBox *_dimensionFactorNumeratorSpinBox;
    IndexSpinBox *_dimensionFactorDenominatorSpinBox;

    QCheckBox *_enableRampCheckBox;
    QCheckBox *_unlockRampCheckBox;
    QCheckBox *_referenceRampCheckBox;

    void targetVelocitySpinboxFinished();
    void targetVelocitySliderChanged();
    void updateMaxVelocityMinMaxAmount();
    void setZeroButton();

    void enableRampClicked(bool ok);
    void unlockRampClicked(bool ok);
    void referenceRampClicked(bool ok);

    void enableRampEvent(bool ok);
    void unlockRampEvent(bool ok);
    void referenceRamp(bool ok);

    void updateInformationLabel();

    void dataLogger();
    void pdoMapping();

    // Create widgets
    QFormLayout *_modeLayout;
    void createWidgets();
    void targetWidgets();
    void informationWidgets();
    void limitWidgets();
    void accelDeccelWidgets();
    void factorWidgets();
    QGroupBox *controlWordWidgets();
    QHBoxLayout *buttonWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;

    // P402Mode interface
public:
    void readRealTimeObjects() override;
    void readAllObjects() override;
    void reset() override;

public slots:
    void setNode(Node *value, uint8_t axis) override;
};

#endif // P402VL_H
