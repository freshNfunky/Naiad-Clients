// -----------------------------------------------------------------------------
//
// NsGraphBodyItem.cc
//
// Class for showing bodies in graph view, source file.
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

#include "NsGraphBodyItem.h"
#include "NsBodyObject.h"
#include "NsGraphFeedItem.h"
#include "NsGraphOutputPlugItem.h"
#include "NsSignatureStore.h"
#include "NsCmdSelectBody.h"
#include "NsPreferences.h"
#include "NsQuery.h"

// -----------------------------------------------------------------------------

// longName
// --------
//! DOCS.

QString
NsGraphBodyItem::longName() const
{
    return body()->longName();
}


// setSelected
// -----------
//! Select this item using a command.

void
NsGraphBodyItem::setSelected(const bool      selected,
                             NsUndoStack *undoStack,
                             const bool      merge)
{
    if (body()->isSelected() != selected) {
        if (0 != undoStack) {
            NsCmdSelectBody::exec(
                NsCmdSelectBody::ArgsList() <<
                    NsCmdSelectBody::Args(body()->longName(),selected),
                    *undoStack, merge);
        }
        else {
            NsCmdSelectBody::exec(
                NsCmdSelectBody::ArgsList() << 
                    NsCmdSelectBody::Args(body()->longName(), selected));
        }
    }
}


void
NsGraphBodyItem::setSelected(bool selected, QUndoCommand &parent)
{
    if (body()->isSelected() != selected) {
        NsCmdSelectBody::exec(
            NsCmdSelectBody::ArgsList() <<
                NsCmdSelectBody::Args(body()->longName(), selected),
            parent);
    }
}


bool
NsGraphBodyItem::isSelected() const
{
    return body()->isSelected();
}

// -----------------------------------------------------------------------------

// NsGraphBodyItem
// ---------------
//! CTOR.

NsGraphBodyItem::NsGraphBodyItem(NsBodyObject    &body,
                                 const qreal      diameter,
                                 NsGraphFeedItem &parent)
    : NsGraphItem(&parent, _isSelectable, _isMovable)
    , _body(&body)
    , _diameter(diameter)
    , _shapePath(computeShapePath(diameter))
    , _boundingRect(computeBoundingRect(_shapePath))
    , _sigPath(NsSignatureStore::instance()->shape(body.sigName(), _diameter))
{
#ifdef NS_GRAPH_VIEW_DEBUG
    setAcceptsHoverEvents(true);
#endif
}

// -----------------------------------------------------------------------------

void
NsGraphBodyItem::paint(QPainter                       *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget                        *widget)
{
    const QPainterPath shapePath = shape();
    const QColor sigColor = 
        NsPreferences::instance()->graphViewSignatureColor(_body->sigName());

    painter->fillPath(
        shapePath, 
        QBrush(NsPreferences::instance()->graphViewBodyBackground()));
    painter->fillPath(
        _sigPath, 
        QBrush(sigColor));

    if (body()->isSelected() || isItemSelected()) {
        painter->strokePath(
            shapePath, 
            QPen(NsPreferences::instance()->graphViewBodyLineColor(), 6.));
        painter->strokePath(
            shapePath, 
            QPen(NsPreferences::instance()->graphViewSelectionColor(), 3.));
    }
    else {
        painter->strokePath(
            shapePath, 
            QPen(NsPreferences::instance()->graphViewBodyLineColor(), 3.));
    }

    if (!body()->isLive()) {
        painter->setFont(QFont("Sans", 0.5*diameter()));
        painter->setPen(
            QPen(NsPreferences::instance()->graphViewBodyTextColor()));
        painter->drawText(QPointF(0.85*diameter(), 0.25*diameter()), "C");
    }

    // Draw outer ring to denote evolving bodies.

    if (body()->isEvolving()) {
        painter->scale(1.5, 1.5);
        painter->strokePath(
            shapePath, 
            QPen(NsPreferences::instance()->graphViewBodyLineColor(), 1.));
        painter->scale(0.67, 0.67);     // 1./1.5
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
NsGraphBodyItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QToolTip::showText(
        event->screenPos(),
        QString(
            tr("Body") + "\n" +
            tr("Long name")   + ": " + longName() + "\n" +
            (body()->isLive() ? "[Live]" : "[Cached]") + "\n" +
            (body()->isEvolving() ? "[Evolving]" : "")));
    NsGraphItem::hoverEnterEvent(event);
}


// hoverLeaveEvent
// ---------------
//! DOCS

void
NsGraphBodyItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QToolTip::hideText();
    NsGraphItem::hoverLeaveEvent(event);
}

#endif  // NS_GRAPH_VIEW_DEBUG

// -----------------------------------------------------------------------------

// computeShapePath
// ----------------
//! Compute the path used in the graph view.

QPainterPath
NsGraphBodyItem::computeShapePath(const qreal diameter)
{
    QPainterPath p;
    p.addEllipse(-0.5*diameter, -0.5*diameter, diameter, diameter);
    return p;
}

// -----------------------------------------------------------------------------
