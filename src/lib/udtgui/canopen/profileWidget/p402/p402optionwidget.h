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

#ifndef P402OPTIONWIDGET_H
#define P402OPTIONWIDGET_H

#include "../../../udtgui_global.h"

#include "p402modewidget.h"

#include <QButtonGroup>
#include <QGroupBox>

class Node;
class NodeProfile402;

class UDTGUI_EXPORT P402OptionWidget : public P402ModeWidget
{
    Q_OBJECT
public:
    P402OptionWidget(QWidget *parent = nullptr);
    ~P402OptionWidget() override;

private:
    QButtonGroup *_abortConnectionOptionGroup;
    QButtonGroup *_quickStopOptionGroup;
    QButtonGroup *_shutdownOptionGroup;
    QButtonGroup *_disableOptionGroup;
    QButtonGroup *_haltOptionGroup;
    QButtonGroup *_faultReactionOptionGroup;

    void abortConnectionOptionClicked(int id);
    void quickStopOptionClicked(int id);
    void shutdownOptionClicked(int id);
    void disableOptionClicked(int id);
    void haltOptionClicked(int id);
    void faultReactionOptionClicked(int id);

    // Create widgets
    void createWidgets();
    QGroupBox *abortConnectionWidgets();
    QGroupBox *quickStopWidgets();
    QGroupBox *shutdownWidgets();
    QGroupBox *disableWidgets();
    QGroupBox *haltWidgets();
    QGroupBox *faultReactionWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;

    // P402Mode interface
public:
    void readAllObjects() override;

public slots:
    void setNode(Node *value, uint8_t axis) override;
};

#endif  // P402OPTIONWIDGET_H
