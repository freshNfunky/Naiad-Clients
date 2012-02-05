// -----------------------------------------------------------------------------
//
// NsGraphOpItem.cc
//
// Abstract base class for Op items in the graph view, source file.
//
// Copyright (c) 2011 Exotic Matter AB. All rights reserved.
//
// This file is part of Open Naiad Studio.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// * Neither the name of Exotic Matter AB nor its contributors may be used to
// endorse or promote products derived from this software without specific
// prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
// FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
// LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
// LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
// ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// -----------------------------------------------------------------------------

#include "NsGraphOpItem.h"
#include "NsGraphOpItemArgs.h"
#include "NsGraphOpStateItem.h"
#include "NsGraphOpNameItem.h"
#include "NsGraphInputPlugItem.h"
#include "NsGraphBodyInputPlugItem.h"
#include "NsGraphFieldInputPlugItem.h"
#include "NsGraphOutputPlugItem.h"
#include "NsGraphBodyOutputPlugItem.h"
#include "NsGraphFieldOutputPlugItem.h"
#include "NsPreferences.h"
#include "NsCmdSetOpPosition.h"
#include "NsCmdSelectOp.h"
#include "NsQuery.h"

#include <NgStore.h>
#include <QDebug>

// -----------------------------------------------------------------------------

// ~NsGraphOpItem
// --------------
//! DTOR.

NsGraphOpItem::~NsGraphOpItem()
{ qDebug() << "~NsGraphOpItem"; }


QString
NsGraphOpItem::longName() const
{
    return _longName;
    //return op().longName();
}


// setSelected
// -----------
//! Select this item using a command.

void
NsGraphOpItem::setSelected(const bool   selected,
                           NsUndoStack *undoStack,
                           const bool   merge)
{
    if (_op->isSelected() != selected) {
        if (0 != undoStack) {
            NsCmdSelectOp::exec(
                NsCmdSelectOp::ArgsList() <<
                    NsCmdSelectOp::Args(_op->longName(), selected), 
                *undoStack, merge);
        }
        else {
            NsCmdSelectOp::exec(                
                NsCmdSelectOp::ArgsList() <<
                    NsCmdSelectOp::Args(_op->longName(), selected)); 
        }
    }
}


// setSelected
// -----------
//! Select this item using a command.

void
NsGraphOpItem::setSelected(const bool selected, QUndoCommand &parent)
{
    if (_op->isSelected() != selected) {
        NsCmdSelectOp::exec(                
            NsCmdSelectOp::ArgsList() <<
                NsCmdSelectOp::Args(_op->longName(), selected), 
            parent); 
    }
}


bool
NsGraphOpItem::isSelected() const
{
    return _op->isSelected();
}

// -----------------------------------------------------------------------------

// NsGraphOpItem
// -------------
//! CTOR.

NsGraphOpItem::NsGraphOpItem(const NsGraphOpItemArgs &args)
    : NsGraphItem(args.parent(), _isSelectable, _isMovable)
    , _op(args.op())
    , _undoStack(args.undoStack())
    , _diameter(args.diameter())
    , _longName(args.op()->longName())
{
#ifdef NS_GRAPH_VIEW_DEBUG
    setAcceptsHoverEvents(true);
#endif

    _setPosition(op()->position());
}


// plugItems
// ---------
//! Returns a list of the Op item's plug items. The list may be empty.

QList<NsGraphPlugItem*>
NsGraphOpItem::plugItems() const
{
    // NB: We must use childItems() as opposed to findChildren() because
    // QGraphicsObject wants this.

    QList<NsGraphPlugItem*> plugList;

    foreach (QGraphicsItem *item, childItems()) {
        NsGraphPlugItem *plug(dynamic_cast<NsGraphPlugItem*>(item));

        if (0 != plug) {
            plugList.append(plug);
        }
    }

    return plugList;
}


// inputPlugItems
// --------------
//! Returns a list of the Op item's input plug items. The list may be empty.

