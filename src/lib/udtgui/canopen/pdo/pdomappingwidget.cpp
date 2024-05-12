/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2024 UniSwarm
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

#include "pdomappingwidget.h"

#include <QHBoxLayout>

#include "services/pdo.h"

PDOMappingWidget::PDOMappingWidget(QWidget *parent)
    : QWidget(parent)
{
    _pdo = nullptr;
    createWidget();
}

void PDOMappingWidget::setPdo(PDO *pdo)
{
    _pdoMappingView->setPdo(pdo);

    if (_pdo != nullptr)
    {
        disconnect(_pdo, &PDO::enabledChanged, this, &PDOMappingWidget::updateEnabled);
        disconnect(_pdo, &PDO::cobIdChanged, this, &PDOMappingWidget::updateCobId);
    }
    _pdo = pdo;
    if (_pdo != nullptr)
    {
        _pdoNameLabel->setText(_pdo->type());
        updateEnabled(pdo->isEnabled());
        updateCobId(pdo->cobId());
        connect(_pdo, &PDO::enabledChanged, this, &PDOMappingWidget::updateEnabled);
        connect(_pdo, &PDO::cobIdChanged, this, &PDOMappingWidget::updateCobId);
    }
}

void PDOMappingWidget::clearMapping()
{
    if (_pdo != nullptr)
    {
        _pdo->writeMapping(QList<NodeObjectId>());
    }
}

void PDOMappingWidget::setEnabled(bool enabled)
{
    if (_pdo != nullptr)
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

void PDOMappingWidget::updateCobId(quint32 cobIb)
{
    _pdoIndexLabel->setText(QStringLiteral("0x%1").arg(QString::number(cobIb, 16).toUpper()));
}

PDO *PDOMappingWidget::pdo() const
{
    return _pdo;
}

void PDOMappingWidget::createWidget()
{
    QLayout *layout = new QHBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    QLayout *layoutHeader = new QVBoxLayout();
    layoutHeader->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));

    _pdoNameLabel = new QLabel();
    _pdoNameLabel->setAlignment(Qt::AlignCenter);
    layoutHeader->addWidget(_pdoNameLabel);

    _toolBar = new QToolBar(tr("Mapping commands"));
    _toolBar->setIconSize(QSize(12, 12));
    _toolBar->setStyleSheet(QStringLiteral("QToolBar {background: none}"));

    QAction *action;
    action = _toolBar->addAction(tr("Clear"));
    action->setIcon(QIcon(QStringLiteral(":/icons/img/icons8-broom.png")));
    action->setStatusTip(tr("Clear mapping"));
    connect(action, &QAction::triggered, this, &PDOMappingWidget::clearMapping);

    _enableAction = _toolBar->addAction(tr("Enable"));
    QIcon iconCheck;
    iconCheck.addFile(QStringLiteral(":/icons/img/icons8-checked-checkbox.png"), QSize(), QIcon::Normal, QIcon::On);
    iconCheck.addFile(QStringLiteral(":/icons/img/icons8-unchecked-checkbox.png"), QSize(), QIcon::Normal, QIcon::Off);
    _enableAction->setIcon(iconCheck);
    _enableAction->setStatusTip(tr("Enable pdo"));
    _enableAction->setCheckable(true);
    connect(_enableAction, &QAction::triggered, this, &PDOMappingWidget::setEnabled);

    layoutHeader->addWidget(_toolBar);

    _pdoIndexLabel = new QLabel();
    _pdoIndexLabel->setAlignment(Qt::AlignCenter);
    layoutHeader->addWidget(_pdoIndexLabel);

    layoutHeader->addItem(new QSpacerItem(0, 0, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding));
    layout->addItem(layoutHeader);

    _pdoMappingView = new PDOMappingView();
    layout->addWidget(_pdoMappingView);

    setLayout(layout);
}
