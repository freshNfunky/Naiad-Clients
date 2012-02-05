// -----------------------------------------------------------------------------
//
// NsGraphOpStateItem.cc
//
// Class for showing Op state in the graph view, source file.
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

#include "NsGraphOpStateItem.h"
#include "NsGraphOpItem.h"
#include "NsPreferences.h"
#include "NsOpObject.h"
#include "NsStringUtils.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// longName
// --------
//! DOCS

QString
NsGraphOpStateItem::longName() const
{ return op()->longName(); }

// -----------------------------------------------------------------------------

// NsGraphOpStateItem
// ------------------
//! CTOR.

NsGraphOpStateItem::NsGraphOpStateItem(NsGraphOpItem &parent,
                                       const qreal    diameter)
    : NsGraphItem(&parent)
    , _parent(&parent)
    , _diameter(diameter)
    , _shapePath(computeShapePath(_diameter))
    , _boundingRect(computeBoundingRect(_shapePath, 0.5*2))
    , _statePath(computeStatePath(_diameter))
{
#ifdef NS_GRAPH_VIEW_DEBUG
    setAcceptsHoverEvents(true);
#endif  // NS_GRAPH_VIEW_DEBUG
}


// op
// --
//! DOCS

NsOpObject*
NsGraphOpStateItem::op() const
{ return _parent->op(); }

// -----------------------------------------------------------------------------

// paint
// -----
//! Draws the item in the graph view.

void
NsGraphOpStateItem::paint(QPainter                       *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget                        *widget)
{
    const QPen shapePen(NsPreferences::instance()->graphViewOpLineColor(), 2);
    const QBrush shapeBrush(NsPreferences::instance()->graphViewOpBackground());
    painter->setPen(shapePen);
    painter->setBrush(shapeBrush);
    painter->drawPath(_shapePath);
    painter->fillPath(
       _statePath, 
       QBrush(NsPreferences::instance()->graphViewOpStateColor(op()->state())));

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
NsGraphOpStateItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QToolTip::showText(event->screenPos(),
                       tr("State") + ": " + op()->state());
    NsGraphItem::hoverEnterEvent(event);
}


// hoverLeaveEvent
// ---------------
//! DOCS

void
NsGraphOpStateItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QToolTip::hideText();
    NsGraphItem::hoverLeaveEvent(event);
}

#endif  // NS_GRAPH_VIEW_DEBUG

// -----------------------------------------------------------------------------

// computeShapePath
// ----------------
//! [static]

QPainterPath
NsGraphOpStateItem::computeShapePath(const qreal diameter)
{
    const qreal radius(0.5*diameter);
    QPainterPath p;
    p.addEllipse(-radius, -radius, diameter, diameter);
    return p;
}


// computeStatePath
// ----------------
//! [static]

QPainterPath
NsGraphOpStateItem::computeStatePath(const qreal diameter)
{
    const qreal radius(0.3*diameter);
    QPainterPath p;
    p.addEllipse(-radius, -radius, 2.*radius, 2.*radius);
    return p;
}

// -----------------------------------------------------------------------------
