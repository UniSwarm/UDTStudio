#include "p402optionwidget.h"
#include <QFormLayout>
#include <QRadioButton>
#include <QGroupBox>

P402OptionWidget::P402OptionWidget(QWidget *parent) : QWidget(parent)
{
    _node = nullptr;
    createWidgets();

    _abortConnectionObjectId = 0x6007;
    _quickStopObjectId = 0x605A;
    _shutdownObjectId = 0x605B;
    _disableObjectId = 0x605C;
    _haltObjectId = 0x605D;
    _faultReactionObjectId = 0x605E;
}

P402OptionWidget::~P402OptionWidget()
{
    unRegisterFullOd();
}

void P402OptionWidget::setNode(Node *node)
{
    if (node != _node)
    {
        if (_node)
        {
            disconnect(_node, &Node::statusChanged, this, &P402OptionWidget::updateData);
        }

    }

    registerObjId({_abortConnectionObjectId, 0x00});
    registerObjId({_quickStopObjectId, 0x00});
    registerObjId({_shutdownObjectId, 0x00});
    registerObjId({_disableObjectId, 0x00});
    registerObjId({_haltObjectId, 0x00});
    registerObjId({_faultReactionObjectId, 0x00});

    setNodeInterrest(node);

    _node = node;
    if (_node)
    {
        connect(_node, &Node::statusChanged, this, &P402OptionWidget::updateData);
        updateData();
    }
}

Node *P402OptionWidget::node() const
{
    return _node;
}

void P402OptionWidget::updateData()
{
    if (_node)
    {
        if (_node->status() == Node::STARTED)
        {
            this->setEnabled(true);
            _node->readObject(_abortConnectionObjectId, 0x0);
            _node->readObject(_quickStopObjectId, 0x0);
            _node->readObject(_shutdownObjectId, 0x0);
            _node->readObject(_disableObjectId, 0x0);
            _node->readObject(_haltObjectId, 0x0);
            _node->readObject(_faultReactionObjectId, 0x0);
        }
        else
        {
            // this->setEnabled(false);
        }
    }
}

void P402OptionWidget::abortConnectionOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_abortConnectionObjectId, 0x00, QVariant(value));
}

void P402OptionWidget::quickStopOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_quickStopObjectId, 0x00, QVariant(value));
}

void P402OptionWidget::shutdownOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_shutdownObjectId, 0x00, QVariant(value));
}

void P402OptionWidget::disableOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_disableObjectId, 0x00, QVariant(value));
}

void P402OptionWidget::haltOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_haltObjectId, 0x00, QVariant(value));
}

void P402OptionWidget::faultReactionOptionClicked(int id)
{
    if (!_node)
    {
        return;
    }
    quint16 value = static_cast<quint16>(id);
    _node->writeObject(_faultReactionObjectId, 0x00, QVariant(value));
}

void P402OptionWidget::refreshData(quint16 object)
{
    if (_node->nodeOd()->indexExist(object))
    {
        int value = _node->nodeOd()->value(object).toInt();

        // Abort connection option code
        if (object == _abortConnectionObjectId)
        {
            _abortConnectionOptionGroup->button(value)->setChecked(true);
        }

        // Quick stop option code
        if (object == _quickStopObjectId)
        {
            _quickStopOptionGroup->button(value)->setChecked(true);
        }

        // Shutdown option code
        if (object == _shutdownObjectId)
        {
            _shutdownOptionGroup->button(value)->setChecked(true);
        }

        // Disable operation option code
        if (object == _disableObjectId)
        {
            _disableOptionGroup->button(value)->setChecked(true);
        }

        // Halt option code
        if (object == _haltObjectId)
        {
            _haltOptionGroup->button(value)->setChecked(true);
        }

        // Fault reaction option code
        if (object == _faultReactionObjectId)
        {
            _faultReactionOptionGroup->button(value)->setChecked(true);
        }
    }
}

