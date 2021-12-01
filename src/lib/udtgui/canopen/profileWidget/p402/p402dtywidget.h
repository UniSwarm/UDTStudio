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

#ifndef P402DCWIDGET_H
#define P402DCWIDGET_H

#include "../../../udtgui_global.h"

#include "p402modewidget.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>

class Node;
class NodeProfile402;
class IndexSpinBox;
class IndexLabel;
class P402ModeWidget;
class ModeDty;

class UDTGUI_EXPORT P402DtyWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402DtyWidget(QWidget *parent = nullptr);
    ~P402DtyWidget() override;

    void setEnableRamp(bool ok);
    bool isEnableRamp(void) const;

private:
    ModeDty *_modeDty;

    NodeObjectId _demandObjectId;
    NodeObjectId _targetObjectId;

    void updateFromSpinbox();
    void updateTargetFromSlider();
    void updateMaxDty();

    void setZeroButton();

    void createDataLogger();
    void mapDefaultObjects();

    // Create widgets
    QFormLayout *_modeLayout;
    void createWidgets();

    void createTargetWidgets();
    QSpinBox *_targetSpinBox;
    QSlider *_targetSlider;
    QLabel *_sliderMinLabel;
    QLabel *_sliderMaxLabel;

    void createDemandWidgets();
    IndexLabel *_demandLabel;

    void createLimitWidgets();
    IndexSpinBox *_maxSpinBox;

    void createSlopeWidgets();
    IndexSpinBox *_slopeSpinBox;

    QGroupBox *createControlWordWidgets();
    QCheckBox *_enableRampCheckBox;

    QHBoxLayout *createButtonWidgets();

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

#endif  // P402DCWIDGET_H
