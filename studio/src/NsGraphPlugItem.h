// -----------------------------------------------------------------------------
//
// NsGraphPlugItem.h
//
// Abstract base class for Op plug items, header file.
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

#ifndef NS_GRAPH_PLUG_ITEM_H
#define NS_GRAPH_PLUG_ITEM_H

#include "NsGraphItem.h"
#include "NsPlugObject.h"

class NsGraphOpItem;

// -----------------------------------------------------------------------------

// NsGraphPlugItem
// ---------------
//! Abstract base class for plug items. Note that the parent must be an Op item
//! and cannot be null.

class NsGraphPlugItem : public NsGraphItem
{
    Q_OBJECT

public:     // NsGraphItem interface.

    virtual
    ~NsGraphPlugItem() = 0;     // Force abstract.

    virtual QString
    longName() const
    { return plugObject()->longName(); }

public:

    virtual QPointF
    plugPos() const = 0;

    virtual NsPlugObject*
    plugObject() const
    { return _plug; }

    NsOpObject*
    opObject() const;

    NsGraphOpItem*
    opItem()
    { return _opItem; }

    //! Returns the plug item's diameter.
    qreal
    diameter() const
    { return _diameter; }

protected slots:

    void
    onGroupChanged()
    { groupChanged(); }

    void
    onSmackChanged()
    { smackChanged(); }

protected:

    explicit
    NsGraphPlugItem(NsPlugObject  *po,
                    NsGraphOpItem &parent,
                    qreal          diameter);

protected:      // Hooks.

    virtual void
    groupChanged()
    { update(); }

    virtual void
    smackChanged()
    { update(); }

protected:      // Paint events.

    virtual QRectF
    boundingRect() const = 0;

    virtual QPainterPath
    shape() const = 0;

    virtual void
    paint(QPainter                       *painter,
          const QStyleOptionGraphicsItem *option,
          QWidget                        *widget) = 0;


#ifdef NS_GRAPH_VIEW_DEBUG
public:

    virtual QString
    plugType() const = 0;

protected:  // Mouse events.

    static QString
    toolTip(const QString &plugType,
            const QString &name,
            const QString &sig,
            bool           group);

    virtual void
    hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void
    hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
#endif  // NS_GRAPH_VIEW_DEBUG

protected:

    static void
    paintText(QPainter      *painter,
              const QPen    &textPen,
              const QFont   &font,
              const QRectF  &textRect,
              const QString &text);

    static void
    paintUnsmacked(QPainter           *painter,
                   const QPainterPath &shapePath,
                   const QLineF       &line,
                   const QPen         &shapePen,
                   const QBrush       &shapeBrush,
                   const QPainterPath &sigPath,
                   const QPen         &sigPen,
                   const QBrush       &sigBrush);

    static void
    paintSmacked(QPainter          *painter,
                 const QPainterPath &shapePath,
                 const QPen         &shapePen,
                 const QBrush       &shapeBrush);

protected:

    static QPainterPath
    computeBodyPlugPath(const QPointF &pos,
                        const qreal    diameter);

    static QPainterPath
    computeFieldPlugPath(const QPointF &pos,
                         const qreal    diameter, 
                         const qreal    xRadius, 
                         const qreal    yRadius);

    static QPainterPath
    computeSigPath(const NsPlugObject &po, 
                   const QPointF      &pos,
                   qreal               diameter);

    static QRectF
    computeTextRect(const QString &text, 
                    const QFont   &font, 
                    const QPointF &pos);

    static QRectF
    computeBoundingRect(const QPainterPath &shape, 
                        const QRectF       &textRect, 
                        qreal               pad);

private:

    static const bool _isMovable    = false;
    static const bool _isSelectable = false;

private:    // Member variables.

    NsPlugObject  *_plug; //!< Resource.
    NsGraphOpItem *_opItem;
    qreal          _diameter;
};

// -----------------------------------------------------------------------------

#endif // NS_GRAPH_PLUG_ITEM_H
