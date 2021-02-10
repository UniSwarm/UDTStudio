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

#ifndef P402TQ_H
#define P402TQ_H

#include "../../udtgui_global.h"
#include "node.h"
#include "nodeodsubscriber.h"

#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QScrollArea>

class NodeProfile402;
class IndexSpinBox;
class IndexLabel;

class P402TqWidget : public QScrollArea, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P402TqWidget(QWidget *parent = nullptr);
    ~P402TqWidget() override;

    Node *node() const;

    void readData();
    void reset();

signals:

public slots:
    void setNode(Node *value, uint8_t axis = 0);
    void updateData();

private:
    Node *_node;
    uint8_t _axis;

    NodeProfile402 *_nodeProfile402;

    NodeObjectId _torqueDemandObjectId;
    NodeObjectId _torqueTargetObjectId;
    NodeObjectId _torqueActualValueObjectId;
    NodeObjectId _currentActualValueObjectId;
    NodeObjectId _dcLinkVoltageObjectId;

    QSpinBox *_targetTorqueSpinBox;
    QSlider *_targetTorqueSlider;
    QLabel *_sliderMinLabel;
    QLabel *_sliderMaxLabel;

    IndexLabel *_torqueDemandLabel;
    IndexLabel *_torqueActualValueLabel;
    IndexLabel *_currentActualValueLabel;

    IndexSpinBox *_targetSlopeSpinBox;
    IndexSpinBox *_torqueProfileTypeSpinBox;
    IndexSpinBox *_maxTorqueSpinBox;
    IndexSpinBox *_maxCurrentSpinBox;
    IndexSpinBox *_motorRatedTorqueSpinBox;
    IndexSpinBox *_motorRatedCurrentSpinBox;
    IndexLabel *_dcLinkVoltageLabel;

    void targetTorqueSpinboxFinished();
    void targetTorqueSliderChanged();
    void maxTorqueSpinboxFinished();

    void dataLogger();
    void pdoMapping();

    void createWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402TQ_H
