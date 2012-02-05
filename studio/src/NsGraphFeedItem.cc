// -----------------------------------------------------------------------------
//
// NsGraphFeedItem.cc
//
// Class for showing feeds in graph view, source file.
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

#include "NsGraphFeedItem.h"
#include "NsGraphScene.h"
#include "NsGraphInputPlugItem.h"
#include "NsGraphPlugItem.h"
#include "NsGraphOpItem.h"
#include "NsGraphBodyItem.h"
#include "NsGraphCallback.h"
#include "NsPreferences.h"
#include "NsCmdCentral.h"
#include "NsCmdSelectFeed.h"
#include "NsBodyCache.h"
#include "NsBodyObject.h"
#include "NsQuery.h"
#include <QtGui>
#include <QDebug>

// -----------------------------------------------------------------------------

// ~NsGraphFeedItem
// ----------------
//! DTOR.

NsGraphFeedItem::~NsGraphFeedItem()
{
    qDebug() << "~NsGraphFeedItem";
    //_clearBodyItems();
}


// longName
// --------
//! Provides a long name for the feed. Uses the long name of the input plug.

QString
NsGraphFeedItem::longName() const
{
    return _input->plugObject()->longName();
}


// setSelected
// -----------
//! Select this item using a command.

void
NsGraphFeedItem::setSelected(const bool   selected,
                             NsUndoStack *undoStack,
                             const bool   merge)
{
    if (_input->plugObject()->isSelected() != selected) {
        if (0 != undoStack) {
            NsCmdSelectFeed::exec(
                NsCmdSelectFeed::ArgsList() <<
                    NsCmdSelectFeed::Args(longName(), selected),
                *undoStack,
                merge);
        }
        else {
            NsCmdSelectFeed::exec(
                NsCmdSelectFeed::ArgsList() <<
                    NsCmdSelectFeed::Args(longName(), selected));
        }
    }
}


// setSelected
// -----------
//! Select this item using a command.

void
NsGraphFeedItem::setSelected(const bool selected, QUndoCommand &parent)
{
    if (_input->plugObject()->isSelected() != selected) {
        NsCmdSelectFeed::exec(
            NsCmdSelectFeed::ArgsList() <<
                NsCmdSelectFeed::Args(longName(), selected),
            parent);
    }
}


bool
NsGraphFeedItem::isSelected() const
{
    return _input->plugObject()->isSelected();
}

// -----------------------------------------------------------------------------

// NsGraphFeedItem
// ---------------
//! CTOR.

NsGraphFeedItem::NsGraphFeedItem(NsGraphInputPlugItem &input,
                                 NsGraphPlugItem      &plug,
                                 QGraphicsItem        *parent,
                                 const qreal           bodyItemDiameter)
    : NsGraphItem(parent, _isSelectable, _isMovable)
    , _input(&input)
    , _plug(&plug)
    , _style(Normal)
    , _bodyItemDiameter(bodyItemDiameter)
{
#ifdef NS_GRAPH_VIEW_DEBUG
    setAcceptsHoverEvents(true);
#endif

    //setFlag(ItemIsSelectable, true);
    //setFlag(ItemIsMovable, false);
    setZValue(-1.); // Place feeds behind all other items.

    //_setSelected(input.plugObject()->isSelected());
    _setPath();
    _updateBodyItems(_grabBodies(queryCurrentVisibleFrameTimeBundle().frame)); 
}

// -----------------------------------------------------------------------------

//! [slot]

void
NsGraphFeedItem::onCurrentVisibleFrameChanged(const int  cvf, 
                                              const bool update3DView, 
                                              const bool success)
{ 
    Q_UNUSED(update3DView);

    if (success) {
        currentVisibleFrameChanged(cvf); 
    }
}

void
NsGraphFeedItem::liveBodyCacheChanged()
{ 
    _updateBodyItems(_grabBodies(queryCurrentVisibleFrameTimeBundle().frame)); 
}

void
NsGraphFeedItem::empBodyCacheChanged()
{ 
    _updateBodyItems(_grabBodies(queryCurrentVisibleFrameTimeBundle().frame)); 
}

void
NsGraphFeedItem::stateChanged()
{
    _updateBodyItems(_grabBodies(queryCurrentVisibleFrameTimeBundle().frame)); 
}

// -----------------------------------------------------------------------------

#ifdef NS_GRAPH_VIEW_DEBUG

// hoverEnterEvent
// ---------------
//! DOCS

void
NsGraphFeedItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QToolTip::showText(
        event->screenPos(),
        QString(
           tr("Feed") + "\n" +
           tr("Input")       + ": " + _input->plugObject()->longName() + "\n" +
           tr("Plug")        + ": " + _plug->plugObject()->longName()  + "\n" +
           tr("Long name")   + ": " + longName()      + "\n"));
    NsGraphItem::hoverEnterEvent(event);
}


// hoverLeaveEvent
// ---------------
//! DOCS

void
NsGraphFeedItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QToolTip::hideText();
    NsGraphItem::hoverLeaveEvent(event);
}

#endif // NS_GRAPH_VIEW_DEBUG

// -----------------------------------------------------------------------------

// paint
// -----
//! DOCS.

