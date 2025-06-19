/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2025 UniSwarm
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

#include "nodescreenadamp.h"

#include <QHBoxLayout>
#include <QScrollArea>
#include <QToolBar>

#include <canopen/indexWidget/indexlabel.h>
#include <canopen/indexWidget/indexslider.h>

NodeScreenAdamp::NodeScreenAdamp(QWidget *parent)
    : NodeScreen(parent)
{
    createWidgets();
    connect(&_timer, &QTimer::timeout, this, &NodeScreenAdamp::readAll);
}

void NodeScreenAdamp::toggleStartLogger(bool start)
{
    if (node() == nullptr)
    {
        return;
    }

    if (start)
    {
        _timer.start(_logTimerSpinBox->value());
    }
    else
    {
        _timer.stop();
    }

    if (_startStopAction->isChecked() != start)
    {
        _startStopAction->blockSignals(true);
        _startStopAction->setChecked(start);
        _startStopAction->blockSignals(false);
    }
}

void NodeScreenAdamp::setLogTimer(int ms)
{
    if (_startStopAction->isChecked())
    {
        _timer.start(ms);
    }
}

void NodeScreenAdamp::createWidgets()
{
    QLayout *glayout = new QVBoxLayout();
    glayout->setContentsMargins(0, 0, 0, 0);

    QLayout *toolBarLayout = new QVBoxLayout();
    toolBarLayout->setContentsMargins(2, 2, 2, 0);
    QToolBar *toolBar = new QToolBar(tr("Node screen commands"));
    toolBar->setIconSize(QSize(20, 20));

    // read all action
    QAction *actionReadMappings = toolBar->addAction(tr("Read all"));
    actionReadMappings->setIcon(QIcon(QStringLiteral(":/icons/img/icons8-update.png")));
    actionReadMappings->setShortcut(QKeySequence(QStringLiteral("Ctrl+R")));
    actionReadMappings->setStatusTip(tr("Read all the objects of the current window"));
    connect(actionReadMappings, &QAction::triggered, this, &NodeScreenAdamp::readAll);

    toolBar->addSeparator();
    _startStopAction = toolBar->addAction(tr("Start / stop"));
    _startStopAction->setCheckable(true);
    QIcon iconStartStop;
    iconStartStop.addFile(QStringLiteral(":/icons/img/icons8-stop.png"), QSize(), QIcon::Normal, QIcon::On);
    iconStartStop.addFile(QStringLiteral(":/icons/img/icons8-play.png"), QSize(), QIcon::Normal, QIcon::Off);
    _startStopAction->setIcon(iconStartStop);
    _startStopAction->setStatusTip(tr("Start or stop reading input objects "));
    connect(_startStopAction, &QAction::triggered, this, &NodeScreenAdamp::toggleStartLogger);

    _logTimerSpinBox = new QSpinBox();
    _logTimerSpinBox->setRange(20, 5000);
    _logTimerSpinBox->setValue(200);
    _logTimerSpinBox->setSuffix(QStringLiteral(" ms"));
    _logTimerSpinBox->setToolTip(tr("Sets the interval of timer in ms"));
    connect(_logTimerSpinBox,
            QOverload<int>::of(&QSpinBox::valueChanged),
            this,
            [=](int i)
            {
                setLogTimer(i);
            });
    toolBar->addWidget(_logTimerSpinBox);

    toolBarLayout->addWidget(toolBar);
    glayout->addItem(toolBarLayout);

    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setWidgetResizable(true);

    QWidget *widget = new QWidget(this);
    QGridLayout *layout = new QGridLayout();
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(3);

    {
        IndexLabel *indexLabel;
        int col = 0;

        layout->addWidget(new QLabel(QString("Tension d'alimentation")), 0, col, Qt::AlignHCenter);
        indexLabel = new IndexLabel(NodeObjectId(0x2000, 1));
        indexLabel->setUnit(" V");
        indexLabel->setScale(1/100.);
        layout->addWidget(indexLabel, 1, col++, Qt::AlignHCenter);
        addIndexWidget(indexLabel);

        layout->addWidget(new QLabel(QString("Température CPU")), 0, col, Qt::AlignHCenter);
        indexLabel = new IndexLabel(NodeObjectId(0x2020, 1));
        indexLabel->setUnit(" °C");
        indexLabel->setScale(1/10.);
        layout->addWidget(indexLabel, 1, col++, Qt::AlignHCenter);
        addIndexWidget(indexLabel);

        int row = 4;
        layout->addWidget(new QLabel(QString("Gain preamp")), row++, 0, Qt::AlignHCenter);
        row++;
        layout->addWidget(new QLabel(QString("Température puissance")), row++, 0, Qt::AlignHCenter);
        layout->addWidget(new QLabel(QString("Entrée preamp RMS")), row++, 0, Qt::AlignHCenter);
        layout->addWidget(new QLabel(QString("Tension sortie RMS")), row++, 0, Qt::AlignHCenter);
        layout->addWidget(new QLabel(QString("Courant sortie RMS")), row++, 0, Qt::AlignHCenter);
        layout->addWidget(new QLabel(QString("Impédence")), row++, 0, Qt::AlignHCenter);
        layout->addWidget(new QLabel(QString("Puissance")), row++, 0, Qt::AlignHCenter);
    }

    QFrame *frame = new QFrame();
    frame->setFrameStyle(QFrame::HLine);
    frame->setFrameShadow(QFrame::Sunken);
    layout->addWidget(frame, 2, 0, 1, -1);

    for (int i = 0; i < 4; i++)
    {
        IndexLabel *indexLabel;
        int row = 3;
        int col = i + 1;
        layout->addWidget(new QLabel(QString("Voie %1").arg(i + 1)), row++, col, Qt::AlignHCenter);

        IndexSlider *slider = new IndexSlider(NodeObjectId(0x6201, i + 1));
        slider->setOrientation(Qt::Vertical);
        slider->setRange(-47, 6);
        slider->setVisualFeedBack(false);
        slider->setMaximumHeight(300);
        layout->addWidget(slider, row++, col, Qt::AlignHCenter);
        addIndexWidget(slider);

        indexLabel = new IndexLabel(NodeObjectId(0x6201, i + 1));
        indexLabel->setMinimumWidth(60);
        indexLabel->setUnit(" dB");
        layout->addWidget(indexLabel, row++, col, Qt::AlignHCenter);
        addIndexWidget(indexLabel);

        indexLabel = new IndexLabel(NodeObjectId(0x6003, i / 2 + 1));
        indexLabel->setUnit(" °C");
        indexLabel->setScale(1/10.);
        layout->addWidget(indexLabel, row++, col, Qt::AlignHCenter);
        addIndexWidget(indexLabel);

        indexLabel = new IndexLabel(NodeObjectId(0x6102, i + 1));
        indexLabel->setUnit(" V");
        indexLabel->setScale(1/1000.);
        layout->addWidget(indexLabel, row++, col, Qt::AlignHCenter);
        addIndexWidget(indexLabel);

        indexLabel = new IndexLabel(NodeObjectId(0x6112, i + 1));
        indexLabel->setUnit(" V");
        indexLabel->setScale(1/1000.);
        layout->addWidget(indexLabel, row++, col, Qt::AlignHCenter);
        addIndexWidget(indexLabel);

        indexLabel = new IndexLabel(NodeObjectId(0x6122, i + 1));
        indexLabel->setUnit(" A");
        indexLabel->setScale(1/1000.);
        layout->addWidget(indexLabel, row++, col, Qt::AlignHCenter);
        addIndexWidget(indexLabel);

        indexLabel = new IndexLabel(NodeObjectId(0x6130, i + 1));
        indexLabel->setUnit(" Ω");
        indexLabel->setScale(1/10.);
        layout->addWidget(indexLabel, row++, col, Qt::AlignHCenter);
        addIndexWidget(indexLabel);

        indexLabel = new IndexLabel(NodeObjectId(0x6131, i + 1));
        indexLabel->setUnit(" W");
        indexLabel->setScale(1/10.);
        layout->addWidget(indexLabel, row++, col, Qt::AlignHCenter);
        addIndexWidget(indexLabel);
    }

    widget->setLayout(layout);
    scrollArea->setWidget(widget);

    glayout->addWidget(scrollArea);
    setLayout(glayout);
}

QString NodeScreenAdamp::title() const
{
    return tr("ADAMP");
}

void NodeScreenAdamp::setNodeInternal(Node *node, uint8_t axis)
{
    Q_UNUSED(node)
    Q_UNUSED(axis)
}
