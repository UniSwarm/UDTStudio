/**
 ** This file is part of the UDTStudio project.
 ** Copyright 2019-2020 UniSwarm
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

class IndexSpinBox;
class IndexLabel;

class UDTGUI_EXPORT NodeScreenSynchro : public NodeScreen
{
    Q_OBJECT
  public:
    NodeScreenSynchro(QWidget *parent = nullptr);

  protected:

    uint8_t _axis;
    void createWidgets();

    IndexSpinBox *_modeSynchroSpinBox;
    IndexSpinBox *_maxDiffSpinBox;
    IndexSpinBox *_coeffSpinBox;
    IndexSpinBox *_windowSpinBox;
    IndexSpinBox *_offsetSpinBox;

    IndexLabel *_flagLabel;
    IndexLabel *_erorLabel;
    IndexLabel *_correctorLabel;

    // NodeScreen interface
  public:
    QString title() const override;
    void setNodeInternal(Node *node, uint8_t axis) override;
};

#endif // NODESCREENSYNCHRO_H