QList<NsGraphInputPlugItem*>
NsGraphOpItem::inputPlugItems() const
{
    // NB: We must use childItems() as opposed findChildren() because
    // QGraphicsObject wants this.

    QList<NsGraphInputPlugItem*> plugList;

    foreach (QGraphicsItem *item, childItems()) {
        NsGraphInputPlugItem *plug(dynamic_cast<NsGraphInputPlugItem*>(item));

        if (0 != plug) {
            plugList.append(plug);
        }
    }

    return plugList;
}


// outputPlugItems
// ---------------
//! Returns a list of the Op item's output plug items. The list may be empty.

QList<NsGraphOutputPlugItem*>
NsGraphOpItem::outputPlugItems() const
{
    // NB: We must use childItems() as opposed findChildren() because
    // QGraphicsObject wants this.

    QList<NsGraphOutputPlugItem*> plugList;

    foreach (QGraphicsItem *item, childItems()) {
        NsGraphOutputPlugItem *plug(
            dynamic_cast<NsGraphOutputPlugItem*>(item));

        if (0 != plug) {
            plugList.append(plug);
        }
    }

    return plugList;
}


// plugItem
// --------
//! Returns null if the given plug does not exist.

NsGraphPlugItem*
NsGraphOpItem::plugItem(const QString &plugLongName) const
{
    foreach (NsGraphPlugItem *gpi, plugItems()) {
        if (plugLongName == gpi->plugObject()->longName()) {
            return gpi;    // Found, we are done!
        }
    }

    return 0;   // Plug not found.
}


// inputPlugItem
// -------------
//! Returns null if the given plug does not exist.

NsGraphInputPlugItem*
NsGraphOpItem::inputPlugItem(const QString &plugLongName) const
{
    foreach (NsGraphInputPlugItem *gipi, inputPlugItems()) {
        if (plugLongName == gipi->plugObject()->longName()) {
            return gipi;    // Found, we are done!
        }
    }

    return 0;   // Plug not found.
}


// outputPlugItem
// --------------
//! Returns null if the given plug does not exist.

NsGraphOutputPlugItem*
NsGraphOpItem::outputPlugItem(const QString &plugLongName) const
{
    foreach (NsGraphOutputPlugItem *gopi, outputPlugItems()) {
        if (plugLongName == gopi->plugObject()->longName()) {
            return gopi;    // Found, we are done!
        }
    }

    return 0;   // Plug not found.
}


// hasInputPlugItems
// -----------------
//! Returns true if list of input plugs is non-empty.

bool
NsGraphOpItem::hasInputPlugItems() const
{
    return !inputPlugItems().empty();
}


// hasOutputPlugItems
// ------------------
//! Returns true if list of output plugs is non-empty.

bool
NsGraphOpItem::hasOutputPlugItems() const
{
    return !outputPlugItems().empty();
}

// -----------------------------------------------------------------------------

// _addStateItem
// -------------
//! Create an item representing the state of the Op.

NsGraphOpStateItem*
NsGraphOpItem::addStateItem(const qreal diameter)
{
    NsGraphOpStateItem *gosi(new NsGraphOpStateItem(*this, diameter)); // Child.

    // Connect state item to Op.

    connect(op(), SIGNAL(stateChanged(/*QString*/)),
            gosi, SLOT(onStateChanged(/*QString*/)));

    return gosi;
}


// _addNameItem
// ------------
//! Create an item representing the name of the Op.

NsGraphOpNameItem*
NsGraphOpItem::addNameItem()
{
    NsGraphOpNameItem *goni(new NsGraphOpNameItem(*this)); // Child.

    // Connect state item to Op.

    connect(op(), SIGNAL(nameChanged(QString,QString)),
            goni, SLOT(onNameChanged(QString,QString)));

    return goni;
}


// createBodyInputPlugItem
// -----------------------
//! DOCS

NsGraphBodyInputPlugItem*
NsGraphOpItem::createBodyInputPlugItem(NsBodyInputPlugObject *bipo,
                                       NsGraphOpItem          &parent,
                                       const qreal             diameter,
                                       const QPointF          &plugPos,
                                       const QLineF           &line,
                                       const QPointF          &expTextPos,
                                       const QPointF          &minTextPos,
                                       const QString          &text) const
{
    NsGraphBodyInputPlugItem *bipi =
        new NsGraphBodyInputPlugItem(
                bipo,
                parent,  // Child.
                diameter,
                plugPos,
                line,
                expTextPos,
                minTextPos,
                text);
    connect(bipo, SIGNAL(groupChanged()),
            bipi, SLOT(onGroupChanged()));
    connect(bipo, SIGNAL(smackChanged()),
            bipi, SLOT(onSmackChanged()));
    return bipi;
}


