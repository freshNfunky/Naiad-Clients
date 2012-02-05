// -----------------------------------------------------------------------------
//
// NsGraphGlobalOpItem.cc
//
// Class showing the global Op in graph view, source file.
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

#include "NsGraphGlobalOpItem.h"
#include "NsPreferences.h"

// -----------------------------------------------------------------------------

const NsGraphOpItemKey NsGraphGlobalOpItem::key("GLOBAL_OP"); //!< Key. [static]

// -----------------------------------------------------------------------------

// NsGraphGlobalOpItem
// -------------------
//! CTOR.

NsGraphGlobalOpItem::NsGraphGlobalOpItem(
    const NsGraphOpItemArgs &args,
    const QString           &text,
    const QFont             &font,
    const qreal              wPadding,
    const qreal              hPadding)
    : NsGraphOpItem(args)
    , _text(text)
    , _font(font)
    , _wPadding(wPadding)
    , _hPadding(hPadding)
    , _textRect(computeTextRect(_text, _font, _wPadding, _hPadding))
    , _shapePath(computeShapePath(diameter()))
    , _boundingRect(computeBoundingRect(_shapePath, 0.5*4))
{}

// -----------------------------------------------------------------------------

// paint
// -----
//! Draws the item in the graph view.

void
NsGraphGlobalOpItem::paint(QPainter                       *painter,
                           const QStyleOptionGraphicsItem *option,
                           QWidget                        *widget)
{
    NsGraphOpItem::paint(painter, option, widget); // Parent method.

    painter->setPen(QPen(NsPreferences::instance()->graphViewOpTextColor()));
    painter->setFont(_font);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->drawText(_textRect, Qt::AlignCenter, _text);

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
//! Compute size and position of rectangle in which Op name is drawn. [static]

QRectF
NsGraphGlobalOpItem::computeTextRect(const QString &text,
                                     const QFont   &font,
                                     const qreal    wPadding,
                                     const qreal    hPadding)
{
    const QFontMetrics fontMetrics(font);
    const qreal textWidth(fontMetrics.width(text) + wPadding);
    const qreal textHeight(fontMetrics.height() + hPadding);

    return QRectF(-0.5*textWidth,       // Left.
                  -0.5*textHeight,      // Top.
                  textWidth,            // Width.
                  textHeight);          // Height.
}


// computeShapePath
// ----------------
//! Compute the path used in the graph view.

QPainterPath
NsGraphGlobalOpItem::computeShapePath(const qreal diameter)
{
    QPainterPath p;
    p.addEllipse(QPointF(0., 0.), 0.5*diameter, 0.5*diameter);
    return p;
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsGraphGlobalOpItem::_CreatorType
    NsGraphGlobalOpItem::_creator(*NsGraphOpItemFactory::instance());

// -----------------------------------------------------------------------------