void P402OptionWidget::createWidgets()
{
     // SECOND COLUMM
    QLayout *layout = new QVBoxLayout();
    layout->setMargin(0);

    // Group Box Abort connection option
    QGroupBox *abortConnectionOptionGroupBox = new QGroupBox(tr("Abort connection option (0x6007)"));
    QFormLayout *abortConnectionOptionLayout = new QFormLayout();
    _abortConnectionOptionGroup = new QButtonGroup(this);
    _abortConnectionOptionGroup->setExclusive(true);
    QRadioButton *_0AbortConnectionOptionCheckBox = new QRadioButton(tr("No action"));
    abortConnectionOptionLayout->addRow(_0AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_0AbortConnectionOptionCheckBox, 0);
    QRadioButton *_1AbortConnectionOptionCheckBox = new QRadioButton(tr("Fault signal"));
    abortConnectionOptionLayout->addRow(_1AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_1AbortConnectionOptionCheckBox, 1);
    QRadioButton *_2AbortConnectionOptionCheckBox = new QRadioButton(tr("Disable voltage command"));
    abortConnectionOptionLayout->addRow(_2AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_2AbortConnectionOptionCheckBox, 2);
    QRadioButton *_3AbortConnectionOptionCheckBox = new QRadioButton(tr("Quick stop command"));
    abortConnectionOptionLayout->addRow(_3AbortConnectionOptionCheckBox);
    _abortConnectionOptionGroup->addButton(_3AbortConnectionOptionCheckBox, 3);
    abortConnectionOptionGroupBox->setLayout(abortConnectionOptionLayout);
    connect(_abortConnectionOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { abortConnectionOptionClicked(id); });
    // END Group Box Abort connection option

    // Group Box Quick stop option
    QGroupBox *quickStopOptionGroupBox = new QGroupBox(tr("Quick stop option (0x605A)"));
    QFormLayout *quickStopOptionLayout = new QFormLayout();
    _quickStopOptionGroup = new QButtonGroup(this);
    _quickStopOptionGroup->setExclusive(true);
    QRadioButton *_0QuickStopOptionCheckBox = new QRadioButton(tr("Disable drive function"));
    quickStopOptionLayout->addRow(_0QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_0QuickStopOptionCheckBox, 0);
    QRadioButton *_1QuickStopOptionCheckBox = new QRadioButton(tr("Slow down on slow down ramp and transit into switch on disabled"));
    quickStopOptionLayout->addRow(_1QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_1QuickStopOptionCheckBox, 1);
    QRadioButton *_2QuickStopOptionCheckBox = new QRadioButton(tr("Slow down on quick stop ramp and transit into switch on disabled"));
    quickStopOptionLayout->addRow(_2QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_2QuickStopOptionCheckBox, 2);
    QRadioButton *_5QuickStopOptionCheckBox = new QRadioButton(tr("Slow down on slow down ramp and stay in quick stop active"));
    quickStopOptionLayout->addRow(_5QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_5QuickStopOptionCheckBox, 5);
    QRadioButton *_6QuickStopOptionCheckBox = new QRadioButton(tr("Slow down on quick stop ramp and stay in quick stop active"));
    quickStopOptionLayout->addRow(_6QuickStopOptionCheckBox);
    _quickStopOptionGroup->addButton(_6QuickStopOptionCheckBox, 6);
    quickStopOptionGroupBox->setLayout(quickStopOptionLayout);
    connect(_quickStopOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { quickStopOptionClicked(id); });
    // END Group Box Quick stop option

    // Group Box Shutdown option code
    QGroupBox *shutdownOptionGroupBox = new QGroupBox(tr("Shutdown option code (0x605B)"));
    QFormLayout *shutdownOptionLayout = new QFormLayout();
    _shutdownOptionGroup = new QButtonGroup(this);
    _shutdownOptionGroup->setExclusive(true);
    QRadioButton *_0ShutdownOptionCheckBox = new QRadioButton(tr("Disable drive function (switch-off the drive power stage)"));
    shutdownOptionLayout->addRow(_0ShutdownOptionCheckBox);
    _shutdownOptionGroup->addButton(_0ShutdownOptionCheckBox, 0);
    QRadioButton *_1ShutdownOptionCheckBox = new QRadioButton(tr("Slow down with slow down ramp; disable of the drive function"));
    shutdownOptionLayout->addRow(_1ShutdownOptionCheckBox);
    _shutdownOptionGroup->addButton(_1ShutdownOptionCheckBox, 1);
    shutdownOptionGroupBox->setLayout(shutdownOptionLayout);
    connect(_shutdownOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { shutdownOptionClicked(id); });
    // END Group Box Shutdown option code

    // Group Box Disable operation option code
    QGroupBox *disableOptionGroupBox = new QGroupBox(tr("Disable operation option code (0x605C)"));
    QFormLayout *disableOptionLayout = new QFormLayout();
    _disableOptionGroup = new QButtonGroup(this);
    _disableOptionGroup->setExclusive(true);
    QRadioButton *_0DisableOptionCheckBox = new QRadioButton(tr("Disable drive function (switch-off the drive power stage)"));
    disableOptionLayout->addRow(_0DisableOptionCheckBox);
    _disableOptionGroup->addButton(_0DisableOptionCheckBox, 0);
    QRadioButton *_1DisableOptionCheckBox = new QRadioButton(tr("Slow down with slow down ramp; disable of the drive function"));
    disableOptionLayout->addRow(_1DisableOptionCheckBox);
    _disableOptionGroup->addButton(_1DisableOptionCheckBox, 1);
    disableOptionGroupBox->setLayout(disableOptionLayout);
    connect(_disableOptionGroup, QOverload<int>::of(&QButtonGroup::buttonPressed), [=](int id) { disableOptionClicked(id); });
    // END Group Box Disable operation option code

    // Group Box Halt option
    QGroupBox *haltOptionGroupBox = new QGroupBox(tr("Halt option (0x605D)"));
    QFormLayout *haltOptionLayout = new QFormLayout();
    _haltOptionGroup = new QButtonGroup(this);
    _haltOptionGroup->setExclusive(true);
    QRadioButton *_slowDownRampCheckBox = new QRadioButton(tr("Slow down on slow down ramp and stay in operation enabled"));
    haltOptionLayout->addRow(_slowDownRampCheckBox);
    _haltOptionGroup->addButton(_slowDownRampCheckBox, 1);
    QRadioButton *_quickStopRampCheckBox = new QRadioButton(tr("Slow down on quick stop ramp and stay in operation enabled"));
    haltOptionLayout->addRow(_quickStopRampCheckBox);
    _haltOptionGroup->addButton(_quickStopRampCheckBox, 2);
    haltOptionGroupBox->setLayout(haltOptionLayout);
    connect(_haltOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { haltOptionClicked(id); });
    // END Group Box Halt option

    // Group Box Fault reaction option
    QGroupBox *faultReactionOptionGroupBox = new QGroupBox(tr("Fault reaction option (0x605E)"));
    QFormLayout *faultReactionOptionLayout = new QFormLayout();
    _faultReactionOptionGroup = new QButtonGroup(this);
    _faultReactionOptionGroup->setExclusive(true);
    QRadioButton *_0FaultReactionOptionCheckBox = new QRadioButton(tr("Disable drive function, motor is free to rotate"));
    faultReactionOptionLayout->addRow(_0FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_0FaultReactionOptionCheckBox, 0);
    QRadioButton *_1FaultReactionOptionCheckBox = new QRadioButton(tr("Slow down on slow down ramp"));
    faultReactionOptionLayout->addRow(_1FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_1FaultReactionOptionCheckBox, 1);
    QRadioButton *_2FaultReactionOptionCheckBox = new QRadioButton(tr("Slow down on quick stop ramp"));
    faultReactionOptionLayout->addRow(_2FaultReactionOptionCheckBox);
    _faultReactionOptionGroup->addButton(_2FaultReactionOptionCheckBox, 2);
    faultReactionOptionGroupBox->setLayout(faultReactionOptionLayout);
    connect(_faultReactionOptionGroup, QOverload<int>::of(&QButtonGroup::buttonClicked), [=](int id) { faultReactionOptionClicked(id); });
    // END Group Box Quick stop option

    layout->addWidget(abortConnectionOptionGroupBox);
    layout->addWidget(quickStopOptionGroupBox);
    layout->addWidget(shutdownOptionGroupBox);
    layout->addWidget(disableOptionGroupBox);
    layout->addWidget(haltOptionGroupBox);
    layout->addWidget(faultReactionOptionGroupBox);
    // END SECOND COLUMM

    setLayout(layout);
}

void P402OptionWidget::odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags)
{
    if (!_node)
    {
        return;
    }

    if ((objId.index == _abortConnectionObjectId)
        || (objId.index == _quickStopObjectId)
        || (objId.index == _shutdownObjectId)
        || (objId.index == _disableObjectId)
        || (objId.index == _haltObjectId)
        || (objId.index == _faultReactionObjectId))
    {
        if (flags == SDO::FlagsRequest::Error)
        {
            return;
        }
        refreshData(objId.index);
    }
}
