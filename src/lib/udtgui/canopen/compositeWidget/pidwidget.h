#ifndef PIDWIDGET_H
#define PIDWIDGET_H

#include <QWidget>
#include "canopen/datalogger/dataloggerwidget.h"
#include <node.h>
#include <QTabWidget>
#include "profile/nodeprofile402.h"
#include "canopen/widget/indexspinbox.h"
#include "QPushButton"

class UDTGUI_EXPORT PidWidget : public QWidget
{
    Q_OBJECT
  public:
    explicit PidWidget(QWidget *parent = nullptr);

    Node *node() const;

    QString title() const;

    void setPObjectId(const NodeObjectId &objId);
    void setIObjectId(const NodeObjectId &objId);
    void setDObjectId(const NodeObjectId &objId);
    void setPeriodObjectId(const NodeObjectId &objId);
    void setTargetObjectId(const NodeObjectId &objId);

  public slots:
    void setNode(Node *node);
    void setMode(NodeProfile402::Mode mode);

  private slots:
    void stopMeasurementTimer();

  protected:
    void createWidgets();

    Node *_node;
    NodeProfile402::Mode _mode;

    QTabWidget *_tabWidget;

    DataLogger *_dataLogger;
    QTimer _measurementTimer;
    NodeObjectId _pidP_ObjId;
    NodeObjectId _pidI_ObjId;
    NodeObjectId _pidD_ObjId;
    NodeObjectId _period_ObjId;
    NodeObjectId _target_ObjId;

    NodeObjectId _pidInputStatus_ObjId;
    NodeObjectId _pidErrorStatus_ObjId;
    NodeObjectId _pidIntegratorStatus_ObjId;
    NodeObjectId _pidOutputStatus_ObjId;

    IndexSpinBox *_pSpinBox;
    IndexSpinBox *_iSpinBox;
    IndexSpinBox *_dSpinBox;
    IndexSpinBox *_periodSpinBox;
    IndexSpinBox *_targetSpinBox;
    QSpinBox *_windowSpinBox;

    QPushButton *_goTargetPushButton;
    QPushButton *_savePushButton;

    void goTargetPosition();
    void savePosition();
};

#endif // PIDWIDGET_H
