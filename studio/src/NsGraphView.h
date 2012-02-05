// -----------------------------------------------------------------------------
//
// NsGraphView.h
//
// Naiad Studio Op graph view, header file.
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

#ifndef NS_GRAPH_VIEW_H
#define NS_GRAPH_VIEW_H

#include "NsGraphicsView.h"
#include <QRubberBand>
#include <QGraphicsLineItem>
#include <NiTypes.h>

class NsUndoStack;
class NsGraphScene;
class NsGraphItem;
class NsGraphFeedItem;
class NsGraphPlugItem;
class NsGraphInputPlugItem;
class NsGraphOutputPlugItem;

QT_BEGIN_NAMESPACE
class QAction;
class QMouseEvent;
//class QRubberBand;
QT_END_NAMESPACE

// -----------------------------------------------------------------------------

// NsGraphView
// -----------
//! The NsGraphView class, representing the view showing the Op graph.

class NsGraphView : public NsGraphicsView
{
    Q_OBJECT

public:

    //! The different drag modes.
    enum DragMode {
        None = 0,       //!< No dragging.
        Select,         //!< Rubber band selection.
        Move,           //!< Moving the selected items.
        Pan,            //!< Panning the view.
        Zoom,           //!< Zooming the view.
        PendingFeed     //!< Pending feed between plugs.
    };

public:     // NsGraphicsView interface.

    virtual
    ~NsGraphView();

public:

    explicit
    NsGraphView(NsUndoStack *undoStack,
                QWidget     *parent = 0);

    explicit
    NsGraphView(NsGraphScene *graphScene,
                NsUndoStack  *undoStack,
                QWidget      *parent = 0);

public:

    NsGraphScene*
    graphScene() const;

    QPointF
    mapToSceneClamped(const QPoint &p, const QRect &r) const;

public:     // Actions.

    QAction*
    createZoomInAction(QObject *parent) const;

    QAction*
    createZoomOutAction(QObject *parent) const;

    QAction*
    createZoomAction(QObject *parent, int zoomScale) const;

    QAction*
    createFrameSelectedGraphAction(QObject *parent) const;

    QAction*
    createFrameAllGraphAction(QObject *parent) const;

public slots:

    void
    onItemRemoved(NsGraphItem *item);

protected slots:

    void
    onFramed(bool selected, int pad, bool success);

    void
    onCurrentVisibleFrameChanged(int cvf, bool update3DView, bool success);

    void
    onBeginTimeStep(const NtTimeBundle &tb);

    void
    onEndStep(const NtTimeBundle &tb);

protected:  // Event handling.

    virtual void
    paintEvent(QPaintEvent *event);

    virtual void
    drawBackground(QPainter *painter, const QRectF &rect);

    virtual bool
    event(QEvent *event);

    virtual void
    keyPressEvent(QKeyEvent *event)
    { NsGraphicsView::keyPressEvent(event); }

    virtual void
    mousePressEvent(QMouseEvent *event);

    virtual void
    mouseMoveEvent(QMouseEvent *event);

    virtual void
    mouseReleaseEvent(QMouseEvent *event);

    virtual void
    mouseDoubleClickEvent(QMouseEvent *event)
    { NsGraphicsView::mouseDoubleClickEvent(event); }

    virtual void
    contextMenuEvent(QContextMenuEvent *event)
    {}

    //virtual void 
    //leftClickEvent(QMouseEvent *event, QGraphicsItem *item);

    //virtual void 
    //rightClickEvent(QMouseEvent *event, QGraphicsItem *item);

protected:  // Selection.

    //! Returns true if <ctrl> is pressed.
    static bool
    toggleSelection(const Qt::KeyboardModifiers &mods)
    {  return (mods & Qt::ControlModifier); }

    //! Returns true if neither <ctrl> or <shift> is pressed.
    static bool
    clearSelection(const Qt::KeyboardModifiers &mods)
    { return !((mods & Qt::ControlModifier) || (mods & Qt::ShiftModifier)); }

protected:  // Dragging.

    const QPoint&
    dragStart() const
    { return _dragStart; }

