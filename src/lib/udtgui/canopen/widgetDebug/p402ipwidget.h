#ifndef P402IP_H
#define P402IP_H

#include "../../udtgui_global.h"
#include "node.h"
#include "nodeodsubscriber.h"
#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QCheckBox>
#include <QWidget>

class P402IpWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P402IpWidget(QWidget *parent = nullptr);
    ~P402IpWidget() override;

    Node *node() const;

    void readData();
signals:

public slots:
    void setNode(Node *value);
    void updateData();
    void stop();

private:
    Node *_node;
    CanOpenBus *_bus;

    void createWidgets();

    quint16 _cmdControlWord;
    quint16 _controlWordObjectId;
    quint16 _statusWordObjectId;

    // IP mode
    quint16 _ipDataRecordObjectId;
    quint16 _ipDataConfigurationObjectId;
    quint16 _ipPositionDemandValueObjectId;
    quint16 _ipVelocityActualObjectId;
    quint16 _ipTimePeriodObjectId;
    quint16 _ipPositionRangelLimitObjectId;
    quint16 _ipSoftwarePositionLimitObjectId;

    quint16 _ipHomeOffsetObjectId;
    quint16 _ipPolarityObjectId;

    quint16 _ipProfileVelocityObjectId;
    quint16 _ipEndVelocityObjectId;

    quint16 _ipMaxProfileVelocityObjectId;
    quint16 _ipMaxMotorSpeedObjectId;

    quint16 _ipProfileAccelerationObjectId;
    quint16 _ipMaxAccelerationObjectId;

    quint16 _ipProfileDecelerationObjectId;
    quint16 _ipMaxDecelerationObjectId;

    quint16 _ipQuickStopDecelerationObjectId;

    int _iteratorForSendDataRecord;
    QStringList _listDataRecord;

    enum ControlWordIP : quint16
    {
        CW_IP_EnableRamp = 0x10,
        CW_Halt = 0x100
    };

    // IP MODE
    QButtonGroup *_ipEnableRampButtonGroup;
    QButtonGroup *_ipHaltButtonGroup;

    QLineEdit *_ipDataRecordLineEdit;

    QLabel *_ipPositionDemandValueLabel;

    QSpinBox *_ipTimePeriodUnitSpinBox;
    QSpinBox *_ipTimePeriodIndexSpinBox;

    QPushButton *_clearBufferPushButton;

    QSpinBox *_ipPositionRangelLimitMinSpinBox;
    QSpinBox *_ipPositionRangelLimitMaxSpinBox;

    QSpinBox *_ipSoftwarePositionLimitMinSpinBox;
    QSpinBox *_ipSoftwarePositionLimitMaxSpinBox;

    QSpinBox *_ipHomeOffsetSpinBox;
    QSpinBox *_ipPolaritySpinBox;

    QSpinBox *_ipProfileVelocitySpinBox;
    QSpinBox *_ipEndVelocitySpinBox;

    QSpinBox *_ipMaxProfileVelocitySpinBox;
    QSpinBox *_ipMaxMotorSpeedSpinBox;

    QSpinBox *_ipProfileAccelerationSpinBox;
    QSpinBox *_ipMaxAccelerationSpinBox;

    QSpinBox *_ipProfileDecelerationSpinBox;
    QSpinBox *_ipMaxDecelerationSpinBox;

    QSpinBox *_ipQuickStopDecelerationSpinBox;

    QSpinBox *_targetPositionSpinBox;
    QCheckBox *_relativeTargetpositionSpinBox;
    QSpinBox *_durationSpinBox;
    QPushButton *_goTargetPushButton;
    QPushButton *_stopTargetPushButton;

    QVector<int> _pointSinusoidalVector;
    QTimer _sendPointSinusoidalTimer;

    void goTargetPosition();
    void stopTargetPosition();
    void calculatePointSinusoidalMotionProfile(qint32 initialPosition);
    void sendDataRecordTargetWithPdo();
    void sendDataRecordTargetWithSdo();
    void readActualBufferSize();

    void ipDataRecordLineEditFinished();
    void ipSendDataRecord();

    void ipTargetVelocitySpinboxFinished();

    void ipTimePeriodUnitEditingFinished();
    void ipTimePeriodIndexEditingFinished();

    void ipPositionRangelLimitMinEditingFinished();
    void ipPositionRangelLimitMaxEditingFinished();

    void ipSoftwarePositionLimitMinEditingFinished();
    void ipSoftwarePositionLimitMaxFinished();

    void ipHomeOffsetEditingFinished();
    void ipPolarityEditingFinished();

    void ipProfileVelocityFinished();
    void ipEndVelocityFinished();

    void ipMaxProfileVelocityFinished();
    void ipMaxMotorSpeedFinished();

    void ipProfileAccelerationFinished();
    void ipMaxAccelerationFinished();

    void ipProfileDecelerationFinished();
    void ipMaxDecelerationFinished();

    void ipQuickStopDecelerationFinished();

    void ipClearBufferClicked();

    void ipEnableRampClicked(int id);
    void ipHaltClicked(int id);

    void dataLogger();
    void pdoMapping();
    void manageNotificationControlWordObject(SDO::FlagsRequest flags);
    void refreshData(quint16 object);

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402IP_H
