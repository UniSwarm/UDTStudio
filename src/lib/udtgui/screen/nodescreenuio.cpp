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

#include "nodescreenuio.h"

#include "canopen/profileWidget/p401/p401widget.h"

#include <QLayout>

NodeScreenUio::NodeScreenUio(QWidget *parent)
    : NodeScreen(parent)
{
    createWidgets();
}

void NodeScreenUio::toggleStartLogger(bool start)
{
    if (_node == nullptr)
    {
        return;
    }

    if (start)
    {
        _p401Widget->start(_logTimerSpinBox->value());
    }
    else
    {
        _p401Widget->stop();
    }

    if (_startStopAction->isChecked() != start)
    {
        _startStopAction->blockSignals(true);
        _startStopAction->setChecked(start);
        _startStopAction->blockSignals(false);
    }
}

void NodeScreenUio::setLogTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _p401Widget->start(ms);
    }
}

void NodeScreenUio::createWidgets()
{
    _p401Widget = new P401Widget(8, this);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(2, 2, 2, 2);

    QToolBar *toolBar = new QToolBar(tr("UIO commands"));
    toolBar->setIconSize(QSize(20, 20));

    _startStopAction = toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    QIcon iconStartStop;
    iconStartStop.addFile(":/icons/img/icons8-stop.png", QSize(), QIcon::Normal, QIcon::On);
    iconStartStop.addFile(":/icons/img/icons8-play.png", QSize(), QIcon::Normal, QIcon::Off);
    _startStopAction->setIcon(iconStartStop);
    _startStopAction->setStatusTip(tr("Start or stop reading input objects "));
    connect(_startStopAction, &QAction::triggered, this, &NodeScreenUio::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    connect(_logTimerSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int i)
            {
                setLogTimer(i);
            });
    toolBar->addWidget(_logTimerSpinBox);
    toolBar->addSeparator();

    QAction *settingsAction = new QAction();
    settingsAction->setCheckable(true);
    settingsAction->setIcon(QIcon(":/icons/img/icons8-settings.png"));
    settingsAction->setToolTip(tr("Settings"));
    connect(settingsAction, &QAction::triggered, _p401Widget, &P401Widget::setSettings);
    toolBar->addAction(settingsAction);

    // read all action
    QAction *readAllObjectAction = toolBar->addAction(tr("Read all objects"));
    readAllObjectAction->setIcon(QIcon(":/icons/img/icons8-update.png"));
    readAllObjectAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllObjectAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllObjectAction, &QAction::triggered, _p401Widget, &P401Widget::readAllObject);
    toolBar->addAction(readAllObjectAction);
    toolBar->addSeparator();

    QAction *_dataLoggerAction = new QAction();
    _dataLoggerAction->setIcon(QIcon(":/icons/img/icons8-line-chart.png"));
    _dataLoggerAction->setToolTip(tr("Data logger"));
    connect(_dataLoggerAction, &QAction::triggered, _p401Widget, &P401Widget::dataLogger);
    toolBar->addAction(_dataLoggerAction);

    layout->addWidget(toolBar);
    layout->addWidget(_p401Widget);

    setLayout(layout);
}

QString NodeScreenUio::title() const
{
    return QString(tr("UIO"));
}

QIcon NodeScreenUio::icon() const
{
    return QIcon(":/uBoards/uio.png");
}

void NodeScreenUio::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(axis)

    if (node == nullptr)
    {
        return;
    }

    if ((node->profileNumber() != 0x191))
    {
        return;
    }

    _p401Widget->setNode(node);
}
