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

#include "abstractindexwidget.h"

#include "udtguimanager.h"
#include "canopen/datalogger/dataloggersingleton.h"
#include "node.h"

#include <QApplication>
#include <QDrag>
#include <QMainWindow>
#include <QMenu>
#include <QMimeData>
#include <QMouseEvent>
#include <QStatusBar>
#include <QWidget>

AbstractIndexWidget::AbstractIndexWidget(const NodeObjectId &objId)
{
    Q_UNUSED(objId);

    _hint = DisplayDirectValue;
    _bitMask = 0xFFFFFFFFFFFFFFFF;
    _offset = 0.0;
    _scale = 1.0;

    _requestRead = false;

    _widget = nullptr;
}

Node *AbstractIndexWidget::node() const
{
    return nodeInterrest();
}

void AbstractIndexWidget::setNode(Node *node)
{
    setNodeInterrest(node);
    if (node != nullptr)
    {
        _objId.setBusId(node->busId());
        _objId.setNodeId(node->nodeId());
        if (_objId.isValid())
        {
            _lastValue = nodeInterrest()->nodeOd()->value(_objId);
            this->updateObjId();
            setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Normal);
        }
        else
        {
            this->updateObjId();
        }
    }
    else
    {
        _objId.setBusId(0xFF);
        _objId.setNodeId(0xFF);
    }
}

void AbstractIndexWidget::requestWriteValue(const QVariant &value)
{
    if (nodeInterrest() == nullptr)
    {
        return;
    }

    switch (inBound(value))
    {
        case AbstractIndexWidget::BoundTooLow:
            _pendingValue = (_minValue.isValid()) ? _minValue : _minType;
            break;

        case AbstractIndexWidget::BoundOK:
            _pendingValue = value;
            break;

        case AbstractIndexWidget::BoundTooHigh:
            _pendingValue = (_maxValue.isValid()) ? _maxValue : _maxType;
            break;
    }

    if (_scale != 0 && _pendingValue.canConvert(QMetaType::Double))
    {
        _pendingValue = _pendingValue.toDouble() / _scale;
    }

    switch (_hint)
    {
        case AbstractIndexWidget::DisplayDirectValue:
        case AbstractIndexWidget::DisplayHexa:
            _pendingValue = static_cast<int32_t>(_pendingValue.toDouble());
            break;

        case AbstractIndexWidget::DisplayQ15_16:
        case AbstractIndexWidget::DisplayQ1_15:
            _pendingValue = static_cast<int32_t>(_pendingValue.toDouble() * 65536.0);
            break;

        case AbstractIndexWidget::DisplayFloat:
            break;
    }

    nodeInterrest()->writeObject(_objId, _pendingValue);
    setDisplayValue(pValue(_pendingValue, _hint), DisplayAttribute::PendingValue);
}

void AbstractIndexWidget::requestReadValue()
{
    if (nodeInterrest() == nullptr)
    {
        return;
    }
    _requestRead = true;
    nodeInterrest()->readObject(_objId);
}

void AbstractIndexWidget::cancelEdit()
{
    if (nodeInterrest() == nullptr)
    {
        setDisplayValue(QVariant(), DisplayAttribute::Error);
        return;
    }
    setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Normal);
}

void AbstractIndexWidget::updateHint()
{
}

void AbstractIndexWidget::updateRange()
{
}

void AbstractIndexWidget::updateObjId()
{
    NodeSubIndex *nodeSubIndex = _objId.nodeSubIndex();
    if (nodeSubIndex != nullptr)
    {
        _minType = nodeSubIndex->minType();
        _maxType = nodeSubIndex->maxType();
        if (_unit.isEmpty())
        {
            setUnit(nodeSubIndex->unit());
        }
        _scale = nodeSubIndex->scale();
        updateToolTip();
    }
}

QVariant AbstractIndexWidget::pValue(const QVariant &value, const AbstractIndexWidget::DisplayHint hint) const
{
    QVariant val = value;

    if (_bitMask != 0xFFFFFFFFFFFFFFFF)
    {
        val = value.toLongLong() & _bitMask;
    }

    if (_offset != 0.0)
    {
        val = value.toDouble() + _offset;
    }

    if (_scale != 1.0)
    {
        val = value.toDouble() * _scale;
    }

    if (hint == AbstractIndexWidget::DisplayQ1_15 || hint == AbstractIndexWidget::DisplayQ15_16)
    {
        val = value.toDouble() / 65536.0;
    }

    return val;
}

