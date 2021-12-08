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

#ifndef P402CPWIDGET_H
#define P402CPWIDGET_H

#include "../../../udtgui_global.h"

#include "p402modewidget.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

class NodeProfile402;
class ModeCp;
class IndexSpinBox;
class IndexLabel;
class IndexCheckBox;

class UDTGUI_EXPORT P402CpWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402CpWidget(QWidget *parent = nullptr);

private:
    ModeCp *_modeCp;

    NodeObjectId _positionTargetObjectId;
    NodeObjectId _positionDemandValueObjectId;
    NodeObjectId _positionActualValueObjectId;

    void goOneLineEditFinished();
    void twoOneLineEditFinished();

    void sendAbsRel(bool ok);
    void absRelEvent(bool ok);

    void createDataLogger();
    void mapDefaultObjects();

    // Create widgets
    QFormLayout *_modeLayout;
    void createWidgets();

    void createTargetWidgets();
    QLineEdit *_targetPositionLineEdit;
    QLineEdit *_goOneLineEdit;
    QLineEdit *_goTwoLineEdit;
    QPushButton *_goOnePushButton;
    QPushButton *_goTwoPushButton;

    void createInformationWidgets();
    QLabel *_infoLabel;
    IndexLabel *_positionDemandValueLabel;
    IndexLabel *_positionActualValueLabel;

    void createLimitWidgets();
    IndexSpinBox *_positionRangeLimitMinSpinBox;
    IndexSpinBox *_positionRangeLimitMaxSpinBox;
    IndexSpinBox *_softwarePositionLimitMinSpinBox;
    IndexSpinBox *_softwarePositionLimitMaxSpinBox;

    void createVelocityWidgets();
    IndexSpinBox *_profileVelocitySpinBox;
    IndexSpinBox *_maxProfileVelocitySpinBox;
    IndexSpinBox *_maxMotorSpeedSpinBox;

    void createAccelDeccelWidgets();
    IndexSpinBox *_profileAccelerationSpinBox;
    IndexSpinBox *_maxAccelerationSpinBox;
    IndexSpinBox *_profileDecelerationSpinBox;
    IndexSpinBox *_maxDecelerationSpinBox;
    IndexSpinBox *_quickStopDecelerationSpinBox;

    void createHomePolarityWidgets();
    IndexSpinBox *_homeOffsetSpinBox;
    IndexCheckBox *_polarityCheckBox;

    QGroupBox *createControlWordWidgets();
    QCheckBox *_absRelCheckBox;

    QHBoxLayout *createButtonWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;

    // P402Mode interface
public:
    void readRealTimeObjects() override;
    void readAllObjects() override;

public slots:
    void setNode(Node *node, uint8_t axis) override;
};

#endif  // P402CPWIDGET_H
