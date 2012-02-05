// -----------------------------------------------------------------------------
//
// NsGraphBodyOpItem.cc
//
// Class showing a body Op in graph view, source file.
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

#include "NsGraphBodyOpItem.h"
#include "NsGraphOpStateItem.h"
#include "NsGraphOpNameItem.h"
#include "NsGraphInputPlugItem.h"
#include "NsGraphBodyInputPlugItem.h"
#include "NsGraphFieldInputPlugItem.h"
#include "NsGraphOutputPlugItem.h"
#include "NsGraphBodyOutputPlugItem.h"
#include "NsGraphFieldOutputPlugItem.h"
#include "NsQuery.h"

// -----------------------------------------------------------------------------

const NsGraphOpItemKey NsGraphBodyOpItem::key("BODY_OP");   //!< Key. [static]

// -----------------------------------------------------------------------------

// NsGraphBodyOpItem
// -----------------
//! CTOR.

NsGraphBodyOpItem::NsGraphBodyOpItem(const NsGraphOpItemArgs &args)
    : NsGraphOpItem(args)
    , _shapePath(computeShapePath(args.diameter()))
#ifndef NS_GRAPH_VIEW_NAME_ITEMS
    , _textRect(computeTextRect(op()->longName(), 
                                QFont("sansserif", 12, QFont::DemiBold),
                                args.diameter(),
                                8, 8))                                            
    , _textShapePath(computeTextShapePath(_textRect, 2, 2))
    , _boundingRect(    // Union
        computeBoundingRect(_textShapePath, 0.5*4) | 
        computeBoundingRect(_shapePath, 0.5*4))
#else
    , _boundingRect(computeBoundingRect(_shapePath, 
                                        0.5*4))
#endif
{
    NsGraphOpStateItem *gosi(addStateItem(stateItemDiameter()));
    gosi->setPos(stateItemPos());

    addInputPlugItems(*op());
    addOutputPlugItems(*op());

#ifdef NS_GRAPH_VIEW_NAME_ITEMS
    NsGraphOpNameItem *goni(addNameItem());
    goni->setPos(nameItemPos(*goni));
#endif

#ifdef NS_GRAPH_VIEW_UNION_BRECT
    _boundingRect |= childrenBoundingRect(); // Union.
#endif
}

// -----------------------------------------------------------------------------

void
NsGraphBodyOpItem::paint(QPainter                       *painter,
                         const QStyleOptionGraphicsItem *option,
                         QWidget                        *widget)
{
    NsGraphOpItem::paint(painter, option, widget);

#ifndef NS_GRAPH_VIEW_NAME_ITEMS
    //painter->setClipRect(_boundingRect());
    painter->setPen(QPen(Qt::black, 2.));
    painter->setBrush(QBrush(Qt::white));
    painter->drawPath(_textShapePath);
    painter->setFont(QFont("sansserif", 12, QFont::DemiBold));
    painter->setRenderHint(QPainter::TextAntialiasing);
    painter->drawText(_textRect, Qt::AlignCenter, op()->longName());
#endif
}

// -----------------------------------------------------------------------------

// addInputPlugItems
// -----------------
//! Create the input plugs associated with the Op.

void
NsGraphBodyOpItem::addInputPlugItems(NsOpObject &op)
{
    const qreal plugDiam(plugDiameter(diameter()));
    const qreal plugDist(plugDistance(diameter(), plugDiam));
    const qreal dThetaRad(plugDeltaAngle(diameter(), 1.25*plugDiam));
    const qreal offsetRad(plugOffsetAngle(inputOffsetRad, 
                                          dThetaRad, 
                                          op.inputCount()));

    int index(0);

    // Body inputs.

    foreach (NsBodyInputPlugObject *bipo, op.mutableBodyInputs()) {
        const qreal thetaRad(plugAngle(offsetRad, dThetaRad, index));
        const QPointF plugPos(plugPosition(plugDist, thetaRad));

        createBodyInputPlugItem(
            bipo,
            *this,  // Child.
            plugDiam,
            plugPos,
            plugLine(thetaRad, diameter(), plugPos),
            plugTextPosition(plugDist, thetaRad),
            _circularPos(0.5*diameter(), thetaRad),
            plugText(bipo->name()));

        ++index;
    }

    // Field inputs.

    foreach (NsFieldInputPlugObject *fipo, op.mutableFieldInputs()) {
        const qreal thetaRad(plugAngle(offsetRad, dThetaRad, index));
        const QPointF plugPos(plugPosition(plugDist, thetaRad));

        createFieldInputPlugItem(
            fipo,
            *this,  // Child.
            plugDiam,
            plugPos,
            plugLine(thetaRad, diameter(), plugPos),
            plugTextPosition(plugDist, thetaRad),
            _circularPos(0.5*diameter(), thetaRad),
            plugText(fipo->name()));             

        ++index;
    }
}


