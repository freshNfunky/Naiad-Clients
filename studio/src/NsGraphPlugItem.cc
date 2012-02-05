// -----------------------------------------------------------------------------
//
// NsGraphPlugItem.cc
//
// Abstract base class for Op plug items, source file.
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

#include "NsGraphPlugItem.h"
#include "NsGraphOpItem.h"
#include "NsSignatureStore.h"
#include "NsQuery.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// ~NsGraphPlugItem
// ----------------
//! DTOR.

NsGraphPlugItem::~NsGraphPlugItem()
{ qDebug() << "~NsGraphPlugItem"; }

// -----------------------------------------------------------------------------

NsOpObject*
NsGraphPlugItem::opObject() const
{
    return _opItem->op();
}

// -----------------------------------------------------------------------------

// NsGraphPlugItem
// ---------------
//! CTOR.

NsGraphPlugItem::NsGraphPlugItem(NsPlugObject  *po,
                                 NsGraphOpItem &parent,
                                 const qreal    diameter)
    : NsGraphItem(&parent, _isSelectable, _isMovable)
    , _plug(po)
    , _opItem(&parent)
    , _diameter(diameter)
{
#ifdef NS_GRAPH_VIEW_DEBUG
    setAcceptsHoverEvents(true);
#endif
}

// -----------------------------------------------------------------------------

// paint
// -----
//! DOCS

void
NsGraphPlugItem::paint(QPainter                       *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget                        *widget)
{
#ifdef NS_GRAPH_VIEW_DEBUG
    // For debugging - draw bounding rect

    painter->setBrush(Qt::transparent);
    painter->setPen(QPen(QColor(255, 0, 0, 127), 0));
    painter->drawRect(boundingRect());

    painter->setBrush(Qt::transparent);
    painter->setPen(QPen(QColor(0, 255, 0, 127), 0));
    painter->drawPath(shape());
#endif  // NS_GRAPH_VIEW_DEBUG
}

// -----------------------------------------------------------------------------

// paintText
// ---------
//! [static]

void
NsGraphPlugItem::paintText(QPainter      *painter,
                           const QPen    &textPen,
                           const QFont   &font,
                           const QRectF  &textRect,
                           const QString &text)
{
    painter->setPen(textPen);
    painter->setFont(font);
    painter->drawText(textRect, Qt::AlignCenter, text);
}


// paintUnsmacked
// --------------
//! [static]

void
NsGraphPlugItem::paintUnsmacked(QPainter           *painter,
                                const QPainterPath &shapePath,
                                const QLineF       &line,
                                const QPen         &shapePen,
                                const QBrush       &shapeBrush,
                                const QPainterPath &sigPath,
                                const QPen         &sigPen,
                                const QBrush       &sigBrush)
{
    painter->setPen(shapePen);
    painter->drawLine(line);
    painter->setBrush(shapeBrush);
    painter->drawPath(shapePath);

    painter->setPen(sigPen);
    painter->setBrush(sigBrush);
    painter->drawPath(sigPath);
}


// paintSmacked
// ------------
//! [static]

void
NsGraphPlugItem::paintSmacked(QPainter          *painter,
                              const QPainterPath &shapePath,
                              const QPen         &shapePen,
                              const QBrush       &shapeBrush)
{
    painter->setPen(shapePen);
    painter->setBrush(shapeBrush);
    painter->drawPath(shapePath);
}


// -----------------------------------------------------------------------------

#ifdef NS_GRAPH_VIEW_DEBUG

// toolTip
// -------
//! DOCS [static]

QString
NsGraphPlugItem::toolTip(const QString &type,
                         const QString &name,
                         const QString &sig,
                         const bool     group)
{
    return QString(tr("Type") + ": " + type + "\n" +
                   tr("Name") + ": " + name + "\n" +
                   tr("Signature") + ": " + sig +
                   (group ? (" [" + tr("group") + "]") : ""));
}


// hoverEnterEvent
// ---------------
//! DOCS

void
NsGraphPlugItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    QToolTip::showText(
        event->screenPos(),
        toolTip(plugType(), // Virtual
                plugObject()->name(),
                plugObject()->sigName(),
                plugObject()->isGroup()));
    NsGraphItem::hoverEnterEvent(event);    // Parent method.
}


// hoverLeaveEvent
// ---------------
//! DOCS

void
NsGraphPlugItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    QToolTip::hideText();
    NsGraphItem::hoverLeaveEvent(event);    // Parent method.
}

#endif  // NS_GRAPH_VIEW_DEBUG

// -----------------------------------------------------------------------------

// computeBodyPlugPath
// -------------------
//! DOCS [static]

QPainterPath
NsGraphPlugItem::computeBodyPlugPath(const QPointF &pos,
                                     const qreal    diameter)
{
    QPainterPath p;
    p.addEllipse(
        QRectF(pos.x() - 0.5*diameter, 
               pos.y() - 0.5*diameter, 
               diameter, 
               diameter));
    return p;    
}


// computeFieldPlugPath
// --------------------
//! DOCS [static]

QPainterPath
NsGraphPlugItem::computeFieldPlugPath(const QPointF &pos, 
                                      const qreal   diameter, 
                                      const qreal   xRadius, 
                                      const qreal   yRadius)
{
    QPainterPath p;
    p.addRoundedRect(
        QRectF(pos.x() - 0.5*diameter + 0.5*xRadius,
               pos.y() - 0.5*diameter + 0.5*yRadius,
               diameter - xRadius,
               diameter - yRadius),
        xRadius, 
        yRadius);
    return p; 
}


// computeSigPath
// --------------
//! DOCS [static]

QPainterPath
NsGraphPlugItem::computeSigPath(const NsPlugObject &plug, 
                                const QPointF      &pos,
                                const qreal         diameter)
{
    return NsSignatureStore::instance()->shape(
        plug.sigName(), diameter).translated(pos);
}


// computeTextRect
// ---------------
//! DOCS [static]

QRectF
NsGraphPlugItem::computeTextRect(const QString &text,
                                 const QFont   &font,
                                 const QPointF &pos)
{
    const QFontMetrics fontMetrics(font);

#if 0
    const QRectF tbr(fontMetrics.tightBoundingRect(text));

    return QRectF(pos.x() - 0.5*tbr.width(),    // Left.
                  pos.y() - 0.5*tbr.height(),   // Top.
                  tbr.width(),                  // Width.
                  tbr.height());                // Height.
#endif

    const qreal textWidth(fontMetrics.width(text));
    const qreal textHeight(fontMetrics.height());

    return QRectF(pos.x() - 0.5*textWidth,    // Left.
                  pos.y() - 0.5*textHeight,   // Top.
                  textWidth,                  // Width.
                  textHeight);                // Height.
}


// computeBoundingRect
// -------------------
//! DOCS [static]

QRectF
NsGraphPlugItem::computeBoundingRect(const QPainterPath &shape, 
                                     const QRectF       &textRect, 
                                     const qreal         pad)
{
    return shape.boundingRect().united(textRect).adjusted(-pad, -pad, pad, pad);
}
    
