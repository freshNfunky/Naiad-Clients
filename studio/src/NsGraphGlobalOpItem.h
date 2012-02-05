// -----------------------------------------------------------------------------
//
// NsGraphGlobalOpItem.h
//
// Class showing the global Op in graph view, header file.
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

#ifndef NS_GRAPH_GLOBAL_OP_ITEM_H
#define NS_GRAPH_GLOBAL_OP_ITEM_H

#include "NsGraphOpItem.h"
#include "NsGraphOpItemCreator.h"
#include "NsGraphOpItemKey.h"

// -----------------------------------------------------------------------------

// NsGraphGlobalOpItem
// -------------------
//! Item representing the global Op in the graph view.

class NsGraphGlobalOpItem : public NsGraphOpItem
{
    Q_OBJECT

public:

    static const NsGraphOpItemKey key;  // "GLOBAL_OP"

public:

    explicit 
    NsGraphGlobalOpItem(
        const NsGraphOpItemArgs &args,
        const QString           &text = "G",
        const QFont             &font = QFont("sansserif", 43, QFont::DemiBold),
        qreal                    wPadding = 0.,
        qreal                    hPadding = 0.);

    //! DTOR.
    virtual 
    ~NsGraphGlobalOpItem()
    {}

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

protected:

    static QRectF
    computeTextRect(const QString &text,
                    const QFont   &font,
                    qreal          wPadding,
                    qreal          hPadding);

    static QPainterPath
    computeShapePath(qreal diameter);

    static QRectF
    computeBoundingRect(const QPainterPath &shape, const qreal pad)
    { return shape.boundingRect().adjusted(-pad, -pad, pad, pad); }

private:    // Member variables.

    QString _text;
    QFont   _font;
    qreal   _wPadding;
    qreal   _hPadding;

    QRectF       _textRect;       //!< The rectangle to paint text in.
    QPainterPath _shapePath;
    QRectF       _boundingRect;

private:    // Factory.

    typedef NsGraphOpItemCreator<NsGraphGlobalOpItem> _CreatorType;
    static const _CreatorType _creator;
};

#endif // NS_GRAPH_GLOBAL_OP_ITEM_H
