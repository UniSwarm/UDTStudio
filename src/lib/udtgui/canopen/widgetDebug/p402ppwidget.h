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
class NodeObjectId;
class IndexSpinBox;

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

    NodeObjectId _ppTargetPositionObjectId;
    NodeObjectId _ppPositionDemandValueObjectId;
    NodeObjectId _ppPositionActualValueObjectId;
    NodeObjectId _ppVelocityActualObjectId;
    NodeObjectId _ppPolarityObjectId;

    int _iteratorForSendDataRecord;
    QStringList _listDataRecord;

    QCheckBox *_ppNewSetPointCheckBox;
    QCheckBox *_ppChangeSetImmediatelyPointCheckBox;
    QCheckBox *_ppChangeOnSetPointCheckBox;

    QLineEdit *_ppTargetPositionLineEdit;
    QLabel *_ppPositionDemandValueLabel;
    QLabel *_ppPositionActualValueLabel;

    IndexSpinBox *_ppTimePeriodUnitSpinBox;
    IndexSpinBox *_ppTimePeriodIndexSpinBox;

    QPushButton *_clearBufferPushButton;

    IndexSpinBox *_ppPositionRangelLimitMinSpinBox;
    IndexSpinBox *_ppPositionRangelLimitMaxSpinBox;
    IndexSpinBox *_ppSoftwarePositionLimitMinSpinBox;
    IndexSpinBox *_ppSoftwarePositionLimitMaxSpinBox;
    IndexSpinBox *_ppHomeOffsetSpinBox;
    QSpinBox *_ppPolaritySpinBox;
    IndexSpinBox *_ppProfileVelocitySpinBox;
    IndexSpinBox *_ppEndVelocitySpinBox;
    IndexSpinBox *_ppMaxProfileVelocitySpinBox;
    IndexSpinBox *_ppMaxMotorSpeedSpinBox;
    IndexSpinBox *_ppProfileAccelerationSpinBox;
    IndexSpinBox *_ppMaxAccelerationSpinBox;
    IndexSpinBox *_ppProfileDecelerationSpinBox;
    IndexSpinBox *_ppMaxDecelerationSpinBox;
    IndexSpinBox *_ppQuickStopDecelerationSpinBox;

    QSpinBox *_targetPositionSpinBox;
    QCheckBox *_relativeTargetpositionSpinBox;
    QSpinBox *_durationSpinBox;
    QPushButton *_goTargetPushButton;
    QPushButton *_stopTargetPushButton;

    QVector<int> _pointSinusoidalVector;
    QTimer _sendPointSinusoidalTimer;

    void ppTargetPositionLineEditFinished();
    void ipSendDataRecord();

    void ipPolarityEditingFinished();

    void ppNewSetPointClicked(int id);
    void newSetPointEvent(bool ok);

    void ppChangeSetImmediatelyPointCheckBoxRampClicked(int id);
    void changeSetImmediatelyPointEvent(bool ok);

    void ppChangeOnSetPointCheckBoxRampClicked(int id);
    void changeOnSetPointEvent(bool ok);

    void updatePositionDemandLabel(void);
    void updatePositionActualLabel(void);

    void dataLogger();
    void pdoMapping();
    void refreshData(NodeObjectId object);

    void createWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402PP_H
