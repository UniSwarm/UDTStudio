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

#ifndef P402OPTION_H
#define P402OPTION_H

#include "../../udtgui_global.h"

#include "nodeodsubscriber.h"

#include <QButtonGroup>
#include <QScrollArea>

class Node;

class P402OptionWidget : public QScrollArea, public NodeOdSubscriber
{
    Q_OBJECT
public:
    P402OptionWidget(QWidget *parent = nullptr);
    ~P402OptionWidget() override;

    Node *node() const;

public slots:
    void setNode(Node *value, uint8_t axis = 0);
    void updateData();

    void abortConnectionOptionClicked(int id);
    void quickStopOptionClicked(int id);
    void shutdownOptionClicked(int id);
    void disableOptionClicked(int id);
    void haltOptionClicked(int id);
    void faultReactionOptionClicked(int id);

private:
    Node *_node;
    uint8_t _axis;

    NodeObjectId _abortConnectionObjectId;
    NodeObjectId _quickStopObjectId;
    NodeObjectId _shutdownObjectId;
    NodeObjectId _disableObjectId;
    NodeObjectId _haltObjectId;
    NodeObjectId _faultReactionObjectId;

    QButtonGroup *_abortConnectionOptionGroup;
    QButtonGroup *_quickStopOptionGroup;
    QButtonGroup *_shutdownOptionGroup;
    QButtonGroup *_disableOptionGroup;
    QButtonGroup *_haltOptionGroup;
    QButtonGroup *_faultReactionOptionGroup;

    void refreshData(NodeObjectId object);

    void createWidgets();

    // NodeOdSubscriber interface
protected:
    void odNotify(const NodeObjectId &objId, SDO::FlagsRequest flags) override;
};

#endif // P402OPTION_H
