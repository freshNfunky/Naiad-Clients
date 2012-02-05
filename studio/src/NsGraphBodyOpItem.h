// -----------------------------------------------------------------------------
//
// NsGraphBodyOpItem.h
//
// Class showing a body Op in graph view, header file.
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

#ifndef NS_GRAPH_BODY_OP_ITEM_H
#define NS_GRAPH_BODY_OP_ITEM_H

#include "NsGraphOpItem.h"
#include "NsGraphOpItemCreator.h"
#include "NsGraphOpItemKey.h"

// -----------------------------------------------------------------------------

// NsGraphBodyOpItem
// -----------------
//! An item in the Graph View representing a body Op.

class NsGraphBodyOpItem : public NsGraphOpItem
{
    Q_OBJECT

public:

    static const NsGraphOpItemKey key;  // "BODY_OP"

public:

    explicit
    NsGraphBodyOpItem(const NsGraphOpItemArgs &args);

    //! DTOR.
    virtual
    ~NsGraphBodyOpItem()
    {}

#ifndef NS_GRAPH_VIEW_NAME_ITEMS
protected:

    virtual void
    nameChanged(const QString &oldOpInstance,
                const QString &newOpInstance)
    { 
        NsGraphOpItem::nameChanged(oldOpInstance, newOpInstance);
        _setName(newOpInstance); 
    }
#endif

protected:      // Paint events.

    virtual QRectF
    boundingRect() const
    { return _boundingRect; }

    virtual QPainterPath
    shape() const
    { return _shapePath; }

    virtual void
    paint(QPainter                       *painter,
          const QStyleOptionGraphicsItem *option,
          QWidget                        *widget);

protected:      // Plug creation.

    void
    addInputPlugItems(NsOpObject &op);

    void
    addOutputPlugItems(NsOpObject &op);

protected:

    static const qreal inputOffsetRad;
    static const qreal outputOffsetRad;

    //! DOCS
    static qreal
    plugOffsetAngle(const qreal theta0, const qreal dTheta, const int n)
    { return theta0 - ((n & 1) ? dTheta*((n - 1)/2) : dTheta*(0.5*(n - 1))); }

    //! Returns angle between plugs in radians.
    static qreal
    plugDeltaAngle(const qreal opDiameter, const qreal plugDiameter)
    { return (2.*asin(plugDiameter/opDiameter)); }

    //! Returns plug angle in the unit that dTheta and offset are given in.
    static qreal
    plugAngle(const qreal offset, const qreal dTheta, const int index)
    { return (offset + dTheta*index); }

     //! Returns distance from plug center to Op center.
    static qreal
    plugDistance(const qreal opDiameter, const qreal plugDiameter)
    { return (0.5*opDiameter + plugDiameter); }

    //! Compute plug position in circular coords.
    static QPointF
    plugPosition(const qreal dist, const qreal thetaRad)
    { return _circularPos(dist, thetaRad); }

    static QPointF
    plugTextPosition(const qreal plugDist, const qreal thetaRad)
    { return _circularPos(0.5*plugDist, thetaRad); }

    static QLineF
    plugLine(const qreal    thetaRad, 
             const qreal    opDiameter, 
             const QPointF &plugPos)
    { return QLineF(_circularPos(0.5*opDiameter, thetaRad), plugPos); }

    static QString
    plugText(const QString &plugName)
    { return QString(plugName.at(0)); }

protected:

    qreal
    stateItemDiameter() const
    { return 0.25*diameter(); }

    QPointF
    stateItemPos() const
    { return QPointF(-0.75*diameter(), 0.); }

protected:

    QPointF
    nameItemPos(const NsGraphOpNameItem &item) const;

protected:

    static QPainterPath
    computeShapePath(qreal diameter);

    static QRectF
    computeBoundingRect(const QPainterPath &shape,
                        const qreal         pad)
    { return shape.boundingRect().adjusted(-pad, -pad, pad, pad); }

    static QRectF
    computeTextRect(const QString &text,
                    const QFont   &font,
                    qreal          diameter,
                    qreal          widthPadding,
                    qreal          heightPadding);

    static QPainterPath
    computeTextShapePath(const QRectF &textRect,
                         qreal         xRadius,
                         qreal         yRadius);

private:

    void
    _setName(const QString &newName);

private:

    static QPointF
    _circularPos(const qreal radius, const qreal thetaRad)
    { return QPointF(radius*cos(thetaRad), radius*sin(thetaRad)); }

private:        // Member variables.

    QPainterPath _shapePath;

#ifndef NS_GRAPH_VIEW_NAME_ITEMS
    QRectF       _textRect;
    QPainterPath _textShapePath;
#endif

    QRectF       _boundingRect;

private:        // Factory.

    typedef NsGraphOpItemCreator<NsGraphBodyOpItem> _CreatorType;
    static const _CreatorType _creator;
};

#endif // NS_GRAPH_BODY_OP_ITEM_H
