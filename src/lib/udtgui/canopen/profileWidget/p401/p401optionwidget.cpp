#include "p401optionwidget.h"

#include <QFormLayout>

#include "canopen/widget/indexspinbox.h"
#include "indexdb401.h"

P401OptionWidget::P401OptionWidget(uint8_t channel, QWidget *parent)
    : QWidget(parent)
    , _channel(channel)
{
    createWidgets();
}

void P401OptionWidget::readAllObject()
{
    _diSchmittTriggersHigh->readObject();
    _diSchmittTriggersHigh->readObject();
}

void P401OptionWidget::setNode(Node *node)
{
    if (!node)
    {
        return;
    }
    _node = node;

    _diSchmittTriggersHigh->setObjId(IndexDb401::getObjectId(IndexDb401::OD_MS_DI_SCHMITT_TRIGGERS_HIGH, _channel + 1));
    _diSchmittTriggersHigh->setNode(_node);

    _diSchmittTriggersLow->setObjId(IndexDb401::getObjectId(IndexDb401::OD_MS_DI_SCHMITT_TRIGGERS_LOW, _channel + 1));
    _diSchmittTriggersLow->setNode(_node);
}

void P401OptionWidget::createWidgets()
{
    QFormLayout *formLayout = new QFormLayout();

    _diSchmittTriggersHigh = new IndexSpinBox();
    _diSchmittTriggersLow = new IndexSpinBox();

    formLayout->addRow(tr("Schmitt Triggers High"), _diSchmittTriggersHigh);
    formLayout->addRow(tr("Schmitt Triggers Low"), _diSchmittTriggersLow);

    setLayout(formLayout);
}