// createFieldInputPlugItem
// ------------------------
//! DOCS

NsGraphFieldInputPlugItem*
NsGraphOpItem::createFieldInputPlugItem(NsFieldInputPlugObject *fipo,
                                        NsGraphOpItem          &parent,
                                        const qreal             diameter,
                                        const QPointF          &plugPos,
                                        const QLineF           &line,
                                        const QPointF          &expTextPos,
                                        const QPointF          &minTextPos,
                                        const QString          &text) const
{
    NsGraphFieldInputPlugItem *fipi =
        new NsGraphFieldInputPlugItem(
            fipo,
            parent,  // Child.
            diameter,
            plugPos,
            line,
            expTextPos,
            minTextPos,
            text);
    connect(fipo, SIGNAL(groupChanged()),
            fipi, SLOT(onGroupChanged()));
    connect(fipo, SIGNAL(smackChanged()),
            fipi, SLOT(onSmackChanged()));
    return fipi;
}


// createBodyOutputPlugItem
// ------------------------
//! DOCS

NsGraphBodyOutputPlugItem*
NsGraphOpItem::createBodyOutputPlugItem(NsBodyOutputPlugObject *bopo,
                                        NsGraphOpItem          &parent,
                                        const qreal             diameter,
                                        const QPointF          &plugPos,
                                        const QLineF           &line,
                                        const QPointF          &expTextPos,
                                        const QPointF          &minTextPos,
                                        const QString          &text) const
{
    NsGraphBodyOutputPlugItem *bopi =
        new NsGraphBodyOutputPlugItem(
            bopo,
            parent,  // Child.
            diameter,
            plugPos,
            line,
            expTextPos,
            minTextPos,
            text);
    connect(bopo, SIGNAL(groupChanged()),
            bopi, SLOT(onGroupChanged()));
    connect(bopo, SIGNAL(smackChanged()),
            bopi, SLOT(onSmackChanged()));
    return bopi;
}


// createFieldOutputPlugItem
// -------------------------
//! DOCS

NsGraphFieldOutputPlugItem*
NsGraphOpItem::createFieldOutputPlugItem(NsFieldOutputPlugObject *fopo,
                                         NsGraphOpItem           &parent,
                                         const qreal              diameter,
                                         const QPointF           &plugPos,
                                         const QLineF            &line,
                                         const QPointF           &expTextPos,
                                         const QPointF           &minTextPos,
                                         const QString           &text) const
{
    NsGraphFieldOutputPlugItem *fopi =
        new NsGraphFieldOutputPlugItem(
            fopo,
            parent,  // Child.
            diameter,
            plugPos,
            line,
            expTextPos,
            minTextPos,
            text);
    connect(fopo, SIGNAL(groupChanged()),
            fopi, SLOT(onGroupChanged()));
    connect(fopo, SIGNAL(smackChanged()),
            fopi, SLOT(onSmackChanged()));
    return fopi;
}

// -----------------------------------------------------------------------------

// plugDiameter
// ------------
//! Returns plug diameter computed from Op diameter. [static]

qreal
NsGraphOpItem::plugDiameter(const qreal opDiameter)
{
    return 0.25*opDiameter;
}

// -----------------------------------------------------------------------------

// stateDiameter
// -------------
//! Returns recommended diameter for Op state item. [static]

qreal
NsGraphOpItem::stateDiameter(const qreal opDiameter)
{
    return 0.25*opDiameter;
}

// -----------------------------------------------------------------------------

// paint
// -----
//! Draw the item in the graph view.

