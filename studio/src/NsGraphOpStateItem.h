// -----------------------------------------------------------------------------
//
// NsGraphOpStateItem.h
//
// Class for showing Op state in the graph view, header file.
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

#ifndef NS_GRAPH_OP_STATE_ITEM_H
#define NS_GRAPH_OP_STATE_ITEM_H

#include "NsGraphItem.h"

class NsGraphOpItem;
class NsOpObject;

// -----------------------------------------------------------------------------

// NsGraphOpStateItem
// ------------------
//! DOCS

class NsGraphOpStateItem : public NsGraphItem
{
    Q_OBJECT

public:     // NsGraphItem interface.

    //! DTOR.
    virtual
    ~NsGraphOpStateItem()
    { qDebug() << "~NsGraphOpStateItem"; }

    virtual QString
    longName() const;

    virtual bool
    isSelectable() const
    { return false; }

public:

    explicit
    NsGraphOpStateItem(NsGraphOpItem &parent,
                       qreal          diameter);

    qreal
    diameter() const
    { return _diameter; }

    NsOpObject*
    op() const;

protected slots:

    void
    onStateChanged()
    { stateChanged(); }

protected:  // Hooks.

    virtual void
    stateChanged()
    { _stateChanged(); }

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


#ifdef NS_GRAPH_VIEW_DEBUG
protected:      // Mouse events.

    virtual void
    hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void
    hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
#endif  // NS_GRAPH_VIEW_DEBUG


protected:

    static QPainterPath
    computeShapePath(qreal diameter);

    static QRectF
    computeBoundingRect(const QPainterPath &shape, const qreal pad)
    { return shape.boundingRect().adjusted(-pad, -pad, pad, pad); }

    static QPainterPath
    computeStatePath(qreal diameter);

private:

    void
    _stateChanged()
    { update(); }

private:    // Member variables.

    NsGraphOpItem *_parent;
    qreal          _diameter;

    QPainterPath _shapePath;
    QRectF       _boundingRect;
    QPainterPath _statePath;
};

#endif // NS_GRAPH_OP_STATE_ITEM_H
