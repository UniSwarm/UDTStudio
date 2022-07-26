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

#ifndef P402PPWIDGET_H
#define P402PPWIDGET_H

#include "../../../udtgui_global.h"

#include "p402modewidget.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

class NodeProfile402;
class ModePp;
class IndexSpinBox;
class IndexLabel;
class IndexCheckBox;
class IndexFormLayout;

class UDTGUI_EXPORT P402PpWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402PpWidget(QWidget *parent = nullptr);

private:
    ModePp *_modePp;

    void goOneLineEditFinished();
    void twoOneLineEditFinished();

    void changeSetImmediatelyPointCheckBoxRampClicked(bool ok);
    void changeSetImmediatelyPointEvent(bool ok);

    void absRelCheckBoxRampClicked(bool ok);
    void absRelEvent(bool ok);

    void changeOnSetPointCheckBoxRampClicked(bool ok);
    void changeOnSetPointEvent(bool ok);

    void updateInformationLabel();

    void createActions();

    // Create widgets
    void createWidgets();

    void createTargetWidgets(IndexFormLayout *indexLayout);
    QLineEdit *_targetPositionLineEdit;
    QLineEdit *_goOneLineEdit;
    QLineEdit *_goTwoLineEdit;
    QPushButton *_goOnePushButton;
    QPushButton *_goTwoPushButton;

    void createInformationWidgets(IndexFormLayout *indexLayout);
    QLabel *_infoLabel;
    IndexLabel *_positionDemandValueLabel;
    IndexLabel *_positionActualValueLabel;

    void createLimitWidgets(IndexFormLayout *indexLayout);
    IndexSpinBox *_positionRangeLimitMinSpinBox;
    IndexSpinBox *_positionRangeLimitMaxSpinBox;
    IndexSpinBox *_softwarePositionLimitMinSpinBox;
    IndexSpinBox *_softwarePositionLimitMaxSpinBox;

    void createVelocityWidgets(IndexFormLayout *indexLayout);
    IndexSpinBox *_profileVelocitySpinBox;
    IndexSpinBox *_endVelocitySpinBox;
    IndexSpinBox *_maxProfileVelocitySpinBox;
    IndexSpinBox *_maxMotorSpeedSpinBox;

    void createAccelDeccelWidgets(IndexFormLayout *indexLayout);
    IndexSpinBox *_profileAccelerationSpinBox;
    IndexSpinBox *_maxAccelerationSpinBox;
    IndexSpinBox *_profileDecelerationSpinBox;
    IndexSpinBox *_maxDecelerationSpinBox;
    IndexSpinBox *_quickStopDecelerationSpinBox;

    void createHomePolarityWidgets(IndexFormLayout *indexLayout);
    IndexSpinBox *_homeOffsetSpinBox;
    IndexCheckBox *_polarityCheckBox;

    QGroupBox *createControlWordWidgets();
    QCheckBox *_changeSetImmediatelyPointCheckBox;
    QCheckBox *_absRelCheckBox;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;

    // P402Mode interface
public:
    void setIProfile(NodeProfile402 *nodeProfile402) override;

    void createDataLogger() override;
    void mapDefaultObjects() override;
    void showDiagram() override;
};

#endif  // P402PPWIDGET_H