// addOutputPlugItems
// ------------------
//! Create the output plugs associated with the Op.

void
NsGraphBodyOpItem::addOutputPlugItems(NsOpObject &op)
{
    const qreal plugDiam(plugDiameter(diameter()));
    const qreal plugDist(plugDistance(diameter(), plugDiam));
    const qreal dThetaRad(plugDeltaAngle(diameter(), 1.25*plugDiam));
    const qreal offsetRad(plugOffsetAngle(outputOffsetRad, 
                                          dThetaRad, 
                                          op.outputCount()));

    // NB: Use reverse indexing direction to get 
    // correct ordering of output plug items.

    int index(op.outputCount() - 1);

    // Body outputs.

    foreach (NsBodyOutputPlugObject *bopo, op.mutableBodyOutputs()) {
        const qreal thetaRad(plugAngle(offsetRad, dThetaRad, index));
        const QPointF plugPos(plugPosition(plugDist, thetaRad));
        createBodyOutputPlugItem(
            bopo,
            *this,  // Child.
            plugDiam,
            plugPos,
            plugLine(thetaRad, diameter(), plugPos),
            plugTextPosition(plugDist, thetaRad),
            _circularPos(0.5*diameter(), thetaRad),
            plugText(bopo->name()));             // Child.
        --index;
    }

    // Field outputs.

    foreach (NsFieldOutputPlugObject *fopo, op.mutableFieldOutputs()) {
        const qreal thetaRad(plugAngle(offsetRad, dThetaRad, index));
        const QPointF plugPos(plugPosition(plugDist, thetaRad));
        createFieldOutputPlugItem(
            fopo,
            *this,  // Child.
            plugDiam,
            plugPos,
            plugLine(thetaRad, diameter(), plugPos),
            plugTextPosition(plugDist, thetaRad),
            _circularPos(0.5*diameter(), thetaRad),
            plugText(fopo->name()));             
        --index;
    }
}

// -----------------------------------------------------------------------------

const qreal NsGraphBodyOpItem::inputOffsetRad(1.5*M_PI);  //!< 270 deg. [static]
const qreal NsGraphBodyOpItem::outputOffsetRad(0.5*M_PI); //!< 90 deg.  [static]

// -----------------------------------------------------------------------------

QPointF
NsGraphBodyOpItem::nameItemPos(const NsGraphOpNameItem &item) const
{ return QPointF(0.6*diameter(), -0.5*item.height()); }

// -----------------------------------------------------------------------------

// computeShapePath
// ----------------
//! Compute the path used in the graph view. [static]

QPainterPath
NsGraphBodyOpItem::computeShapePath(const qreal diameter)
{
    QPainterPath p;
    p.addEllipse(QPointF(0., 0.), 0.5*diameter, 0.5*diameter);
    return p;
}

QRectF
NsGraphBodyOpItem::computeTextRect(const QString &text,
                                   const QFont   &font,
                                   const qreal    diameter,
                                   const qreal    widthPadding,
                                   const qreal    heightPadding)
{
    const QFontMetrics fontMetrics(font);
    const qreal textWidth(fontMetrics.width(text) + widthPadding);
    const qreal textHeight(fontMetrics.height() + heightPadding);

    return QRectF(0.6*diameter,       // Left.
                  -0.5*textHeight,    // Top.
                  textWidth,          // Width.
                  textHeight);        // Height.
}

QPainterPath
NsGraphBodyOpItem::computeTextShapePath(const QRectF &textRect,
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
NsGraphBodyOpItem::_setName(const QString &newName)
{
#ifndef NS_GRAPH_VIEW_NAME_ITEMS
    prepareGeometryChange();
    const QFont textFont("sansserif", 12, QFont::DemiBold);
    _textRect = computeTextRect(newName, textFont, diameter(), 8, 8);
    _textShapePath = computeTextShapePath(_textRect, 2, 2);
    _boundingRect |= computeBoundingRect(_textShapePath, 0.5*4);
#endif
}

// -----------------------------------------------------------------------------

//! Static creator instance. [static]
const NsGraphBodyOpItem::_CreatorType
    NsGraphBodyOpItem::_creator(*NsGraphOpItemFactory::instance());

// -----------------------------------------------------------------------------
