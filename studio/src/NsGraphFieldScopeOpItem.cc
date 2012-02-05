// -----------------------------------------------------------------------------
//
// NsGraphFieldScopeOpItem.cc
//
// Class for showing a field scope in the graph view, source file.
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

#include "NsGraphFieldScopeOpItem.h"
#include "NsGraphOpStateItem.h"
#include "NsGraphOpItemFactory.h"
#include "NsGraphFieldInputPlugItem.h"
#include "NsSignatureStore.h"

// -----------------------------------------------------------------------------

const NsGraphOpItemKey NsGraphFieldScopeOpItem::key("FIELD_SCOPE");

// -----------------------------------------------------------------------------

// NsGraphFieldScopeOpItem
// -----------------------
//! CTOR.

NsGraphFieldScopeOpItem::NsGraphFieldScopeOpItem(
        const NsGraphOpItemArgs &args)
    : NsGraphOpItem(args)
    , _sigColor(Qt::white)
    , _shapePath(computeShapePath(_diameter(args.diameter())))
    , _boundingRect(computeBoundingRect(_shapePath))
{
    const qreal d(_diameter(diameter()));

    NsGraphOpStateItem *gosi(addStateItem(0.5*d));
    gosi->setPos(0.5*d + 0.75*gosi->diameter(), 0);

/*
    if (0 < op()->fieldInputCount()) {

        NsFieldInputPlugObject *fipo(
            op()->mutableFieldInput(op()->fieldInputNames().at(0)));

        if (0 != fipo) {
            NsGraphFieldInputPlugItem *fipi(0);
            fipi =
                new NsGraphFieldInputPlugItem(
                        fipo,
                        QPointF(-0.75*diameter(), 0.),
                        QPointF(0., 0.),    // Op origin.
                        16.,
                        *this);             // Child.

            _sigColor = NsSignatureStore::instance()->color(fipo->sigName());
            _sigPath = NsSignatureStore::instance()->shape(fipo->sigName(), d);
        }
    }
*/

    _setName(op()->longName());

#ifdef NS_GRAPH_VIEW_UNION_BRECT
    _boundingRect |= childrenBoundingRect(); // Union.
#endif
}

// -----------------------------------------------------------------------------

// nameChanged
// -----------
//! DOCS

void
NsGraphFieldScopeOpItem::nameChanged(const QString &oldOpInstance,
                                     const QString &newOpInstance)
{
    NsGraphOpItem::nameChanged(oldOpInstance, newOpInstance);  // Parent method.

    _setName(newOpInstance);
}

// -----------------------------------------------------------------------------

// paint
// -----
//! Draw the item in the graph view.

void
NsGraphFieldScopeOpItem::paint(QPainter                       *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget                        *widget)
{
    NsGraphOpItem::paint(painter, option, widget);

    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(Qt::black));
    painter->drawPath(_sigPath);

    painter->setPen(QPen(Qt::black, 1.));
    painter->setBrush(shapeBrush());
    painter->drawPath(_rectPath);

    painter->setPen(QPen(Qt::black, 1.));
    painter->setFont(font());
    painter->drawText(_textRect,
                      Qt::AlignCenter,
                      op()->longName());

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

// computeTextRect
// ---------------
//! [static]

QRectF
NsGraphFieldScopeOpItem::computeTextRect(const QFont   &font,
                                         const QString &text,
                                         const qreal    diameter,
                                         const qreal    widthPadding,
                                         const qreal    heightPadding)
{
    const QFontMetrics fontMetrics(font);
    const qreal textWidth(fontMetrics.width(text) + widthPadding);
    const qreal textHeight(fontMetrics.height() + heightPadding);

    return QRectF(-0.5*textWidth,     // Left, centered.
                  0.65*diameter,      // Top.
                  textWidth,          // Width.
                  textHeight);        // Height.
}


// computeShapePath
// ----------------
//! Compute the path used in the graph view. [static]

QPainterPath
NsGraphFieldScopeOpItem::computeShapePath(const qreal diameter,
                                          const qreal xRadius,
                                          const qreal yRadius)
{
    QPainterPath rectPath;
    rectPath.addRoundedRect(-0.5*diameter,
                            -0.5*diameter,
                            diameter,
                            diameter,
                            xRadius,
                            yRadius);

    return rectPath;
}


// computeBoundingRect
// -------------------
//! DOCS. [static]

QRectF
NsGraphFieldScopeOpItem::computeBoundingRect(const QPainterPath &shape,
                                             const qreal         padding)
{
    return shape.boundingRect().adjusted(-padding,
                                         -padding,
                                          padding,
                                          padding);
}

// -----------------------------------------------------------------------------

// _setName
// --------
//! Update the appearance of the Op item according to the Op name.

void
NsGraphFieldScopeOpItem::_setName(const QString &newName)
{
    prepareGeometryChange();
    _textRect = computeTextRect(QFont("sansserif", 8),
                                newName,
                                diameter());

    QPainterPath p;
    p.addRoundedRect(_textRect, 2., 2.);
    _rectPath = p;

    _boundingRect
        = computeBoundingRect(_shapePath).united(_rectPath.boundingRect());
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsGraphFieldScopeOpItem::_CreatorType
    NsGraphFieldScopeOpItem::_creator(*NsGraphOpItemFactory::instance());

// -----------------------------------------------------------------------------
