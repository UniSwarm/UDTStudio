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

#ifndef P402TQWIDGET_H
#define P402TQWIDGET_H

#include "../../../udtgui_global.h"

#include "p402modewidget.h"

#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

class NodeProfile402;
class IndexSpinBox;
class IndexSlider;
class IndexLabel;
class P402ModeWidget;
class ModeTq;

class UDTGUI_EXPORT P402TqWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402TqWidget(QWidget *parent = nullptr);

private:
    ModeTq *_modeTq;

    void setTargetZero();

    // Buttons actions
    void createDataLogger();
    void mapDefaultObjects();
    void showDiagram();

    // Create widgets
    void createWidgets();
    QFormLayout *_modeLayout;

    void createTargetWidgets();
    IndexSpinBox *_targetTorqueSpinBox;
    IndexSlider *_targetTorqueSlider;
    QLabel *_sliderMinLabel;
    QLabel *_sliderMaxLabel;

    void createInformationWidgets();
    IndexLabel *_torqueDemandLabel;
    IndexLabel *_torqueActualValueLabel;
    // IndexLabel *_currentActualValueLabel;

    void createLimitWidgets();
    IndexSpinBox *_maxTorqueSpinBox;

    // IndexSpinBox *_torqueProfileTypeSpinBox;
    // IndexSpinBox *_maxCurrentSpinBox;
    // IndexSpinBox *_motorRatedTorqueSpinBox;
    // IndexSpinBox *_motorRatedCurrentSpinBox;
    // IndexLabel *_dcLinkVoltageLabel;

    void createSlopeWidgets();
    IndexSpinBox *_targetSlopeSpinBox;

    QHBoxLayout *createButtonWidgets() const;
    void updateMaxTorque();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;

    // P402Mode interface
public:
    void reset() override;
    void setIProfile(NodeProfile402 *nodeProfile402) override;
};

#endif  // P402TQWIDGET_H
