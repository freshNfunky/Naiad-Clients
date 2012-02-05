// -----------------------------------------------------------------------------
//
// NsGraphBodyItem.h
//
// Class for showing bodies in graph view, header file.
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

#ifndef NS_GRAPH_BODY_ITEM_H
#define NS_GRAPH_BODY_ITEM_H

#include "NsGraphItem.h"
#include <QDebug>

class NsBodyObject;
class NsGraphFeedItem;

// -----------------------------------------------------------------------------

// NsGraphBodyItem
// ---------------
//! Class for showing bodies in the graph view.

class NsGraphBodyItem : public NsGraphItem
{
    Q_OBJECT

public:     // NsGraphItem interface.

    //! DTOR.
    virtual
    ~NsGraphBodyItem()
    { qDebug() << "~NsGraphBodyItem"; }

    virtual QString
    longName() const;

    virtual void
    setSelected(bool selected, NsUndoStack *undoStack, bool merge);

    virtual void
    setSelected(bool selected, QUndoCommand &parent);

    virtual bool
    isSelected() const;

public:

    explicit
    NsGraphBodyItem(NsBodyObject    &body,
                    qreal            diameter,
                    NsGraphFeedItem &parent);

    NsBodyObject*
    body() const
    { return _body; }

    qreal
    diameter() const
    { return _diameter; }

protected slots:

    void
    onSelectionChanged()
    { selectionChanged(); }

protected:  // Hooks.

    virtual void
    selectionChanged()
    { update(); }

protected:      // Paint events.

    virtual QRectF
    boundingRect() const
    { return _boundingRect; }

    virtual QPainterPath
    shape() const
    { return _shapePath; }

    virtual void
    paint(QPainter *painter,
          const QStyleOptionGraphicsItem *option,
          QWidget *widget);

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
    computeBoundingRect(const QPainterPath &shape,
                        const qreal         pad = 16.)
    { return shape.boundingRect().adjusted(-pad, -pad, pad, pad); }

private:

    static const bool _isMovable    = false;
    static const bool _isSelectable = true;

private:    // Member variables.

    NsBodyObject *_body;   //!< Resource.

    qreal        _diameter;
    QPainterPath _shapePath;
    QRectF       _boundingRect;

    QPainterPath _sigPath;
};

#endif // NS_GRAPH_BODY_ITEM_H
