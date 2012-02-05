// -----------------------------------------------------------------------------
//
// NsGraphBodyScopeOpItem.cc
//
// Class for showing a body scope in the graph view, source file.
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

#include "NsGraphBodyScopeOpItem.h"
#include "NsGraphOpStateItem.h"
#include "NsGraphOpNameItem.h"
#include "NsGraphOpItemFactory.h"
#include "NsGraphBodyInputPlugItem.h"
#include "NsPreferences.h"
#include "NsSignatureStore.h"

// -----------------------------------------------------------------------------

const NsGraphOpItemKey NsGraphBodyScopeOpItem::key("BODY_SCOPE");

// -----------------------------------------------------------------------------

// NsGraphBodyScopeOpItem
// ----------------------
//! CTOR.

NsGraphBodyScopeOpItem::NsGraphBodyScopeOpItem(
        const NsGraphOpItemArgs &args,
        const QBrush            &sigBrush)
    : NsGraphOpItem(args)
    , _sigBrush(sigBrush)
    , _shapePath(computeShapePath(_diameter()))
    , _boundingRect(computeBoundingRect(_shapePath, 0.5*4))
    , _nameItem(addNameItem())
{
    NsGraphOpStateItem *gosi(addStateItem(stateItemDiameter()));
    gosi->setPos(stateItemPos(gosi->diameter()));

    //NsGraphOpNameItem *goni(addNameItem());
    //goni->setPos(nameItemPos(*goni));
    _nameItem->setPos(nameItemPos(*_nameItem));

    addInputPlugItems(*op());

#ifdef NS_GRAPH_VIEW_UNION_BRECT
    _boundingRect |= childrenBoundingRect(); // Union.
#endif
}

// -----------------------------------------------------------------------------

void
NsGraphBodyScopeOpItem::addInputPlugItems(NsOpObject &op)
{
    if (0 < op.bodyInputCount()) {

        NsBodyInputPlugObject *bipo(
            op.queryMutableBodyInput(op.bodyInputNames().at(0)));

        if (0 != bipo) {
            
            const qreal thetaRad(M_PI);

            const qreal plugDiam(plugDiameter(diameter()));
            const qreal plugDist(plugDistance(_diameter(), plugDiam));
            const QPointF plugPos(plugPosition(plugDist, thetaRad));    

            createBodyInputPlugItem(
                bipo, 
                *this,  // Child.
                plugDiam,
                plugPos,
                plugLine(thetaRad, _diameter(), plugPos),
                plugTextPosition(plugDist, thetaRad),
                _circularPos(0.5*_diameter(), thetaRad),
                plugText(bipo->name()));

            _sigName = bipo->sigName();
            //const QString sigName(bipo->sigName());
            _sigPath = NsSignatureStore::instance()->shape(_sigName,_diameter());
            //setShapeBrush(QBrush(NsSignatureStore::instance()->color(sigName)));
        }
    }
}

// -----------------------------------------------------------------------------

// paint
// -----
//! Draw the item in the graph view.

void
NsGraphBodyScopeOpItem::paint(QPainter                       *painter,
                              const QStyleOptionGraphicsItem *option,
                              QWidget                        *widget)
{
    //NsGraphOpItem::paint(painter, option, widget); // Parent method.

    const QPainterPath sh = shape();
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
            NsPreferences::instance()->graphViewSignatureColor(_sigName);
        const QColor disabledColor =
            NsPreferences::instance()->graphViewOpDisabledColor();
        fb = !opEnabled ? QBrush(disabledColor) : QBrush(categoryColor);
        break;
    }

    //painter->setClipRect(boundingRect());
    painter->fillPath(sh, fb);

    if (op()->isSelected() || isItemSelected()) {
        painter->strokePath(
            sh, 
            QPen(NsPreferences::instance()->graphViewOpLineColor(), 4));
        painter->strokePath(
            sh, 
            QPen(NsPreferences::instance()->graphViewSelectionColor(), 2));
    }
    else {
        painter->strokePath(
            sh,
            QPen(NsPreferences::instance()->graphViewOpLineColor(), 2));
    }

    painter->fillPath(_sigPath, _sigBrush);

    if (!opEnabled) {
        const QRectF shapeRect = sh.boundingRect();
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

void
NsGraphBodyScopeOpItem::nameChanged(const QString &oldOpInstance,
                                    const QString &newOpInstance)
{
    NsGraphOpItem::nameChanged(oldOpInstance, newOpInstance);
    _nameItem->setPos(nameItemPos(*_nameItem));
}

// -----------------------------------------------------------------------------

QPointF
NsGraphBodyScopeOpItem::nameItemPos(const NsGraphOpNameItem &item) const
{ return QPointF(-0.5*item.width(), 0.6*_diameter()); }

// -----------------------------------------------------------------------------

// computeShapePath
// ----------------
//! Compute the path used in the graph view. [static]

QPainterPath
NsGraphBodyScopeOpItem::computeShapePath(const qreal diameter)
{
    QPainterPath p;
    p.addEllipse(QPointF(0., 0.), 0.5*diameter, 0.5*diameter);
    return p;
}

// -----------------------------------------------------------------------------

//! Static creator instance.
const NsGraphBodyScopeOpItem::_CreatorType
    NsGraphBodyScopeOpItem::_creator(*NsGraphOpItemFactory::instance());

// -----------------------------------------------------------------------------