QString AbstractIndexWidget::pstringValue(const QVariant &value, const AbstractIndexWidget::DisplayHint hint) const
{
    QString str;
    switch (hint)
    {
        case AbstractIndexWidget::DisplayDirectValue:
            if (value.userType() != QMetaType::QString && value.userType() != QMetaType::QByteArray)
            {
                str = QString::number(value.toInt());
            }
            else
            {
                str = value.toString();
            }
            break;

        case AbstractIndexWidget::DisplayHexa:
            str = "0x" + QString::number(value.toInt(), 16).toUpper();
            break;

        case AbstractIndexWidget::DisplayQ1_15:
        case AbstractIndexWidget::DisplayQ15_16:
        case AbstractIndexWidget::DisplayFloat:
            str = QString::number(value.toDouble(), 'g', 10);
            if (!str.contains('.'))
            {
                str.append(".0");
            }
            break;
    }

    if (!_unit.isEmpty())
    {
        str.append(_unit);
    }

    return str;
}

AbstractIndexWidget::Bound AbstractIndexWidget::inBound(const QVariant &value)
{
    QVariant min = (_minValue.isValid()) ? _minValue : _minType;
    QVariant max = (_maxValue.isValid()) ? _maxValue : _maxType;

    if (value.toDouble() > max.toDouble())
    {
        return BoundTooHigh;
    }
    if (value.toDouble() < min.toDouble())
    {
        return BoundTooLow;
    }

    return BoundOK;
}

void AbstractIndexWidget::indexWidgetMouseClick(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        _dragStartPosition = event->pos();
    }
}

void AbstractIndexWidget::indexWidgetMouseMove(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton))
    {
        return;
    }
    if ((event->pos() - _dragStartPosition).manhattanLength() < QApplication::startDragDistance())
    {
        return;
    }
    QDrag *drag = new QDrag(_widget);
    QMimeData *mimeData = new QMimeData();

    QByteArray encodedData;
    encodedData.append(objId().mimeData().toUtf8());
    mimeData->setData("index/subindex", encodedData);
    drag->setMimeData(mimeData);

    drag->exec();
}

const QVariant &AbstractIndexWidget::minValue() const
{
    return _minValue;
}

void AbstractIndexWidget::setMinValue(const QVariant &minValue)
{
    _minValue = minValue;
    updateRange();
}

const QVariant &AbstractIndexWidget::maxValue() const
{
    return _maxValue;
}

void AbstractIndexWidget::setMaxValue(const QVariant &maxValue)
{
    _maxValue = maxValue;
    updateRange();
}

void AbstractIndexWidget::setRangeValue(const QVariant &minValue, const QVariant &maxValue)
{
    _minValue = minValue;
    _maxValue = maxValue;
    updateRange();
}

QString AbstractIndexWidget::unit() const
{
    return _unit;
}

void AbstractIndexWidget::setUnit(const QString &unit)
{
    _unit = unit;
}

double AbstractIndexWidget::scale() const
{
    return _scale;
}

void AbstractIndexWidget::setScale(double scale)
{
    if (scale == 0.0)
    {
        scale = 1.0;
    }
    _scale = scale;
}

double AbstractIndexWidget::offset() const
{
    return _offset;
}

void AbstractIndexWidget::setOffset(double offset)
{
    _offset = offset;
}

uint64_t AbstractIndexWidget::bitMask() const
{
    return _bitMask;
}

void AbstractIndexWidget::setBitMask(const uint64_t &bitMask)
{
    _bitMask = bitMask;
}

AbstractIndexWidget::DisplayHint AbstractIndexWidget::hint() const
{
    return _hint;
}

void AbstractIndexWidget::setDisplayHint(const AbstractIndexWidget::DisplayHint hint)
{
    if (_hint != hint)
    {
        _hint = hint;
        updateHint();
        if (nodeInterrest() == nullptr)
        {
            return;
        }
        setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Normal);
    }
}

QVariant AbstractIndexWidget::value() const
{
    return pValue(_lastValue, _hint);
}

QString AbstractIndexWidget::stringValue() const
{
    return pstringValue(value(), _hint);
}

void AbstractIndexWidget::readObject()
{
    if (nodeInterrest() == nullptr)
    {
        return;
    }
    nodeInterrest()->readObject(_objId);
}

const NodeObjectId &AbstractIndexWidget::objId() const
{
    return _objId;
}

void AbstractIndexWidget::setObjId(const NodeObjectId &objId)
{
    if (_objId.isValid())
    {
        unRegisterObjId(_objId);
    }
    _objId = objId;

    if (objId.isASubIndex())
    {
        registerObjId(_objId);
    }
    Node *node = _objId.node();
    if (node != nullptr)
    {
        setNode(node);
        return;
    }
    this->updateObjId();
}

