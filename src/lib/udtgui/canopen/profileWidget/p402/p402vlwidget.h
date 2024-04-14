﻿/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#ifndef P402VLWIDGET_H
#define P402VLWIDGET_H

#include "../../../udtgui_global.h"

#include "p402modewidget.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

class NodeProfile402;
class ModeVl;
class IndexSpinBox;
class IndexLabel;
class IndexSlider;
class IndexFormLayout;

class UDTGUI_EXPORT P402VlWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402VlWidget(QWidget *parent = nullptr);

private:
    ModeVl *_modeVl;

    void updateMinMaxVelocity();
    void setTargetZero();

    void enableRampClicked(bool ok);
    void unlockRampClicked(bool ok);
    void referenceRampClicked(bool ok);

    void enableRampEvent(bool ok);
    void unlockRampEvent(bool ok);
    void referenceRamp(bool ok);

    void updateInformationLabel();

    void createActions();

    // Create widgets
    void createWidgets();

    void createTargetWidgets(IndexFormLayout *indexLayout);
    IndexSpinBox *_targetVelocitySpinBox;
    IndexSlider *_targetVelocitySlider;
    QLabel *_sliderMinLabel;
    QLabel *_sliderCenterLabel;
    QLabel *_sliderMaxLabel;

    void createInformationWidgets(IndexFormLayout *indexLayout);
    QLabel *_infoLabel;
    IndexLabel *_velocityDemandLabel;
    IndexLabel *_velocityActualLabel;

    void createLimitWidgets(IndexFormLayout *indexLayout);
    IndexSpinBox *_minVelocityMinMaxAmountSpinBox;
    IndexSpinBox *_maxVelocityMinMaxAmountSpinBox;

    void createAccelDeccelWidgets(IndexFormLayout *indexLayout);
    IndexSpinBox *_accelerationDeltaSpeedSpinBox;
    IndexSpinBox *_accelerationDeltaTimeSpinBox;
    IndexSpinBox *_decelerationDeltaSpeedSpinBox;
    IndexSpinBox *_decelerationDeltaTimeSpinBox;
    IndexSpinBox *_quickStopDeltaSpeedSpinBox;
    IndexSpinBox *_quickStopDeltaTimeSpinBox;

    void createFactorWidgets(IndexFormLayout *indexLayout);
    IndexSpinBox *_setPointFactorNumeratorSpinBox;
    IndexSpinBox *_setPointFactorDenominatorSpinBox;
    IndexSpinBox *_dimensionFactorNumeratorSpinBox;
    IndexSpinBox *_dimensionFactorDenominatorSpinBox;

    QGroupBox *createControlWordWidgets();
    QCheckBox *_enableRampCheckBox;
    QCheckBox *_unlockRampCheckBox;
    QCheckBox *_referenceRampCheckBox;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;

    // P402Mode interface
public:
    void reset() override;
    void setIProfile(NodeProfile402 *nodeProfile402) override;

    void createDataLogger() override;
    void mapDefaultObjects() override;
    void showDiagram() override;
};

#endif  // P402VLWIDGET_H
