#ifndef P402OPTION_H
#define P402OPTION_H

#include "../../udtgui_global.h"
#include "node.h"
#include "nodeodsubscriber.h"
#include <QButtonGroup>
#include <QWidget>

class P402OptionWidget : public QWidget, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P402OptionWidget(QWidget *parent = nullptr);
    ~P402OptionWidget() override;

    Node *node() const;

public slots:
    void setNode(Node *value);
    void updateData();

    void abortConnectionOptionClicked(int id);
    void quickStopOptionClicked(int id);
    void shutdownOptionClicked(int id);
    void disableOptionClicked(int id);
    void haltOptionClicked(int id);
    void faultReactionOptionClicked(int id);

private:
    Node *_node;

    quint16 _controlWordObjectId;
    quint16 _statusWordObjectId;
    quint16 _abortConnectionObjectId;
    quint16 _quickStopObjectId;
    quint16 _shutdownObjectId;
    quint16 _disableObjectId;
    quint16 _haltObjectId;
    quint16 _faultReactionObjectId;

    QButtonGroup *_abortConnectionOptionGroup;
    QButtonGroup *_quickStopOptionGroup;
    QButtonGroup *_shutdownOptionGroup;
    QButtonGroup *_disableOptionGroup;
    QButtonGroup *_haltOptionGroup;
    QButtonGroup *_faultReactionOptionGroup;

    void refreshData(quint16 object);

    void createWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402OPTION_H
