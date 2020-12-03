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

#ifndef P402IP_H
#define P402IP_H

#include "../../udtgui_global.h"

#include "node.h"
#include "nodeodsubscriber.h"

#include <QCheckBox>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

class NodeProfile402;
class NodeObjectId;
class IndexSpinBox;

class P402IpWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P402IpWidget(QWidget *parent = nullptr);
    ~P402IpWidget() override;

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

    NodeObjectId _ipDataRecordObjectId;
    NodeObjectId _ipBufferClearObjectId;
    NodeObjectId _ipPositionDemandValueObjectId;
    NodeObjectId _ipVelocityActualObjectId;
    NodeObjectId _ipTimePeriodIndexObjectId;
    NodeObjectId _ipTimePeriodUnitsObjectId;
    NodeObjectId _ipPolarityObjectId;

    int _iteratorForSendDataRecord;
    QStringList _listDataRecord;

    QCheckBox *_ipEnableRampCheckBox;

    QLineEdit *_ipDataRecordLineEdit;
    QLabel *_ipPositionDemandValueLabel;

    IndexSpinBox *_ipTimePeriodUnitSpinBox;
    IndexSpinBox *_ipTimePeriodIndexSpinBox;

    QPushButton *_clearBufferPushButton;

    IndexSpinBox *_ipPositionRangelLimitMinSpinBox;
    IndexSpinBox *_ipPositionRangelLimitMaxSpinBox;
    IndexSpinBox *_ipSoftwarePositionLimitMinSpinBox;
    IndexSpinBox *_ipSoftwarePositionLimitMaxSpinBox;
    IndexSpinBox *_ipHomeOffsetSpinBox;
    QSpinBox *_ipPolaritySpinBox;
//    IndexSpinBox *_ipProfileVelocitySpinBox;
//    IndexSpinBox *_ipEndVelocitySpinBox;
    IndexSpinBox *_ipMaxProfileVelocitySpinBox;
    IndexSpinBox *_ipMaxMotorSpeedSpinBox;
//    IndexSpinBox *_ipProfileAccelerationSpinBox;
//    IndexSpinBox *_ipMaxAccelerationSpinBox;
//    IndexSpinBox *_ipProfileDecelerationSpinBox;
//    IndexSpinBox *_ipMaxDecelerationSpinBox;
//    IndexSpinBox *_ipQuickStopDecelerationSpinBox;
    QSpinBox *_targetPositionSpinBox;
    QCheckBox *_relativeTargetpositionSpinBox;
    QSpinBox *_durationSpinBox;
    QPushButton *_goTargetPushButton;
    QPushButton *_stopTargetPushButton;

    QVector<int> _pointSinusoidalVector;
    QTimer _sendPointSinusoidalTimer;

    void ipDataRecordLineEditFinished();
    void ipSendDataRecord();

    void ipPolarityEditingFinished();
    void ipClearBufferClicked();
    void ipEnableRampClicked(int id);
    void enableRampEvent(bool ok);

    void updatePositionDemandLabel(void);

    void goTargetPosition();
    void stopTargetPosition();
    void calculatePointSinusoidalMotionProfile(qint32 initialPosition);
    void sendDataRecordTargetWithPdo();
    void sendDataRecordTargetWithSdo();
    void readActualBufferSize();

    void dataLogger();
    void pdoMapping();
    void refreshData(NodeObjectId object);



    void createWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif  // P402IP_H
