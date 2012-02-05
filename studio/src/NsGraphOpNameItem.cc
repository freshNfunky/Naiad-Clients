// -----------------------------------------------------------------------------
//
// NsGraphOpNameItem.cc
//
// Class for showing an Op name in the graph view, source file.
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

#include "NsGraphOpNameItem.h"
#include "NsGraphOpItem.h"
#include "NsPreferences.h"
#include "NsOpObject.h"

// -----------------------------------------------------------------------------

QString
NsGraphOpNameItem::longName() const
{
    return op()->longName();
}

// -----------------------------------------------------------------------------

// NsGraphOpNameItem
// -----------------
//! CTOR.

NsGraphOpNameItem::NsGraphOpNameItem(NsGraphOpItem &parent,
                                     const qreal    xRadius,
                                     const qreal    yRadius,
                                     const qreal    wPadding,
                                     const qreal    hPadding)
    : NsGraphItem(&parent)
    , _parent(&parent)
    , _font("sansserif", 12, QFont::DemiBold)
    , _xRadius(xRadius)
    , _yRadius(yRadius)
    , _wPadding(wPadding)
    , _hPadding(hPadding)
    , _textRect(computeTextRect(op()->longName(), _font, _wPadding, _hPadding))
    , _shapePath(computeShapePath(_textRect, _xRadius, _yRadius))
    , _boundingRect(computeBoundingRect(_shapePath, 0.5*2))
{}


// op
// --
//! DOCS

NsOpObject*
NsGraphOpNameItem::op() const
{ 
    return _parent->op(); 
}

// -----------------------------------------------------------------------------

// onNameChanged
// -------------
//! DOCS [slot]

void
NsGraphOpNameItem::onNameChanged(const QString &oldOpInstance,
                                 const QString &newOpInstance)
{
    Q_UNUSED(oldOpInstance);
    nameChanged(newOpInstance);
}

// -----------------------------------------------------------------------------

void
NsGraphOpNameItem::nameChanged(const QString &newName)
{
    _setName(newName);
}

// -----------------------------------------------------------------------------

void
NsGraphOpNameItem::paint(QPainter                       *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget                        *widget)
{
    const QPen shapePen(NsPreferences::instance()->graphViewOpLineColor(), 2);
    const QBrush shapeBrush(NsPreferences::instance()->graphViewOpBackground());
    painter->setPen(shapePen);
    painter->setBrush(shapeBrush);
    painter->drawPath(_shapePath);
    painter->setFont(_font);
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->drawText(_textRect, Qt::AlignCenter, op()->longName());

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
//! DOCS. [static]

QRectF
NsGraphOpNameItem::computeTextRect(const QString &text,
                                   const QFont   &font,
                                   const qreal    widthPadding,
                                   const qreal    heightPadding)
{
    const QFontMetrics fontMetrics(font);
    const qreal textWidth(fontMetrics.width(text) + widthPadding);
    const qreal textHeight(fontMetrics.height() + heightPadding);

    return QRectF(0, //-0.5*textWidth,     // Left, centered.
                  0, //0.65*diameter,      // Top.
                  textWidth,          // Width.
                  textHeight);        // Height.
}


// computeShapePath
// ----------------
//! [static]

QPainterPath
NsGraphOpNameItem::computeShapePath(const QRectF &textRect, 
                                    const qreal   xRadius, 
                                    const qreal   yRadius)
{
    QPainterPath p;
    p.addRoundedRect(textRect, xRadius, yRadius);
    return p;
}

// -----------------------------------------------------------------------------

// _setName
// --------
//! DOCS

void
NsGraphOpNameItem::_setName(const QString &newName)
{
    prepareGeometryChange();
    _textRect = computeTextRect(newName, _font, _wPadding, _hPadding);
    _shapePath = computeShapePath(_textRect, _xRadius, _yRadius);
    _boundingRect = computeBoundingRect(_shapePath, 0.5*2);
}
