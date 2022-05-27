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

#ifndef P402CSTCAWIDGET_H
#define P402CSTCAWIDGET_H

#include "../../../udtgui_global.h"

#include "p402modewidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

class NodeProfile402;
class IndexSpinBox;
class IndexLabel;
class IndexCheckBox;
class P402ModeWidget;

class UDTGUI_EXPORT P402CstcaWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402CstcaWidget(QWidget *parent = nullptr);

signals:

private:
    ModeCstca *_modeCstca;

    NodeObjectId _torqueDemandObjectId;
    NodeObjectId _torqueTargetObjectId;
    NodeObjectId _torqueActualValueObjectId;
    NodeObjectId _currentActualValueObjectId;
    NodeObjectId _dcLinkVoltageObjectId;

    void targetTorqueSpinboxFinished();
    void targetTorqueSliderChanged();
    void maxTorqueSpinboxFinished();

    void setZeroButton();

    // Buttons actions
    void createDataLogger();
    void mapDefaultObjects();

    // Create widgets
    void createWidgets();
    QFormLayout *_modeLayout;

    void createTargetWidgets();
    QSpinBox *_targetTorqueSpinBox;
    QSlider *_targetTorqueSlider;
    QLabel *_sliderMinLabel;
    QLabel *_sliderMaxLabel;

    void createAngleWidgets();
    IndexSpinBox *_angleSpinBox;

    void createInformationWidgets();
    IndexLabel *_torqueDemandLabel;
    IndexLabel *_torqueActualValueLabel;
    // IndexLabel *_currentActualValueLabel;

    void createLimitWidgets();
    IndexSpinBox *_maxTorqueSpinBox;

    void createInterpolationWidgets();
    IndexSpinBox *_timePeriodUnitSpinBox;
    IndexSpinBox *_timePeriodIndexSpinBox;

    void createHomePolarityWidgets();
    IndexSpinBox *_homeOffsetSpinBox;
    IndexCheckBox *_polarityCheckBox;

    QHBoxLayout *createButtonWidgets() const;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;

    // P402ModeWidget interface
public:
    void readRealTimeObjects() override;
    void readAllObjects() override;
    void reset() override;

public slots:
    void setNode(Node *node, uint8_t axis) override;
};

#endif  // P402CSTCAWIDGET_H
