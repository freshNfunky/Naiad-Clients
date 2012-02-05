// -----------------------------------------------------------------------------
//
// NsGraphFeedItem.h
//
// Class for showing feeds in graph view, header file.
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

#ifndef NS_GRAPH_FEED_ITEM_H
#define NS_GRAPH_FEED_ITEM_H

#include "NsGraphItem.h"
#include "NsValueObjectHandle.h"
#include <QHash>

class NsGraphInputPlugItem;
class NsGraphPlugItem;
class NsGraphBodyItem;
class NsBodyCache;
class NsBodyObject;
class NsOpObject;

// -----------------------------------------------------------------------------

// NsGraphFeedItem
// ---------------
//! Class for showing feeds in the graph view.

class NsGraphFeedItem : public NsGraphItem
{
    Q_OBJECT

public:

    virtual
    ~NsGraphFeedItem();

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
    NsGraphFeedItem(
        NsGraphInputPlugItem &input,
        NsGraphPlugItem      &plug,
        QGraphicsItem        *parent = 0,
        qreal                 bodyItemDiameter = 16.);

public:     // Style.

    enum Style {
        Normal = 0,
        Replaced        
    };

    Style
    style() const
    { return _style; }

    void
    setStyle(Style style)
    {
        _style = style;
        update();
    }

protected slots:

    void
    onSelectionChanged()
    { selectionChanged(); }

    void
    onLiveBodyCacheChanged()
    { liveBodyCacheChanged(); }

    void
    onEmpBodyCacheChanged()
    { empBodyCacheChanged(); }

    void
    onStateChanged()
    { stateChanged(); }

    void
    onCurrentVisibleFrameChanged(const int  cvf, 
                                 const bool update3DView, 
                                 const bool success);

    void
    onBodyObjectDestroyed(NsBodyObject *bo)
    { bodyObjectDestroyed(bo); }

    void
    onPathChanged()
    { pathChanged(); }

protected:  // Hooks.

    virtual void
    selectionChanged()
    { update(); }

    virtual void
    liveBodyCacheChanged();

    virtual void
    empBodyCacheChanged();

    virtual void
    stateChanged();

    virtual void
    currentVisibleFrameChanged(const int cvf)
    { _updateBodyItems(_grabBodies(cvf)); }

    virtual void
    bodyObjectDestroyed(NsBodyObject *bo)
    { _removeBodyItem(bo); }

    virtual void
    pathChanged()
    { _setPath(); }

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

#ifdef NS_GRAPH_VIEW_DEBUG
protected:  // Mouse events.

    virtual void
    hoverEnterEvent(QGraphicsSceneHoverEvent *event);

    virtual void
    hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
#endif  // NS_GRAPH_VIEW_DEBUG

protected:

    static QPainterPath
    computeShapePath(const QPointF &p1, 
                     const QPointF &p2, 
                     bool           dummy);

    static QRectF
    computeBoundingRect(const QPainterPath &shape,
                        const qreal         pad)
    { return shape.boundingRect().adjusted(-pad, -pad, pad, pad); }

private:

    void
    _setPath();

    QList<NsBodyObject*>
    _grabBodies(int cvf);

    void
    _updateBodyItems(const QList<NsBodyObject*> &bodies);

    NsGraphBodyItem*
    _createBodyItem(NsBodyObject    *body, 
                    qreal            diameter, 
                    NsGraphFeedItem *parent);

    void
    _removeBodyItem(NsBodyObject *bo);

    void
    _clearBodyItems();

    void
    _updateBodyItemPositions();

    static const bool _isMovable    = false;
    static const bool _isSelectable = true;

private:    // Member variables.

    typedef QHash<NsValueObjectHandle, NsGraphBodyItem*> _BodyItemHashType;
    _BodyItemHashType _bodyItems;

    NsGraphInputPlugItem *_input;
    NsGraphPlugItem      *_plug;

    Style _style;

    QPainterPath _shapePath;
    QRectF       _boundingRect;

    qreal _bodyItemDiameter;
};

#endif // NS_GRAPH_FEED_ITEM_H
