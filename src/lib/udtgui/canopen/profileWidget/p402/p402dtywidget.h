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
class IndexSlider;
class IndexLabel;
class P402ModeWidget;
class ModeDty;

class UDTGUI_EXPORT P402DtyWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402DtyWidget(QWidget *parent = nullptr);

    void setEnableRamp(bool ok);
    bool isEnableRamp() const;

private:
    ModeDty *_modeDty;

    void updateMaxDty();
    void setTargetZero();

    // Buttons actions
    void createDataLogger();
    void mapDefaultObjects();
    void showDiagram();

    // Create widgets
    QFormLayout *_modeLayout;
    void createWidgets();

    void createTargetWidgets();
    IndexSpinBox *_targetSpinBox;
    IndexSlider *_targetSlider;
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

    QHBoxLayout *createButtonWidgets() const;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;

    // P402Mode interface
public:
    void readRealTimeObjects() override;
    void readAllObjects() override;
    void reset() override;

public slots:
    void setNode(Node *node, uint8_t axis) override;
};

#endif  // P402DCWIDGET_H