void
NsGraphFeedItem::paint(QPainter                       *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget                        *widget)
{
    switch (_style) {
    default:
    case Normal:
        if (_input->plugObject()->isSelected() || isItemSelected()) {
            painter->strokePath(
                _shapePath, 
                QPen(NsPreferences::instance()->graphViewFeedLineColor(), 4));
            painter->strokePath(
                _shapePath, 
                QPen(NsPreferences::instance()->graphViewSelectionColor(), 2));
        }
        else {
            painter->strokePath(
                _shapePath, 
                QPen(NsPreferences::instance()->graphViewFeedLineColor(), 2));
        }
        break;
    case Replaced:
        painter->strokePath(
            _shapePath, 
            QPen(NsPreferences::instance()->graphViewFeedReplacedLineColor(), 2));
        break;
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

// computeShapePath
// ----------------
//! DOCS [static]

QPainterPath
NsGraphFeedItem::computeShapePath(const QPointF &p1, 
                                  const QPointF &p2, 
                                  const bool     dummy)
{
    const qreal dx12(p1.x() - p2.x());
    const qreal dy12(p1.y() - p2.y());
    const qreal dx21(p2.x() - p1.x());
    const qreal dy21(p2.y() - p1.y());

    QPointF ctrl1;
    QPointF ctrl2;

    if (dummy) {
        if (p1.x() > p2.x()) {
            ctrl1 = QPointF(p1.x() + 0.5*dx21, p1.y());
            ctrl2 = QPointF(p2.x() + 0.5*dx12, p2.y());
        }
        else {
            ctrl1 = QPointF(p1.x() + 0.5*dx12, p1.y());
            ctrl2 = QPointF(p2.x() + 0.5*dx21, p2.y());
        }
    }
    else {
        if (p1.y() > p2.y()) {
            ctrl1 = QPointF(p1.x(), p1.y() + 0.5*dy21);
            ctrl2 = QPointF(p2.x(), p2.y() + 0.5*dy12);
        }
        else {
            ctrl1 = QPointF(p1.x(), p1.y() + 0.5*dy12);
            ctrl2 = QPointF(p2.x(), p2.y() + 0.5*dy21);
        }
    }

    QPainterPath path(p1);
    path.cubicTo(ctrl1, ctrl2, p2);
    return path;
}

// -----------------------------------------------------------------------------

//void
//NsGraphFeedItem::_setSelected(bool selected)
//{
//    if (isSelected() != selected) {
//        QGraphicsObject::setSelected(selected);
//    }
//}


void
NsGraphFeedItem::_setPath()
{
    // Create new path from plug positions.

    prepareGeometryChange();

    _shapePath = 
        computeShapePath(
            mapToScene(mapFromItem(_input->opItem(), _input->plugPos())),
            mapToScene(mapFromItem(_plug->opItem(), _plug->plugPos())),
            _input->plugObject()->isDummy());

                    
    _boundingRect = 
        computeBoundingRect(_shapePath, 0.5*4);

    _updateBodyItemPositions();
}


QList<NsBodyObject*>
NsGraphFeedItem::_grabBodies(const int cvf)
{
    const bool activeOp(!("INACTIVE" == _plug->opItem()->op()->state()));
    const bool liveFrame(queryIsFrameLive(cvf));

    if (liveFrame && activeOp) {
        return _plug->plugObject()->mutableLiveBodies();
    }

    return _plug->opObject()->mutableCachedBodies();       
}


// _updateBodyItems
// ----------------
//! DOCS

void
NsGraphFeedItem::_updateBodyItems(const QList<NsBodyObject*> &bodies)
{
    _clearBodyItems();  // Clear existing body items no matter what.

    foreach (NsBodyObject *body, bodies) {
        _bodyItems.insert(
            body->handle(),
            _createBodyItem(body, _bodyItemDiameter, this));
    }

    _updateBodyItemPositions();
}


// _createBodyItem
// ---------------
//! DOCS

NsGraphBodyItem*
NsGraphFeedItem::_createBodyItem(NsBodyObject    *body, 
                                 const qreal      diameter, 
                                 NsGraphFeedItem *parent)
{
    NsGraphBodyItem *bodyItem(
        new NsGraphBodyItem(*body, diameter, *parent)); // Child.

    connect(body,     SIGNAL(selectionChanged()),
            bodyItem, SLOT(onSelectionChanged()));

    // Make sure body item is removed when body object is destroyed.

    connect(body,
            SIGNAL(bodyObjectDestroyed(NsBodyObject*)),
            SLOT(onBodyObjectDestroyed(NsBodyObject*)));

    return bodyItem;
}

// _removeBodyItem
// ---------------
//! Remove a body item.

void
NsGraphFeedItem::_removeBodyItem(NsBodyObject *bo)
{
    _BodyItemHashType::iterator iter(
        _bodyItems.find(bo->handle()));

    if (_bodyItems.end() != iter && 0 != graphScene()) {
        graphScene()->eraseItem(iter.value());
        _bodyItems.erase(iter);  // Remove association.
    }    
}


// _clearBodyItems
// ---------------
//! Remove all body items.

void
NsGraphFeedItem::_clearBodyItems()
{
    qDebug() << "NsGraphFeedItem::_clearBodyItems";

    if (0 != graphScene()) {
        foreach (NsGraphBodyItem *gbi, _bodyItems) {
            graphScene()->eraseItem(gbi);
            //delete gbi;     // Remove item!
        }

        _bodyItems.clear(); // Remove associations.
    }
}


// _updateBodyPositions
// --------------------
//! Update body item positions so that the bodies lie on the path representing
//! the feed.

void
NsGraphFeedItem::_updateBodyItemPositions()
{
    if (!_bodyItems.empty()) {
        const qreal increment(0.9/(_bodyItems.count() + 1));
        qreal offset(0.05);

        foreach (NsGraphBodyItem *gbi, _bodyItems) {
            offset += increment;
            gbi->setPos(_shapePath.pointAtPercent(offset));
        }
    }
}

// -----------------------------------------------------------------------------