    const QPoint&
    dragLast() const
    { return _dragLast; }

    DragMode
    dragMode() const
    { return _dragMode; }

    const QList<QGraphicsItem*>&
    dragItems() const
    { return _dragItems; }

    void
    setDragStart(const QPoint &dragStart)
    { _dragStart = dragStart; }

    void
    setDragLast(const QPoint &dragLast)
    { _dragLast  = dragLast;  }

    void
    setDragMode(const DragMode dragMode)
    { _dragMode  = dragMode; }

    void
    setDragItems(const QList<QGraphicsItem*> &dragItems)
    { _dragItems = dragItems; }

    void
    clearDragItems()
    { _dragItems.clear(); }


    //void 
    //enterDragMode(DragMode mode, const QMouseEvent &event);

    //void 
    //leaveDragMode(DragMode mode);

    // Start drag mode.

    void
    startDragMode(DragMode mode, const QMouseEvent &event);

    void
    startDragSelect(const QMouseEvent &event);

    void
    startDragPendingFeed(const QMouseEvent &event);

    void
    startDragMove(const QMouseEvent &event);

    void
    startDragPan(const QMouseEvent &event);

    void
    startDragZoom(const QMouseEvent &event);


    // Current drag mode.

    void 
    dragSelect(const QMouseEvent &event);

    void 
    dragPendingFeed(const QMouseEvent &event);

    void 
    dragMove(const QMouseEvent &event);

    void 
    dragPan(const QMouseEvent &event);

    void 
    dragZoom(const QMouseEvent &event);


    // End drag mode.

    void
    endDragSelect(const QMouseEvent &event);

    void
    endDragMove(const QMouseEvent &event);

    void
    endDragPan(const QMouseEvent &event);

    void
    endDragZoom(const QMouseEvent &event);

protected:  // Pending feed.

    NsGraphPlugItem*
    pendingPlug() const
    { return _pendingPlug; }

    void
    setPendingPlug(NsGraphPlugItem *plug)
    { _pendingPlug = plug; }

    bool
    hasPendingFeed() const
    { return (0 != pendingPlug()); }

    void
    initPendingFeed(NsGraphPlugItem &plug, const QMouseEvent &event);

    void
    commitPendingFeed(const NsGraphPlugItem &plug);

    void
    endPendingFeed();

    static bool
    validateFeedPlugs(const NsGraphPlugItem &gpi0,
                      const NsGraphPlugItem &gpi1);

    static void
    feedPlugLongNames(const NsGraphPlugItem &gpi0,
                      const NsGraphPlugItem &gpi1,
                      QString               &inputLongName,
                      QString               &plugLongName);

private slots:  // Action slots.

    void
    _zoomIn();

    void
    _zoomOut();

    void
    _setZoomScale();

    void
    _frameAll(int pad = 20);

    void
    _frameSelected(int pad = 20);

private:

    void
    _setZoomScale(qreal scaleX, qreal scaleY);

    void
    _setZoomScale(qreal scale);

    void
    _frame(const QRectF &rect, int pad = 20);

private:    // Constants.

    static const qreal _zoomScaleIncrement;
    static const qreal _zoomScaleMin;
    static const qreal _zoomScaleMax;
    static const qreal _zoomScaleDragIncrement;

private:    // Member variables.

    QPoint _dragStart;                  //!< Start position of current drag.
    QPoint _dragLast;                   //!< Previous position of current drag.
    DragMode _dragMode;                 //!< The current drag mode.
    QList<QGraphicsItem*> _dragItems;   //!< Items being dragged.

    //bool _dragSelectMerge;
    //bool _dragMoveMerge;

    QRubberBand _rubberBand;

    QGraphicsLineItem  _pendingFeedItem;
    NsGraphPlugItem   *_pendingPlug;
    NsGraphFeedItem   *_replacedFeed;

    QAction *_cutAction;                //!< The Cut action.
    QAction *_copyAction;               //!< The Copy action.
    QAction *_pasteAction;              //!< The Paste action.
    QAction *_deleteAction;             //!< The Delete action.
    QAction *_zoomInAction;             //!< The Zoom In action.
    QAction *_zoomOutAction;            //!< The Zoom Out action.
    QAction *_frameGraphAction;         //!< The Frame Graph action.
    QHash<int, QAction *> zoomActions;  //!< Maps zoom values to Zoom actions.