QMainWindow *AbstractIndexWidget::getMainWindow() const
{
    QWidgetList topLevelWidgets = QApplication::topLevelWidgets();
    for (QWidget *w : topLevelWidgets)
    {
        if (QMainWindow *mainWindow = qobject_cast<QMainWindow *>(w))
        {
            return mainWindow;
        }
    }
    return nullptr;
}

void AbstractIndexWidget::displayStatus(const QString &message)
{
    QMainWindow *mainWindow = getMainWindow();
    if (mainWindow == nullptr)
    {
        return;
    }
    mainWindow->statusBar()->showMessage(message);
}

void AbstractIndexWidget::clearStatus()
{
    QMainWindow *mainWindow = getMainWindow();
    if (mainWindow == nullptr)
    {
        return;
    }
    mainWindow->statusBar()->clearMessage();
}

void AbstractIndexWidget::updateToolTip()
{
    if (_widget != nullptr)
    {
        QString toolTip;
        NodeSubIndex *nodeSubIndex = _objId.nodeSubIndex();
        if (nodeSubIndex != nullptr)
        {
            toolTip.append(QObject::tr("<b>Name</b>: %1<br/>", "AbstractIndexWidget").arg(nodeSubIndex->name()));
        }

        toolTip.append(QObject::tr("<b>Index</b>: 0x%1.%2", "AbstractIndexWidget")
                           .arg(QString::number(objId().index(), 16).toUpper().rightJustified(4, '0'), QString::number(objId().subIndex()).toUpper().rightJustified(2, '0')));

        if (nodeSubIndex != nullptr)
        {
            toolTip.append(QObject::tr("<br/><b>Access</b>: %1", "AbstractIndexWidget").arg(nodeSubIndex->accessString()));
            toolTip.append(QObject::tr("<br/><b>Type</b>: %1", "AbstractIndexWidget").arg(NodeSubIndex::dataTypeStr(nodeSubIndex->dataType())));
        }

        _widget->setToolTip(toolTip);
    }
}

QMenu *AbstractIndexWidget::createStandardContextMenu(QMenu *menu)
{
    QMenu *standardMenu = menu;
    QAction *firstAction = nullptr;
    if (menu == nullptr)
    {
        standardMenu = new QMenu();
    }
    if (!standardMenu->actions().isEmpty())
    {
        firstAction = standardMenu->actions().at(0);
    }
    standardMenu->setAttribute(Qt::WA_DeleteOnClose);

    // read action
    QAction *readAction = new QAction(QObject::tr("Read"));
    readAction->setShortcut(Qt::Key_F5);
    readAction->setShortcutContext(Qt::WidgetShortcut);
    readAction->setIcon(QIcon(":/icons/img/icons8-import.png"));
#if QT_VERSION >= 0x050A00
    readAction->setShortcutVisibleInContextMenu(true);
#endif
    QObject::connect(readAction,
                     &QAction::triggered,
                     readAction,
                     [=]()
                     {
                         requestReadValue();
                     });
    standardMenu->insertAction(firstAction, readAction);

    // locate action
    QAction *locateAction = new QAction(QObject::tr("Locate in OD"));
    QObject::connect(locateAction,
                     &QAction::triggered,
                     locateAction,
                     [=]()
                     {
                         UdtGuiManager::locateInOdTreeView(objId());
                     });
    standardMenu->insertAction(firstAction, locateAction);

    standardMenu->insertMenu(firstAction, DataLoggerSingleton::createAddToLoggerMenu(objId()));

    if (menu != nullptr)
    {
        standardMenu->insertSeparator(firstAction);
    }
    return standardMenu;
}

void AbstractIndexWidget::odNotify(const NodeObjectId &objId, NodeOd::FlagsRequest flags)
{
    if (nodeInterrest() == nullptr)
    {
        return;
    }

    _lastValue = nodeInterrest()->nodeOd()->value(objId);
    if ((flags & NodeOd::Error) != 0)
    {
        if (_pendingValue.isValid() && ((flags & NodeOd::Write) != 0))  // we request a write value that cause an error
        {
            setDisplayValue(pValue(_pendingValue, _hint), DisplayAttribute::Error);
            _pendingValue = QVariant();
            return;
        }
        if ((flags & NodeOd::Read) != 0)  // any read cause an error
        {
            if (isEditing() && !_requestRead)
            {
                return;
            }
            setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Error);
            _requestRead = false;
            return;
        }
        // any other write request failed
        return;
    }
    if (((flags & NodeOd::Read) != 0) && this->isEditing() && !_requestRead)
    {
        return;
    }
    setDisplayValue(pValue(_lastValue, _hint), DisplayAttribute::Normal);
    _requestRead = false;
    _pendingValue = QVariant();
}
