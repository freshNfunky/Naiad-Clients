// -----------------------------------------------------------------------------
//
// NsGraphOpItem.h
//
// Abstract base class for Op items in the graph view, header file.
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

#ifndef NS_GRAPH_OP_ITEM_H
#define NS_GRAPH_OP_ITEM_H

#include "NsGraphItem.h"
#include "NsOpObject.h"
#include <NiTypes.h>

class NsUndoStack;
class NsGraphOpItemArgs;
class NsGraphOpStateItem;
class NsGraphOpNameItem;
class NsGraphPlugItem;
class NsGraphInputPlugItem;
class NsGraphOutputPlugItem;
class NsGraphBodyInputPlugItem;
class NsGraphFieldInputPlugItem;
class NsGraphBodyOutputPlugItem;
class NsGraphFieldOutputPlugItem;

// -----------------------------------------------------------------------------

// NsGraphOpItem
// -------------
//! Abstract base class for Graph items representing Ops.

class NsGraphOpItem : public NsGraphItem
{
    Q_OBJECT

public:

    virtual
    ~NsGraphOpItem() = 0;   // Abstract.

    virtual QString
    longName() const;

    virtual void
    setSelected(bool selected, NsUndoStack *undoStack, bool merge);

    virtual void
    setSelected(const bool selected, QUndoCommand &parent);

    virtual bool
    isSelected() const;

public:

    NsOpObject*
    op() const
    { return _op; }

    qreal
    diameter() const
    { return _diameter; }

    NsUndoStack*
    undoStack() const
    { return _undoStack; }


    // Plug queries.

    QList<NsGraphPlugItem*>
    plugItems() const;

    QList<NsGraphInputPlugItem*>
    inputPlugItems() const;

    QList<NsGraphOutputPlugItem*>
    outputPlugItems() const;

    NsGraphPlugItem*
    plugItem(const QString &plugLongName) const;

    NsGraphInputPlugItem*
    inputPlugItem(const QString &plugLongName) const;

    NsGraphOutputPlugItem*
    outputPlugItem(const QString &plugLongName) const;

    bool
    hasInputPlugItems() const;

    bool
    hasOutputPlugItems() const;

protected slots:

    void
    onNameChanged(const QString &oldOpInstance,
                  const QString &newOpInstance)
    { nameChanged(oldOpInstance, newOpInstance); }

    void
    onConditionChanged(const NsOpObject::Condition cond)
    { conditionChanged(cond); }

    void
    onEnabledChanged(const bool enabled)
    { enabledChanged(enabled); }

    void
    onSelectionChanged()
    { selectionChanged(); }

    void
    onPositionChanged(const qreal x, const qreal y)
    { positionChanged(QPointF(x, y)); }

protected:  // Hooks.

    virtual void
    nameChanged(const QString &oldOpInstance,
                const QString &newOpInstance)
    {    
        Q_UNUSED(oldOpInstance);
         _longName = newOpInstance; 
    }

    virtual void
    conditionChanged(const NsOpObject::Condition cond)
    { 
        Q_UNUSED(cond);
        update();
    }

    virtual void
    enabledChanged(const bool enabled)
    {
        Q_UNUSED(enabled);
        update();
    }

    virtual void
    selectionChanged()
    { update(); }

    virtual void
    positionChanged(const QPointF &p)
    { _setPosition(p); }

protected:

    explicit
    NsGraphOpItem(const NsGraphOpItemArgs &args);

    NsGraphOpStateItem*
    addStateItem(qreal diameter);
    
    NsGraphOpNameItem*
    addNameItem();

    NsGraphBodyInputPlugItem*
    createBodyInputPlugItem(NsBodyInputPlugObject *bipo,
                            NsGraphOpItem          &parent,
                            qreal                   diameter,
                            const QPointF          &plugPos,
                            const QLineF           &line,
                            const QPointF          &expTextPos,
                            const QPointF          &minTextPos,
                            const QString          &text) const;

    NsGraphFieldInputPlugItem*
    createFieldInputPlugItem(NsFieldInputPlugObject *fipo,
                             NsGraphOpItem          &parent,
                             qreal                   diameter,
                             const QPointF          &plugPos,
                             const QLineF           &line,
                             const QPointF          &expTextPos,
                             const QPointF          &minTextPos,
                             const QString          &text) const;

    NsGraphBodyOutputPlugItem*
    createBodyOutputPlugItem(NsBodyOutputPlugObject *bopo,
                             NsGraphOpItem          &parent,
                             qreal                   diameter,
                             const QPointF          &plugPos,
                             const QLineF           &line,
                             const QPointF          &expTextPos,
                             const QPointF          &minTextPos,
                             const QString          &text) const;

    NsGraphFieldOutputPlugItem*
    createFieldOutputPlugItem(NsFieldOutputPlugObject *fopo,
                              NsGraphOpItem           &parent,
                              qreal                    diameter,
                              const QPointF           &plugPos,
                              const QLineF            &line,
                              const QPointF           &expTextPos,
                              const QPointF           &minTextPos,
                              const QString           &text) const;

protected:

    static qreal
    plugDiameter(qreal opDiameter);

    static bool
    isFieldPlug(const QString &sig);

    // Op state.

    static qreal
    stateDiameter(qreal opDiameter);

protected:      // Paint events.

    virtual QRectF
    boundingRect() const = 0;

    virtual QPainterPath
    shape() const = 0;

    virtual void
    paint(QPainter                       *painter,
          const QStyleOptionGraphicsItem *option,
          QWidget                        *widget);

protected:

    static const QBrush steppingBrush;
    static const QBrush errorBrush;
    static const QBrush warningBrush;
    //static const QBrush disabledBrush;

#ifdef NS_GRAPH_VIEW_DEBUG
protected:    // Mouse events.

    virtual void
    hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void
    hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
#endif  // NS_GRAPH_VIEW_DEBUG


//private slots:
//
//    void
//    _onPosChanged();

private:

    void
    _setPosition(const QPointF &p)
    { 
        if (QGraphicsObject::pos() != p) {
            QGraphicsObject::setPos(p);
        }
    }

private:    

    static const bool _isMovable    = true;
    static const bool _isSelectable = true;

private:    // Member variables.

    NsOpObject  *_op;
    QString      _longName;
    NsUndoStack *_undoStack;
    qreal        _diameter;
};

#endif // NS_GRAPH_OP_ITEM_H
