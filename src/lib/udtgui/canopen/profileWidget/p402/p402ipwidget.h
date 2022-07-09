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

#ifndef P402IPWIDGET_H
#define P402IPWIDGET_H

#include "../../../udtgui_global.h"

#include "p402modewidget.h"

#include <QCheckBox>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>

class NodeProfile402;
class ModeIp;
class IndexSpinBox;
class IndexLabel;
class IndexCheckBox;

class UDTGUI_EXPORT P402IpWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402IpWidget(QWidget *parent = nullptr);

private:
    ModeIp *_modeIp;

    int _iteratorForSendDataRecord;
    QStringList _listDataRecord;

    QVector<int> _pointSinusoidalVector;
    QTimer _sendPointSinusoidalTimer;

    void dataRecordLineEditFinished();
    void sendDataRecord();

    void polarityEditingFinished();
    void bufferClearClicked();
    void enableRampClicked(bool ok);
    void enableRampEvent(bool ok);

    void startTargetPosition();
    void stopTargetPosition();
    void calculatePointSinusoidalMotionProfile(qint32 targetPosition, qint32 initialPosition, qreal periodMs);
    void sendDataRecordTargetWithPdo();
    void sendDataRecordTargetWithSdo();

    void updateInformationLabel();

    // Buttons actions
    void createDataLogger();
    void mapDefaultObjects();
    void showDiagram();

    // Create widgets
    void createWidgets();
    QFormLayout *_modeLayout;

    void createTargetWidgets();
    QLineEdit *_dataRecordLineEdit;
    QPushButton *_clearBufferPushButton;

    void createInformationWidgets();
    IndexLabel *_positionDemandValueLabel;
    IndexLabel *_positionAcualValueLabel;
    QLabel *_infoLabel;

    void createLimitWidgets();
    IndexSpinBox *_positionRangeLimitMinSpinBox;
    IndexSpinBox *_positionRangeLimitMaxSpinBox;
    IndexSpinBox *_softwarePositionLimitMinSpinBox;
    IndexSpinBox *_softwarePositionLimitMaxSpinBox;
    IndexSpinBox *_maxProfileVelocitySpinBox;
    IndexSpinBox *_maxMotorSpeedSpinBox;

    void createSlopeWidgets();
    IndexSpinBox *_timePeriodUnitSpinBox;
    IndexSpinBox *_timePeriodIndexSpinBox;

    void createHomePolarityWidgets();
    IndexSpinBox *_homeOffsetSpinBox;
    IndexCheckBox *_polarityCheckBox;

    QGroupBox *createSinusoidalMotionProfileWidgets();
    QSpinBox *_targetPositionSpinBox;
    QCheckBox *_relativeTargetpositionSpinBox;
    QSpinBox *_durationSpinBox;
    QPushButton *_goTargetPushButton;
    QPushButton *_stopTargetPushButton;

    QGroupBox *createControlWordWidgets();
    QCheckBox *_enableRampCheckBox;

    QHBoxLayout *createButtonWidgets() const;

    // P402Mode interface
public:
    void readRealTimeObjects() override;
    void readAllObjects() override;
    void stop() override;

public slots:
    void setNode(Node *node, uint8_t axis) override;

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags) override;
};

#endif  // P402IPWIDGET_H
