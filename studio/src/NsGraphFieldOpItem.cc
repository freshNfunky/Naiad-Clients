// -----------------------------------------------------------------------------
//
// NsGraphFieldOpItem.cc
//
// Class showing a field Op in graph view, source file.
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

#include "NsGraphFieldOpItem.h"
#include "NsGraphOpNameItem.h"
#include "NsGraphInputPlugItem.h"
#include "NsGraphBodyInputPlugItem.h"
#include "NsGraphFieldInputPlugItem.h"
#include "NsGraphOutputPlugItem.h"
#include "NsGraphBodyOutputPlugItem.h"
#include "NsGraphFieldOutputPlugItem.h"
#include <QDebug>

// -----------------------------------------------------------------------------

const NsGraphOpItemKey NsGraphFieldOpItem::key("FIELD_OP");  //!< Key. [static]

// -----------------------------------------------------------------------------

// NsGraphFieldOpItem
// -----------------
//! CTOR.

NsGraphFieldOpItem::NsGraphFieldOpItem(const NsGraphOpItemArgs &args,
                                       const qreal              xRadius,
                                       const qreal              yRadius)
    : NsGraphOpItem(args)
    , _xRadius(xRadius)
    , _yRadius(yRadius)
    , _shapePath(computeShapePath(args.diameter(), 
                                  op()->inputCount(), op()->outputCount(),
                                  _xRadius, _yRadius))
    , _boundingRect(computeBoundingRect(_shapePath, 0.5*4))
#ifndef NS_GRAPH_VIEW_NAME_ITEMS
    , _textRect(computeTextRect(op()->longName(), 
                                QFont("sansserif", 12, QFont::DemiBold),
                                args.diameter(),
                                8, 8))                                            
    , _textShapePath(computeTextShapePath(_textRect, 2, 2))
#endif
{
#ifndef NS_GRAPH_VIEW_NAME_ITEMS
    _boundingRect |= computeBoundingRect(_textShapePath, 0.5*4);
#else
    NsGraphOpNameItem *goni(addNameItem());
    goni->setPos(nameItemPos(*goni));
#endif

    addInputPlugItems(*op());
    addOutputPlugItems(*op());

    //_repositionInputPlugItems(_textRect);
    //_repositionOutputPlugItems(_textRect);

#ifdef NS_GRAPH_VIEW_UNION_BRECT
    _boundingRect |= childrenBoundingRect(); // Union.
#endif
}


// -----------------------------------------------------------------------------

