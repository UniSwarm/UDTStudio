#include "pdomappingwidget.h"

#include <QHBoxLayout>

PDOMappingWidget::PDOMappingWidget(QWidget *parent) : QWidget(parent)
{
    _pdo = nullptr;
    createWidget();
}

void PDOMappingWidget::setPdo(PDO *pdo)
{
    _pdoMappingView->setPdo(pdo);

    if (_pdo)
    {
        disconnect(_pdo, &PDO::enabledChanged, this, &PDOMappingWidget::updateEnabled);
    }
    _pdo = pdo;
    if (_pdo)
    {
        _pdoNameLabel->setText(_pdo->type());
        //updateEnabled(pdo->isEnabled());
        connect(_pdo, &PDO::enabledChanged, this, &PDOMappingWidget::updateEnabled);
    }
}

void PDOMappingWidget::clearMapping()
{
    if (_pdo)
    {
        _pdo->writeMapping(QList<NodeObjectId>());
    }
}

void PDOMappingWidget::setEnabled(bool enabled)
{
    if (_pdo)
    {
        _pdo->setEnabled(enabled);
    }
}

void PDOMappingWidget::updateEnabled(bool enabled)
{
    if (enabled == _enableAction->isChecked())
    {
        return;
    }

    QSignalBlocker blockEnable(_enableAction);
    _enableAction->setChecked(enabled);
}

PDO *PDOMappingWidget::pdo() const
{
    return _pdo;
}

void PDOMappingWidget::createWidget()
{
    QLayout *layout = new QHBoxLayout();

    QLayout *layoutHeader = new QVBoxLayout();
    layoutHeader->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    _pdoNameLabel = new QLabel();
    layoutHeader->addWidget(_pdoNameLabel);

    _toolBar = new QToolBar(tr("Mapping commands"));
    _toolBar->setIconSize(QSize(12, 12));
    _toolBar->setStyleSheet("QToolBar {background: none}");
    QAction *action;

    action = _toolBar->addAction(tr("Clear"));
    action->setIcon(QIcon(":/icons/img/icons8-broom.png"));
    action->setStatusTip(tr("Clear mapping"));
    connect(action, &QAction::triggered, this, &PDOMappingWidget::clearMapping);

    _enableAction = _toolBar->addAction(tr("Enable"));
    _enableAction->setIcon(QIcon(":/icons/img/icons8-checked-checkbox.png"));
    _enableAction->setStatusTip(tr("Enable pdo"));
    _enableAction->setCheckable(true);
    connect(_enableAction, &QAction::triggered, this, &PDOMappingWidget::setEnabled);

    layoutHeader->addWidget(_toolBar);

    layoutHeader->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    layout->addItem(layoutHeader);

    _pdoMappingView = new PDOMappingView();
    layout->addWidget(_pdoMappingView);

    setLayout(layout);
}
