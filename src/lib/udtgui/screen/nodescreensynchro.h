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

#ifndef NODESCREENSYNCHRO_H
#define NODESCREENSYNCHRO_H

#include "udtgui_global.h"

#include "nodescreen.h"

#include <QGroupBox>
#include <QSpinBox>
#include <QToolBar>

class IndexComboBox;
class IndexSpinBox;
class IndexLabel;
class DataLogger;
class DataLoggerWidget;

class UDTGUI_EXPORT NodeScreenSynchro : public NodeScreen
{
    Q_OBJECT
public:
    NodeScreenSynchro(QWidget *parent = nullptr);

protected slots:
    void setLogTimer(int ms);

protected:
    uint8_t _axis;

    DataLogger *_dataLogger;
    DataLoggerWidget *_dataLoggerWidget;

    void readAllObject();

    void createWidgets();
    QToolBar *createToolBarWidgets();
    QSpinBox *_logTimerSpinBox;

    QGroupBox *createSynchroConfigurationWidgets();
    QGroupBox *_synchroConfigGroupBox;
    IndexComboBox *_modeSynchroComboBox;
    IndexSpinBox *_maxDiffSpinBox;
    IndexSpinBox *_coeffSpinBox;
    IndexSpinBox *_windowSpinBox;
    IndexSpinBox *_offsetSpinBox;

    QGroupBox *createSynchroStatusWidgets();
    QGroupBox *_synchroStatusGroupBox;
    IndexLabel *_flagLabel;
    IndexLabel *_erorLabel;
    IndexLabel *_correctorLabel;

    // NodeScreen interface
public:
    QString title() const override;
    void setNodeInternal(Node *node, uint8_t axis) override;
};

#endif  // NODESCREENSYNCHRO_H