void
NsGraphFieldOpItem::paint(QPainter                       *painter,
                          const QStyleOptionGraphicsItem *option,
                          QWidget                        *widget)
{
    NsGraphOpItem::paint(painter, option, widget);

#ifndef NS_GRAPH_VIEW_NAME_ITEMS
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
NsGraphFieldOpItem::addInputPlugItems(NsOpObject &op)
{
    const QRectF shapeRect = computeBoundingRect(_shapePath, 0.5*4);


    const qreal sgn(-1.);
    const qreal plugDiam(plugDiameter(diameter()));
    const qreal plugDx(plugSpacingX(shapeRect.width(), op.inputCount()));
    const qreal plugPosY(plugPositionY(shapeRect.height(), plugDiam, sgn));

    int index(0);

    // Body inputs.

    foreach (NsBodyInputPlugObject *bipo, op.mutableBodyInputs()) {
        const qreal plugPosX(plugPositionX(shapeRect.width(),plugDx,index));
        const QPointF plugPos(plugPosX, plugPosY);

        createBodyInputPlugItem(
            bipo,
            *this,  // Child.
            plugDiam,
            plugPos,
            plugLine(plugPos, _boundingRect.height(), sgn),
            plugTextPosition(plugPos.x(), plugDiam, shapeRect.height(), sgn),
            QPointF(plugPos.x(), 0.5*sgn*shapeRect.height()),
            plugText(bipo->name()));

        ++index;
    }

    // Field inputs.

    foreach (NsFieldInputPlugObject *fipo, op.mutableFieldInputs()) {
        const qreal plugPosX(plugPositionX(shapeRect.width(),plugDx,index));
        const QPointF plugPos(plugPosX, plugPosY);

        createFieldInputPlugItem(
            fipo,
            *this,  // Child.
            plugDiam,
            plugPos,
            plugLine(plugPos, _boundingRect.height(), sgn),
            plugTextPosition(plugPos.x(), plugDiam, shapeRect.height(), sgn),
            QPointF(plugPos.x(), 0.5*sgn*shapeRect.height()),
            plugText(fipo->name()));

        ++index;
    }
}


// addOutputPlugItems
// ------------------
//! Create the output plugs associated with the Op.

void
NsGraphFieldOpItem::addOutputPlugItems(NsOpObject &op)
{
    const QRectF shapeRect = computeBoundingRect(_shapePath, 0.5*4);

    const qreal sgn(1.); // Positive.
    const qreal plugDiam(plugDiameter(diameter()));
    const qreal plugDx(plugSpacingX(shapeRect.width(), op.outputCount()));
    const qreal plugPosY(plugPositionY(shapeRect.height(), plugDiam, sgn));

    int index(0);

    // Body outputs.

    foreach (NsBodyOutputPlugObject *bopo, op.mutableBodyOutputs()) {
        const qreal plugPosX(plugPositionX(shapeRect.width(),plugDx,index));
        const QPointF plugPos(plugPosX, plugPosY);

        createBodyOutputPlugItem(
            bopo,
            *this,  // Child.
            plugDiam,
            plugPos,
            plugLine(plugPos, shapeRect.height(), sgn),
            plugTextPosition(plugPos.x(), plugDiam, shapeRect.height(), sgn),
            QPointF(plugPos.x(), 0.5*sgn*shapeRect.height()),
            plugText(bopo->name()));

        ++index;
    }

    // Field outputs.

    foreach (NsFieldOutputPlugObject *fopo, op.mutableFieldOutputs()) {
        const qreal plugPosX(plugPositionX(shapeRect.width(),plugDx,index));
        const QPointF plugPos(plugPosX, plugPosY);

        createFieldOutputPlugItem(
            fopo,
            *this,  // Child.
            plugDiam,
            plugPos,
            plugLine(plugPos, shapeRect.height(), sgn),
            plugTextPosition(plugPos.x(), plugDiam, shapeRect.height(), sgn),
            QPointF(plugPos.x(), 0.5*sgn*shapeRect.height()),
            plugText(fopo->name()));

        ++index;
    }
}

// -----------------------------------------------------------------------------

QPointF
NsGraphFieldOpItem::nameItemPos(const NsGraphOpNameItem &item) const
{ return QPointF(0.6*diameter(), -0.5*item.height()); }

// -----------------------------------------------------------------------------

// computeShapePath
// ----------------
//! Compute the path used in the graph view. [static]

QPainterPath
NsGraphFieldOpItem::computeShapePath(const qreal diameter,
                                     const int   inputCount,
                                     const int   outputCount,
                                     const qreal xRadius,
                                     const qreal yRadius)
{
    const qreal w(
        qMax(diameter, 
             qMax((inputCount + 1)*plugDiameter(diameter),
                  (outputCount + 1)*plugDiameter(diameter))));
    const qreal h(0.75*diameter);

    QPainterPath p;
    p.addRoundedRect(QRect(-0.5*w, -0.5*h, w, h), xRadius, yRadius);
    return p;
}

#ifndef NS_GRAPH_VIEW_NAME_ITEMS
QRectF
NsGraphFieldOpItem::computeTextRect(const QString &text,
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
NsGraphFieldOpItem::computeTextShapePath(const QRectF &textRect,
                                         const qreal   xRadius,
                                         const qreal   yRadius)
{
    QPainterPath p;
    p.addRoundedRect(textRect, xRadius, yRadius);
    return p;
}
#endif

// -----------------------------------------------------------------------------

// _setName
// --------
//! DOCS

void
NsGraphFieldOpItem::_setName(const QString &newName)
{
#ifndef NS_GRAPH_VIEW_NAME_ITEMS
    prepareGeometryChange();
    const QFont textFont("sansserif", 12, QFont::DemiBold);
    _textRect = computeTextRect(newName, textFont, diameter(), 8, 8);
    _textShapePath = computeTextShapePath(_textRect, 2, 2);
    const QRectF shapeRect = computeBoundingRect(_shapePath, 0.5*4);
    const QRectF textRect = computeBoundingRect(_textShapePath, 0.5*4);
    _boundingRect = shapeRect | textRect; // Union.
#endif
}

// -----------------------------------------------------------------------------

//// _repositionInputPlugItems
//// -------------------------
////! DOCS
//
//void
//NsGraphFieldOpItem::_repositionInputPlugItems(const QRectF &textRect)
//{
//    const QList<NsGraphInputPlugItem *> plugList(inputPlugItems());
//
//    if (!plugList.empty()) {
//        const int count(plugList.size());
//        const qreal dx(plugSpacing(textRect.width(), count));
//        const qreal ppy(plugPosY(textRect.height(),
//                                 plugDiameter(diameter()),
//                                 -1.));  // Negative.
//
//        int index(0);
//        foreach (NsGraphInputPlugItem *plug, plugList) {
//            const qreal ppx(plugPosX(textRect.width(), dx, index));
//            plug->setPos(QPointF(ppx, ppy));
//            ++index;
//        }
//    }
//}
//
//
//// _repositionOutputPlugItems
//// --------------------------
////! DOCS
//
//void
//NsGraphFieldOpItem::_repositionOutputPlugItems(const QRectF &textRect)
//{
//    const QList<NsGraphOutputPlugItem *> plugList(outputPlugItems());
//
//    if (!plugList.empty()) {
//        const int count(plugList.size());
//        const qreal dx(plugSpacing(textRect.width(), count));
//        const qreal ppy(plugPosY(textRect.height(),
//                                 plugDiameter(diameter()),
//                                 1.));  // Positive.
//
//        int index(0);
//        foreach (NsGraphOutputPlugItem *plug, plugList) {
//            const qreal ppx(plugPosX(textRect.width(), dx, index));
//            plug->setPos(QPointF(ppx, ppy));
//            ++index;
//        }
//    }
//}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsGraphFieldOpItem::_CreatorType
    NsGraphFieldOpItem::_creator(*NsGraphOpItemFactory::instance());

// -----------------------------------------------------------------------------
