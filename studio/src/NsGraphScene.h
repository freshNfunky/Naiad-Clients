// -----------------------------------------------------------------------------
//
// NsGraphScene.h
//
// Naiad Studio Op graph, header file.
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

#ifndef NS_GRAPH_SCENE_H
#define NS_GRAPH_SCENE_H

#include "NsGraphicsScene.h"
#include "NsValueObjectHandle.h"
#include "NsGraphItem.h" // TMP!!
#include <QHash>
#include <QtGui>    // TMP!!

class NsOpObject;
class NsUndoStack;
class NsGraphItem;
class NsGraphOpItem;
class NsGraphInputPlugItem;
class NsGraphPlugItem;
class NsGraphFeedItem;

// -----------------------------------------------------------------------------

// NsGraphScene
// ------------
//! Class for visually manipulating the Naiad Op graph. Manages top-level
//! graph items, i.e. items with no parents.

class NsGraphScene : public NsGraphicsScene
{
    Q_OBJECT

public:     // NsGraphicsScene interface.

    virtual
    ~NsGraphScene();

public:

    explicit
    NsGraphScene(NsUndoStack *undoStack, QObject *parent = 0);

    bool
    containsOpItem(const NsValueObjectHandle &handle) const;

    NsGraphFeedItem*
    queryMutableFeedItem(const QString &inputLongName) const;

    void
    eraseItem(NsGraphItem *item)
    { _eraseItem(item); }

    QList<NsGraphOpItem*>
    selectedOpItems();

protected slots:

    void
    onOpObjectCreated(NsOpObject *op)
    { _createOpItem(op); }

    void
    onOpObjectDestroyed(NsOpObject *op)
    { _eraseOpItem(op); }

    void
    onFeedChanged(const QString &inputLongName,
                  const QString &plugLongName,
                  bool           success);

    void
    onGraphCleared(bool success);

protected:

    virtual void
    drawBackground(QPainter *painter, const QRectF &rect);

    virtual void
    mousePressEvent(QGraphicsSceneMouseEvent *event);

    virtual void
    mouseMoveEvent(QGraphicsSceneMouseEvent *event);

    virtual void
    mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    virtual void
    mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

signals:

    void
    itemRemoved(NsGraphItem *item);

private:    // Op items.

    void
    _createOpItem(NsOpObject *op);

    void
    _eraseOpItem(NsOpObject *op);

    NsGraphOpItem*
    _findOpItem(const NsValueObjectHandle &handle) const;

private:    // Feed items.

    void
    _createFeedItem(const QString &inputLongName,
                    const QString &plugLongName);

    void
    _eraseFeedItem(const QString &inputLongName);


    // TODO: These are not nicely implemented...

    NsGraphFeedItem*
    _findFeedItem(const QString &inputLongName) const;

    NsGraphPlugItem*
    _findPlugItem(const QString &plugLongName) const;

    NsGraphInputPlugItem*
    _findInputPlugItem(const QString &inputLongName) const;

private:

    void
    _eraseItem(NsGraphItem *item);

    QList<QGraphicsItem*>
    _topLevelItems() const;

private:

    static bool
    _isTopLevel(const QGraphicsItem *item);

private:    // Member variables.

    typedef QHash<NsValueObjectHandle, NsGraphOpItem*> _OpItemHashType;
    _OpItemHashType _opItems;

    NsUndoStack *_undoStack;
};

// -----------------------------------------------------------------------------

//class NsGraphNoteItem : public NsGraphItem
//{
//    Q_OBJECT
//
//public:
//
//    NsGraphNoteItem(QGraphicsItem *parent)    
//
//private:
//}


#endif // NS_GRAPH_SCENE_H







//// moveOp
//// -------
////! Moves the given op the given distance.

//void
//Ns::moveOp(const QString &name, qreal dx, qreal dy)
//{
//    NsOpItem *item = _graph->findOp(name);
//    if (item) {
//        item->moveBy(dx, dy);
//        item->updateConnections();
//        //setOpMeta(name, "x", QString::number(item->x()));
//        //setOpMeta(name, "y", QString::number(item->y()));
//        NsCmdSetMeta::run(name, "x", QString::number(item->x()));
//        NsCmdSetMeta::run(name, "y", QString::number(item->y()));
//    }
//}


//// moveOps
//// --------
////! Moves the given ops so that their top left is at the given position.

//void
//Ns::moveOps(const QStringList &names, const QPointF &pos)
//{
//    QPointF itemPos;
//    bool first = true;

//    foreach(const QString &name, names) {
//        NsOpItem *item = _graph->findOp(name);
//        if (item) {
//            if (first) {
//                itemPos.setX(item->x());
//                itemPos.setY(item->y());
//                first = false;
//            } else {
//                itemPos.setX(qMin(itemPos.x(), item->x()));
//                itemPos.setY(qMin(itemPos.y(), item->y()));
//            }
//        }
//    }

//    foreach(const QString &name, names)
//        moveOp(name, pos.x() - itemPos.x(), pos.y() - itemPos.y());
//}