void
NsGraphOpItem::paint(QPainter                       *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget                        *widget)
{
    const QPainterPath shapePath = shape();
    const bool opEnabled = op()->isEnabled();
    QBrush fb;
    switch(op()->condition()) {
    case NsOpObject::Error:
        fb = errorBrush;
        break;
    case NsOpObject::Warning:
        fb = warningBrush;
        break;
    case NsOpObject::Stepping:
        fb = steppingBrush;
        break;
    case NsOpObject::None:
    default:
        const QColor categoryColor = 
            NsPreferences::instance()->graphViewOpCategoryColor(
                op()->categoryNames().first());
        const QColor disabledColor =
            NsPreferences::instance()->graphViewOpDisabledColor();
        fb = !opEnabled ? QBrush(disabledColor) : QBrush(categoryColor);
        break;
    }

    //painter->setClipRect(boundingRect());
    painter->fillPath(shapePath, fb);

    if (op()->isSelected() || isItemSelected()) {
        painter->strokePath(
            shapePath, 
            QPen(NsPreferences::instance()->graphViewOpLineColor(), 4));
        painter->strokePath(
            shapePath, 
            QPen(NsPreferences::instance()->graphViewSelectionColor(), 2));
    }
    else {
        painter->strokePath(
            shapePath,
            QPen(NsPreferences::instance()->graphViewOpLineColor(), 2));
    }

    if (!opEnabled) {
        const QRectF shapeRect = shapePath.boundingRect();
        painter->setBrush(Qt::NoBrush);
        painter->setPen(
            QPen(NsPreferences::instance()->graphViewOpLineColor(), 8));
        painter->drawLine(shapeRect.bottomLeft(), shapeRect.topRight());
        painter->drawLine(shapeRect.bottomRight(), shapeRect.topLeft());
    }

#ifdef NS_GRAPH_VIEW_DEBUG
    // For debugging - draw bounding rect

    painter->setBrush(Qt::transparent);
    painter->setPen(QPen(QColor(255, 0, 0, 127), 0));
    painter->drawRect(boundingRect());

    painter->setBrush(Qt::transparent);
    painter->setPen(QPen(QColor(0, 255, 0, 127), 0));
    painter->drawPath(shape());
#endif
}

// -----------------------------------------------------------------------------

#ifdef NS_GRAPH_VIEW_DEBUG

// hoverEnterEvent
// ---------------
//! DOCS

void
NsGraphOpItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{

    QToolTip::showText(
        event->screenPos(),
        QString(
            tr("Family") + ": " + op()->familyName() + "\n" +
            tr("Type")   + ": " + op()->typeName()   + "\n" +
            tr("Name")   + ": " + op()->longName()));
    NsGraphItem::hoverEnterEvent(event);
}


// hoverLeaveEvent
// ---------------
//! DOCS

void
NsGraphOpItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QToolTip::hideText();
    NsGraphItem::hoverLeaveEvent(event);
}

#endif  // NS_GRAPH_VIEW_DEBUG

// -----------------------------------------------------------------------------

//void
//NsGraphOpItem::_setSelected(const bool selected)
//{
//    if (isSelected() != selected) {
//        //QGraphicsObject::setSelected(selected);
//        update();
//    }
//}
//
//
//void
//NsGraphOpItem::_setPosition(const QPointF &p)
//{
//    if (pos() != p) {
//        QGraphicsObject::setPos(p);
//        update();
//    }
//}

// -----------------------------------------------------------------------------

//// _onPosChanged
//// -------------
////! This slot is called when the item is moved in the graph. [slot]
//
//void
//NsGraphOpItem::_onPosChanged()
//{
//    // Update meta information. Don't push on undo stack.
//
//    NsCmdSetOpPosition::exec(
//        NsCmdSetOpPosition::ArgsList() <<
//            NsCmdSetOpPosition::Args(op()->longName(), x(), y()));
//}

// -----------------------------------------------------------------------------

const QBrush
NsGraphOpItem::steppingBrush(QColor(0,128,230));//!< Bg if stepping. [static]

const QBrush
NsGraphOpItem::errorBrush(QColor(255,48,48));   //!< Bg if error. [static]

const QBrush
NsGraphOpItem::warningBrush(QColor("#F9FF3D")); //!< Bg if warning. [static]

//const QBrush
//NsGraphOpItem::disabledBrush(Qt::gray);  //!< Bg if disabled. [static]


//static const QColor(0,128,230);
//static const QColor(255,48,48);
//static const QColor("#F9FF3D");
//static const QColor NsSelectionColor("#F9FF3D");
