// -----------------------------------------------------------------------------
//
// NsGraphOpNameItem.h
//
// Class for showing an Op name in the graph view, header file.
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

#ifndef NS_GRAPH_OP_NAME_ITEM_H
#define NS_GRAPH_OP_NAME_ITEM_H

#include "NsGraphItem.h"

class NsGraphOpItem;
class NsOpObject;

// -----------------------------------------------------------------------------

// NsGraphOpNameItem
// -----------------
//! DOCS

class NsGraphOpNameItem : public NsGraphItem
{
    Q_OBJECT

public:     // NsGraphItem interface.

    //! DTOR.
    virtual
    ~NsGraphOpNameItem()
    { qDebug() << "~NsGraphOpNameItem"; }

    virtual QString
    longName() const;

public:

    explicit
    NsGraphOpNameItem(NsGraphOpItem &parent,
                      qreal          xRadius  = 2.,
                      qreal          yRadius  = 2.,
                      qreal          wPadding = 8.,
                      qreal          hPadding = 8.);

    NsOpObject*
    op() const;

    qreal
    width() const
    { return _boundingRect.width(); }

    qreal
    height() const
    { return _boundingRect.height(); }

protected slots:

    void
    onNameChanged(const QString &oldOpInstance,
                  const QString &newOpInstance);

protected:

    virtual void
    nameChanged(const QString &newName);

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
//protected:      // Mouse events.
//
//    virtual void
//    hoverEnterEvent(QGraphicsSceneHoverEvent *event);
//
//    virtual void
//    hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
#endif  // NS_GRAPH_VIEW_DEBUG


protected:

    static QRectF
    computeTextRect(const QString &text,
                    const QFont   &font,
                    qreal          widthPadding,
                    qreal          heightPadding);

    static QPainterPath
    computeShapePath(const QRectF &textRect,
                     qreal         xRadius,
                     qreal         yRadius);

    static QRectF
    computeBoundingRect(const QPainterPath &shape, const qreal pad)
    { return shape.boundingRect().adjusted(-pad, -pad, pad, pad); }

private:

    void
    _setName(const QString &newName);

private:    // Member variables.

    NsGraphOpItem *_parent;

    QFont        _font;

    qreal        _xRadius;
    qreal        _yRadius;
    qreal        _wPadding;
    qreal        _hPadding;

    QRectF       _textRect;
    QPainterPath _shapePath;
    QRectF       _boundingRect;
};

#endif  // NS_GRAPH_OP_NAME_ITEM_H
