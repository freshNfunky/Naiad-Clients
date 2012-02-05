// -----------------------------------------------------------------------------
//
// NsGraphFieldScopeOpItem.h
//
// Class for showing a field scope in the graph view, header file.
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

#ifndef NS_GRAPH_FIELD_SCOPE_OP_ITEM_H
#define NS_GRAPH_FIELD_SCOPE_OP_ITEM_H

#include "NsGraphOpItem.h"
#include "NsGraphOpItemCreator.h"
#include "NsGraphOpItemKey.h"

// -----------------------------------------------------------------------------

// NsGraphFieldScopeOpItem
// -----------------------
//! TODO

class NsGraphFieldScopeOpItem : public NsGraphOpItem
{
    Q_OBJECT

public:

    static const NsGraphOpItemKey key;  // "FIELD_SCOPE"

public:

    explicit
    NsGraphFieldScopeOpItem(const NsGraphOpItemArgs &args);

    //! DTOR.
    virtual
    ~NsGraphFieldScopeOpItem() 
    {}   

protected:

    void
    nameChanged(const QString &oldOpInstance,
                const QString &newOpInstance);

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

protected:

    virtual QBrush
    shapeBrush() const
    { return QBrush(_sigColor); }

    virtual QFont
    font() const
    { return QFont("sansserif", 8); }

protected:

    static QRectF
    computeTextRect(const QFont   &font,
                    const QString &text,
                    qreal          diameter,
                    qreal          widthPadding  = 2.,
                    qreal          heightPadding = 2.);

    static QPainterPath
    computeShapePath(qreal diameter,
                     qreal xRadius = 4.,
                     qreal yRadius = 4.);

    static QRectF
    computeBoundingRect(const QPainterPath &shape,
                        qreal               padding = 3.);

private slots:

    void
    _setName(const QString &newName);

    static qreal
    _diameter(qreal opDiameter)
    { return 0.75*opDiameter; }

private:    // Member variables.

    QColor       _sigColor;
    QPainterPath _sigPath;
    QPainterPath _rectPath;

    QRectF       _textRect;
    QPainterPath _shapePath;
    QRectF       _boundingRect;

private:    // Register with factory.

    typedef NsGraphOpItemCreator<NsGraphFieldScopeOpItem> _CreatorType;
    static const _CreatorType _creator;
};

#endif // NS_GRAPH_FIELD_SCOPE_OP_ITEM_H
