// -----------------------------------------------------------------------------
//
// NsGraphItem.h
//
// Abstract base class for items shown in graph view, header file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This file is part of Open Naiad Studio..
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

#ifndef NS_GRAPH_ITEM_H
#define NS_GRAPH_ITEM_H

// Define to enable debugging of graph view items.
#ifdef DEBUG
#define NS_GRAPH_VIEW_DEBUG
#endif

//#define NS_GRAPH_VIEW_UNION_BRECT
//#define NS_GRAPH_VIEW_NAME_ITEMS

#include <QGraphicsObject>
#include <QtGui>

class NsGraphScene;
class NsUndoStack;
class QUndoCommand;

// -----------------------------------------------------------------------------

// NsGraphItem
// -----------
//! Abstract base class for items shown in the Graph View.

class NsGraphItem : public QGraphicsObject
{
    Q_OBJECT

public:

    virtual
    ~NsGraphItem() = 0;  // Force abstract.

    virtual QString
    longName() const = 0;

    //! By default this function does nothing.
    virtual void
    setSelected(const bool selected, NsUndoStack *undoStack, const bool merge)
    {}

    virtual void
    setSelected(const bool selected, QUndoCommand &parent)
    {}
                
    virtual bool
    isSelected() const
    { return false; }


    bool
    isItemSelectable() const
    { return _itemSelectable; }

    bool
    isItemMovable() const
    { return _itemMovable; }

    bool
    isItemSelected() const
    { return _itemSelected; }

    void
    setItemSelected(const bool selected)
    { 
        if (isItemSelectable() && (_itemSelected != selected)) {
            _itemSelected = selected; 
            update();
        }
    }

    const QPointF
    itemPosition() const
    { return QGraphicsObject::pos(); }

    void
    setItemPosition(const QPointF &p)
    {
        if (isItemMovable() && (QGraphicsObject::pos() != p)) {
            //_itemPosition = p;
            QGraphicsObject::setPos(p);
        }
    }

protected:

    //! CTOR.
    explicit
    NsGraphItem(QGraphicsItem *parent         = 0,
                const bool     itemSelectable = false,
                const bool     itemMovable    = false)
        : QGraphicsObject(parent)
        , _itemSelectable(itemSelectable)
        , _itemMovable(itemMovable)
        , _itemSelected(false)
    { _init(); }

    NsGraphScene*
    graphScene() const;

protected:  // Paint events.

    virtual QRectF
    boundingRect() const = 0;

    virtual QPainterPath
    shape() const = 0;

    virtual void
    paint(QPainter                       *painter,
          const QStyleOptionGraphicsItem *option,
          QWidget                        *widget) = 0;

private:

    void
    _init()
    {
        //setFlag(ItemIsSelectable, false);
        //setFlags(ItemIsSelectable | ItemIsMovable);
        setCacheMode(DeviceCoordinateCache);
    }
    
private:    // Member variables.

    bool    _itemSelectable;
    bool    _itemMovable;
    bool    _itemSelected;
};

// -----------------------------------------------------------------------------

#endif // NS_GRAPH_ITEM_H
