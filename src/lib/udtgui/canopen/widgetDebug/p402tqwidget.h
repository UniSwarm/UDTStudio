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

#ifndef P402TQ_H
#define P402TQ_H

#include "../../udtgui_global.h"
#include "node.h"
#include "nodeodsubscriber.h"

#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QSlider>
#include <QSpinBox>
#include <QWidget>

class IndexSpinBox;

class P402TqWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
  public:
    P402TqWidget(QWidget *parent = nullptr);
    ~P402TqWidget() override;

    Node *node() const;

    void readData();
  signals:

  public slots:
    void setNode(Node *value);
    void updateData();

  private:
    Node *_node;

    NodeObjectId _tqTorqueDemandObjectId;
    NodeObjectId _tqTargetTorqueObjectId;
    NodeObjectId _tqTorqueActualValueObjectId;
    NodeObjectId _tqCurrentActualValueObjectId;
    NodeObjectId _tqDCLinkVoltageObjectId;

    QLabel *_tqTorqueDemandLabel;
    QLabel *_tqTorqueActualValueLabel;
    QLabel *_tqCurrentActualValueLabel;
    QLabel *_tqDCLinkVoltageLabel;
    QSpinBox *_tqTargetTorqueSpinBox;
    QSlider *_tqTargetTorqueSlider;
    IndexSpinBox *_tqTargetSlopeSpinBox;
    IndexSpinBox *_tqTorqueProfileTypeSpinBox;
    IndexSpinBox *_tqMaxTorqueSpinBox;
    IndexSpinBox *_tqMaxCurrentSpinBox;
    IndexSpinBox *_tqMotorRatedTorqueSpinBox;
    IndexSpinBox *_tqMotorRatedCurrentSpinBox;

    void tqTargetTorqueSpinboxFinished();
    void tqTargetTorqueSliderChanged();
    void tqMaxTorqueSpinboxFinished();

    void dataLogger();
    void pdoMapping();
    void refreshData(NodeObjectId object);

    void createWidgets();

    // NodeOdSubscriber interface
  protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402TQ_H
