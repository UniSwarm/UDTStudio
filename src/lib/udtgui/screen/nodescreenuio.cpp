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

void NodeScreenUio::readAll()
{
    _p401Widget->readAllObject();
}

void NodeScreenUio::toggleStartLogger(bool start)
{
    if (!_node)
    {
        return;
    }
    if (start)
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-stop.png"));
        _p401Widget->start(_logTimerSpinBox->value());
    }
    else
    {
        _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
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

    QToolBar *toolBar = new QToolBar(tr("UIO commands"));
    toolBar->setIconSize(QSize(20, 20));

    _startStopAction = toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    _startStopAction->setIcon(QIcon(":/icons/img/icons8-play.png"));
    _startStopAction->setStatusTip(tr("Start or stop the data logger"));
    connect(_startStopAction, &QAction::triggered, this, &NodeScreenUio::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(10, 5000);
    _logTimerSpinBox->setValue(100);
    _logTimerSpinBox->setSuffix(" ms");
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    connect(_logTimerSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), [=](int i) { setLogTimer(i); });

    QAction *_option402Action = new QAction();
    _option402Action->setCheckable(true);
    _option402Action->setIcon(QIcon(":/icons/img/icons8-settings.png"));
    _option402Action->setToolTip(tr("Option code"));
    connect(_option402Action, &QAction::triggered, _p401Widget, &P401Widget::setSettings);

    // read all action
    QAction *readAllObjectAction = toolBar->addAction(tr("Read all objects"));
    readAllObjectAction->setIcon(QIcon(":/icons/img/icons8-sync.png"));
    readAllObjectAction->setShortcut(QKeySequence("Ctrl+R"));
    readAllObjectAction->setStatusTip(tr("Read all the objects of the current window"));
    connect(readAllObjectAction, &QAction::triggered, this, &NodeScreenUio::readAll);

    toolBar->addWidget(_logTimerSpinBox);
    toolBar->addSeparator();
    toolBar->addAction(_option402Action);
    toolBar->addAction(readAllObjectAction);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(0, 0, 0, 0);

    toolBar->addSeparator();
    toolBar->addAction(_option402Action);

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
    if (!node)
    {
        return;
    }

    if ((node->profileNumber() != 0x191))
    {
        return;
    }

    _p401Widget->setNode(node);
}
