#ifndef P402VL_H
#define P402VL_H

#include <QWidget>
#include "../../udtgui_global.h"
#include "nodeodsubscriber.h"
#include "node.h"
#include <QGroupBox>
#include <QLabel>
#include <QButtonGroup>
#include <QSpinBox>
#include <QSlider>

class P402VlWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
  public:
    P402VlWidget(QWidget *parent = nullptr);
    ~P402VlWidget() override;

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
    quint16 _controlWordObjectId;
    quint16 _statusWordObjectId;

    // VL mode
    quint16 _vlTargetVelocityObjectId;
    quint16 _vlVelocityDemandObjectId;
    quint16 _vlVelocityActualObjectId;
    quint16 _vlVelocityMinMaxAmountObjectId;
    quint16 _vlAccelerationObjectId;
    quint16 _vlDecelerationObjectId;
    quint16 _vlQuickStopObjectId;
    quint16 _vlSetPointFactorObjectId;
    quint16 _vlDimensionFactorObjectId;

    enum ControlWordVL : quint16
    {
        CW_VL_EnableRamp = 0x10,
        CW_VL_UnlockRamp = 0x20,
        CW_VL_ReferenceRamp = 0x40,
        CW_Halt = 0x100
    };

    QSpinBox *_logTimerSpinBox;
    QAction *_startStopAction;

    QLabel *_controlWordLabel;
    QLabel *_statusWordRawLabel;
    QLabel *_statusWordLabel;
    QLabel *_voltageEnabledLabel;
    QLabel *_warningLabel;
    QLabel *_remoteLabel;
    QLabel *_targetReachedLabel;
    QLabel *_internalLimitActiveLabel;
    QLabel *_operationModeSpecificLabel;
    QLabel *_manufacturerSpecificLabel;

    QButtonGroup *_stateMachineGroup;
    QButtonGroup *_abortConnectionOptionGroup;
    QButtonGroup *_quickStopOptionGroup;
    QButtonGroup *_shutdownOptionGroup;
    QButtonGroup *_disableOptionGroup;
    QButtonGroup *_haltOptionGroup;
    QButtonGroup *_faultReactionOptionGroup;

    // VL MODE
    QButtonGroup *_vlEnableRampButtonGroup;
    QButtonGroup *_vlUnlockRampButtonGroup;
    QButtonGroup *_vlReferenceRampButtonGroup;
    QButtonGroup *_vlHaltButtonGroup;
    QSpinBox *_vlTargetVelocitySpinBox;
    QSlider *_vlTargetVelocitySlider;
    QLabel *_vlVelocityDemandLabel;
    QLabel *_vlVelocityActualLabel;
    QSpinBox *_vlMinVelocityMinMaxAmountSpinBox;
    QSpinBox *_vlMaxVelocityMinMaxAmountSpinBox;
    QSpinBox *_vlAccelerationDeltaSpeedSpinBox;
    QSpinBox *_vlAccelerationDeltaTimeSpinBox;
    QSpinBox *_vlDecelerationDeltaSpeedSpinBox;
    QSpinBox *_vlDecelerationDeltaTimeSpinBox;
    QSpinBox *_vlQuickStopDeltaSpeedSpinBox;
    QSpinBox *_vlQuickStopDeltaTimeSpinBox;
    QSpinBox *_vlSetPointFactorNumeratorSpinBox;
    QSpinBox *_vlSetPointFactorDenominatorSpinBox;
    QSpinBox *_vlDimensionFactorNumeratorSpinBox;
    QSpinBox *_vlDimensionFactorDenominatorSpinBox;

    void vlTargetVelocitySpinboxFinished();
    void vlTargetVelocitySliderChanged();
    void vlMinAmountEditingFinished();
    void vlMaxAmountEditingFinished();
    void vlAccelerationDeltaSpeedEditingFinished();
    void vlAccelerationDeltaTimeEditingFinished();
    void vlDecelerationDeltaSpeedEditingFinished();
    void vlDecelerationDeltaTimeEditingFinished();
    void vlQuickStopDeltaSpeedEditingFinished();
    void vlQuickStopDeltaTimeEditingFinished();
    void vlSetPointFactorNumeratorEditingFinished();
    void vlSetPointFactorDenominatorEditingFinished();
    void vlDimensionFactorNumeratorEditingFinished();
    void vlDimensionFactorDenominatorEditingFinished();

    void vlEnableRampClicked(int id);
    void vlUnlockRampClicked(int id);
    void vlReferenceRampClicked(int id);
    void vlHaltClicked(int id);
    void dataLogger();
    void pdoMapping();
    void manageNotificationControlWordObject(SDO::FlagsRequest flags);
    void refreshData(quint16 object);

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402VL_H
