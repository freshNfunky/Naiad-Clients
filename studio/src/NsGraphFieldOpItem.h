// -----------------------------------------------------------------------------
//
// NsGraphFieldOpItem.h
//
// Class showing a field Op in graph view, header file.
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

#ifndef NS_GRAPH_FIELD_OP_ITEM_H
#define NS_GRAPH_FIELD_OP_ITEM_H

#include "NsGraphOpItem.h"
#include "NsGraphOpItemCreator.h"
#include "NsGraphOpItemKey.h"

// -----------------------------------------------------------------------------

// NsGraphFieldOpItem
// ------------------
//! TODO

class NsGraphFieldOpItem : public NsGraphOpItem
{
    Q_OBJECT

public:

    static const NsGraphOpItemKey key;  // "FIELD_OP"

public:

    explicit
    NsGraphFieldOpItem(const NsGraphOpItemArgs &args,
                       qreal                    xRadius = 4.,
                       qreal                    yRadius = 4.);

    //! DTOR.
    virtual
    ~NsGraphFieldOpItem()
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

protected:  // Paint events.

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

    static qreal
    plugSpacingX(const qreal width, const int n)
    { return (width/(n + 1)); }

    static qreal
    plugPositionX(const qreal width, const qreal plugSpacing, const int index)
    { return (-0.5*width + (index + 1)*plugSpacing); }

    static qreal
    plugPositionY(const qreal height, const qreal plugDiameter, const qreal sgn)
    { return (sgn*(0.5*height + plugDiameter)); }

    static QPointF
    plugTextPosition(const qreal x,const qreal pd,const qreal h,const qreal sgn)
    { return QPointF(x, 0.4*sgn*(0.5*h + pd)); }

    static QLineF
    plugLine(const QPointF &plugPos, const qreal h, const qreal sgn)
    { return QLineF(QPointF(plugPos.x(), 0.5*sgn*h), plugPos); }

    static QString
    plugText(const QString &plugName)
    { return QString(plugName.at(0)); }

protected:

    QPointF
    nameItemPos(const NsGraphOpNameItem &item) const;

protected:

    static QPainterPath
    computeShapePath(qreal diameter,
                     int   inputCount,
                     int   outputCount,
                     qreal xRadius,
                     qreal yRadius);

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

//private:
//
//    void 
//    _repositionInputPlugItems(const QRectF &textRect);
//
//    void 
//    _repositionOutputPlugItems(const QRectF &textRect);

private:

    void
    _setName(const QString &newName);

private:    // Member variables.

    qreal _xRadius;
    qreal _yRadius;

    QPainterPath _shapePath;
    QRectF       _boundingRect;

#ifndef NS_GRAPH_VIEW_NAME_ITEMS
    QRectF       _textRect;
    QPainterPath _textShapePath;
#endif

private:        // Factory.

    typedef NsGraphOpItemCreator<NsGraphFieldOpItem> _CreatorType;
    static const _CreatorType _creator;
};

#endif // NS_GRAPH_FIELD_OP_ITEM_H
