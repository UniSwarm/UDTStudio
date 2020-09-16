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

    void createWidgets();

    quint16 _cmdControlWord;
    NodeObjectId _controlWordObjectId;
    NodeObjectId _statusWordObjectId;

    // TQ mode
    NodeObjectId _tqTorqueDemandObjectId;
    NodeObjectId _tqTargetTorqueObjectId;
    NodeObjectId _tqTargetSlopeObjectId;
    NodeObjectId _tqTorqueProfileTypeObjectId;
    NodeObjectId _tqMaxTorqueObjectId;

    NodeObjectId _tqMaxCurrentObjectId;
    NodeObjectId _tqMotorRatedTorqueObjectId;
    NodeObjectId _tqMotorRatedCurrentObjectId;

    NodeObjectId _tqTorqueActualValueObjectId;
    NodeObjectId _tqCurrentActualValueObjectId;
    NodeObjectId _tqDCLinkVoltageObjectId;

    enum ControlWordTQ : quint16
    {
        CW_Halt = 0x100
    };

    // TQ MODE
    QLabel *_tqTorqueDemandLabel;
    QLabel *_tqTorqueActualValueLabel;
    QLabel *_tqCurrentActualValueLabel;
    QLabel *_tqDCLinkVoltageLabel;

    QSpinBox *_tqTargetTorqueSpinBox;
    QSlider *_tqTargetTorqueSlider;
    QSpinBox *_tqTargetSlopeSpinBox;
    QSpinBox *_tqTorqueProfileTypeSpinBox;
    QSpinBox *_tqMaxTorqueSpinBox;

    QSpinBox *_tqMaxCurrentSpinBox;
    QSpinBox *_tqMotorRatedTorqueSpinBox;
    QSpinBox *_tqMotorRatedCurrentSpinBox;

    void tqTargetTorqueSpinboxFinished();
    void tqTargetTorqueSliderChanged();
    void tqTargetSlopeEditingFinished();
    void tqMaxTorqueEditingFinished();
    void tqTorqueProfileTypeEditingFinished();
    void tqMaxCurrentEditingFinished();
    void tqMotorRatedTorqueEditingFinished();
    void tqMotorRatedCurrentEditingFinished();

    void tqHaltClicked(int id);
    void dataLogger();
    void pdoMapping();
    void manageNotificationControlWordObject(SDO::FlagsRequest flags);
    void refreshData(NodeObjectId object);

    // NodeOdSubscriber interface
  protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402TQ_H