    QString _liveText;
};



//#include <QHash>

//class NsPlugItem;
//class NsConnectionItem;


//// NsGraphView
//// -----------
////! The NsGraphView class, representing the view showing the Op graph.

//class NsGraphView : public NsGraphicsView
//{
//    Q_OBJECT

//public:

//    NsGraphView(QWidget *parent = 0);

//    //! Returns the Cut action.
//    QAction *cutAction() const { return _cutAction; }

//    //! Returns the Copy action.
//    QAction *copyAction() const { return _copyAction; }

//    //! Returns the Paste action.
//    QAction *pasteAction() const { return _pasteAction; }

//    //! Returns the Delete action.
//    QAction *deleteAction() const { return _deleteAction; }

//    //! Returns the Select All action.
//    QAction *selectAllAction() const { return _selectAllAction; }

//    //! Returns the Zoom In action.
//    QAction *zoomInAction() const { return _zoomInAction; }

//    //! Returns the Zoom Out action.
//    QAction *zoomOutAction() const { return _zoomOutAction; }

//    //! Returns the Frame Graph action.
//    QAction *frameGraphAction() const { return _frameGraphAction; }

//    QAction *zoomAction(int zoomScale);

//public slots:

//    void onSelectionChanged();
//    void updateStateItems();

//protected:

//    virtual void mousePressEvent(QMouseEvent *event);
//    virtual void mouseMoveEvent(QMouseEvent *event);
//    virtual void mouseReleaseEvent(QMouseEvent *event);
//    virtual void keyPressEvent(QKeyEvent *event);
//    virtual bool event(QEvent *event);
//    virtual void leftClickEvent(QMouseEvent *event, QGraphicsItem *item);

//    virtual void select(QGraphicsItem *item);
//    virtual void unselect(QGraphicsItem *item);
//    virtual void unselectAll();
//    virtual void move(const QList<QGraphicsItem *> &items, qreal dx, qreal dy);


//private slots:

//    void cutSelection();
//    void copySelection();
//    void deleteSelection();
//    void selectAll();
//    void zoomIn();
//    void zoomOut();
//    void setZoomScale();
//    void onClipboardChanged();
//    void onShowOps(const QStringList &names);
//    void onReadSettings(const QString &group);
//    void onWriteSettings(const QString &group);

//private:

//    virtual void drawBackground(QPainter * painter, const QRectF & rect);
//    bool validateConnection(NsConnectionItem *connection, NsPlugItem *plug);
//    void startPendingConnection(NsPlugItem *plug);
//    void endPendingConnection(NsPlugItem *plug);
//    void abortPendingConnection();
//    void createActions();

//    QAction *_cutAction;                //!< The Cut action.
//    QAction *_copyAction;               //!< The Copy action.
//    QAction *_pasteAction;              //!< The Paste action.
//    QAction *_deleteAction;             //!< The Delete action.
//    QAction *_selectAllAction;          //!< The Select All action.
//    QAction *_zoomInAction;             //!< The Zoom In action.
//    QAction *_zoomOutAction;            //!< The Zoom Out action.
//    QAction *_frameGraphAction;         //!< The Frame Graph action.
//    QHash<int, QAction *> zoomActions;  //!< Maps zoom values to Zoom actions.

//    //! Used to merge all selection corresponding to one mouse movement into
//    //! one selection command.
//    bool mergeSelect;
//    //! Used to merge all moves corresponding to one mouse movement into one
//    //! move command.
//    bool mergeMove;

//    NsConnectionItem *pendingConnection;   //!< The pending connection.
//    NsConnectionItem *replacedConnection;  //!< The connection that would be replaced by the pending connection.

//    QColor validColor;     //!< The color used for valid connections.
//    QColor invalidColor;   //!< The color used for invalid connections.
//    QColor replacedColor;  //!< The color used connections that would be replaced.
//};

#endif // NS_GRAPH_VIEW_H
