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

#ifndef P402PP_H
#define P402PP_H

#include "../../udtgui_global.h"

#include "node.h"
#include "nodeodsubscriber.h"

#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QScrollArea>

class NodeProfile402;
class ModePp;
class IndexSpinBox;
class IndexLabel;

class P402PpWidget : public QScrollArea, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P402PpWidget(QWidget *parent = nullptr);
    ~P402PpWidget() override;

    Node *node() const;
    void readData();

public slots:
    void setNode(Node *value, uint8_t axis = 0);
    void updateData();
    void stop();

private:
    Node *_node;
    uint8_t _axis;
    CanOpenBus *_bus;
    NodeProfile402 *_nodeProfile402;
    ModePp *_modePp;

    NodeObjectId _targetPositionObjectId;
    NodeObjectId _positionDemandValueObjectId;
    NodeObjectId _positionActualValueObjectId;

    int _iteratorForSendDataRecord;
    QStringList _listDataRecord;

    QCheckBox *_newSetPointCheckBox;
    QCheckBox *_changeSetImmediatelyPointCheckBox;
    QCheckBox *_absRelCheckBox;
    QCheckBox *_changeOnSetPointCheckBox;

    QLineEdit *_targetPositionLineEdit;
    QLabel *_infoLabel;

    IndexLabel *_positionDemandValueLabel;
    IndexLabel *_positionActualValueLabel;

    IndexSpinBox *_timePeriodUnitSpinBox;
    IndexSpinBox *_timePeriodIndexSpinBox;

    IndexSpinBox *_positionRangelLimitMinSpinBox;
    IndexSpinBox *_positionRangelLimitMaxSpinBox;
    IndexSpinBox *_softwarePositionLimitMinSpinBox;
    IndexSpinBox *_softwarePositionLimitMaxSpinBox;
    IndexSpinBox *_homeOffsetSpinBox;
    QSpinBox *_polaritySpinBox;
    IndexSpinBox *_profileVelocitySpinBox;
    IndexSpinBox *_endVelocitySpinBox;
    IndexSpinBox *_maxProfileVelocitySpinBox;
    IndexSpinBox *_maxMotorSpeedSpinBox;
    IndexSpinBox *_profileAccelerationSpinBox;
    IndexSpinBox *_maxAccelerationSpinBox;
    IndexSpinBox *_profileDecelerationSpinBox;
    IndexSpinBox *_maxDecelerationSpinBox;
    IndexSpinBox *_quickStopDecelerationSpinBox;

    QSpinBox *_targetPositionSpinBox;
    QCheckBox *_relativeTargetpositionSpinBox;
    QSpinBox *_durationSpinBox;
    QPushButton *_goTargetPushButton;
    QPushButton *_stopTargetPushButton;

    QVector<int> _pointSinusoidalVector;
    QTimer _sendPointSinusoidalTimer;

    void targetPositionLineEditFinished();
    void sendDataRecord();

    void newSetPointClicked(bool ok);
    void newSetPointEvent(bool ok);

    void changeSetImmediatelyPointCheckBoxRampClicked(bool ok);
    void changeSetImmediatelyPointEvent(bool ok);

    void absRelCheckBoxRampClicked(bool ok);
    void absRelEvent(bool ok);

    void changeOnSetPointCheckBoxRampClicked(bool ok);
    void changeOnSetPointEvent(bool ok);

    void updateInformationLabel(void);

    void dataLogger();
    void pdoMapping();

    void createWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402PP_H
