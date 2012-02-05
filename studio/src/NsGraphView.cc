// -----------------------------------------------------------------------------
//
// NsGraphView.cc
//
// Naiad Studio Op graph view, source file.
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

#include "NsGraphView.h"
#include "NsGraphScene.h"
#include "NsGraphOpItem.h"
#include "NsGraphOpStateItem.h"
#include "NsGraphFeedItem.h"
#include "NsGraphBodyItem.h"
#include "NsGraphPlugItem.h"
#include "NsGraphInputPlugItem.h"
#include "NsGraphOutputPlugItem.h"
#include "NsPreferences.h"
#include "NsStringUtils.h"
#include "NsCmdFeed.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsCmdSelect.h"
#include "NsCmdSelectAll.h"
#include "NsCmdSelectAllOps.h"
#include "NsCmdSelectAllBodies.h"
#include "NsCmdSelectAllFeeds.h"
#include "NsCmdSelectOp.h"
#include "NsCmdSelectBody.h"
#include "NsCmdSelectFeed.h"
#include "NsCmdSmack.h"
#include "NsCmdSetGroupPlug.h"
#include "NsCmdCreate.h"            // TMP!!
#include "NsCmdFeed.h"              // TMP!!
#include "NsCmdSetOpState.h"        // TMP!!
#include "NsCmdSetOpPosition.h"
#include "NsCmdSetParam.h"          // TMP!!
#include "NsQuery.h"
#include "NsPreferences.h"
#include "NsTabMenu.h"
#include <Ni.h>
#include <QtGui>
#include <QtOpenGL>

// -----------------------------------------------------------------------------

// ~NsGraphView
// ------------
//! DTOR.

NsGraphView::~NsGraphView()
{}

// -----------------------------------------------------------------------------

// NsGraphView
// -----------
//! CTOR.

NsGraphView::NsGraphView(NsUndoStack *undoStack, QWidget *parent)
    : NsGraphicsView(undoStack, parent)
    , _dragStart(0, 0)
    , _dragLast(0, 0)
    , _dragMode(None)
    , _rubberBand(QRubberBand::Rectangle, viewport())
    //, _pendingInput(0)
    , _pendingPlug(0)
    , _replacedFeed(0)
{}


// NsGraphView
// -----------
//! CTOR.

NsGraphView::NsGraphView(NsGraphScene   *graphScene,
                         NsUndoStack *undoStack,
                         QWidget        *parent)
    : NsGraphicsView(graphScene, undoStack, parent)
    , _dragStart(0, 0)
    , _dragLast(0, 0)
    , _dragMode(None)
    , _rubberBand(QRubberBand::Rectangle, viewport())
    //, _pendingInput(0)
    , _pendingPlug(0)
    , _replacedFeed(0)
{}

// -----------------------------------------------------------------------------

// graphScene
// ----------
//! Returns the current scene cast as a NsGraphScene. May return null.

NsGraphScene*
NsGraphView::graphScene() const
{ 
    return qobject_cast<NsGraphScene*>(scene());
}


// mapToSceneClamped
// -----------------
//! Clamp point to rectangle before transforming it to
//! scene coordinates.

QPointF
NsGraphView::mapToSceneClamped(const QPoint &p, const QRect &r) const
{
    return mapToScene(
        QPoint(qMax(r.x(), qMin(r.width(), p.x())),
               qMax(r.y(), qMin(r.height(), p.y()))));
}

// -----------------------------------------------------------------------------

// createZoomInAction
// ------------------
//! Returns a newly created zoom in action which has the provided parent.

QAction*
NsGraphView::createZoomInAction(QObject *parent) const
{
    QAction *action = new QAction(tr("Zoom &In"), parent); // Child.
    action->setStatusTip(tr("Increase the zoom scale"));
    action->setShortcut(QKeySequence::ZoomIn);
    connect(action, SIGNAL(triggered()), SLOT(_zoomIn()));
    return action;
}


// createZoomOutAction
// -------------------
//! Returns a newly created zoom out action which has the provided parent.

QAction*
NsGraphView::createZoomOutAction(QObject *parent) const
{
    QAction *action = new QAction(tr("Zoom &Out"), parent); // Child.
    action->setStatusTip(tr("Decrease the zoom scale"));
    action->setShortcut(QKeySequence::ZoomOut);
    connect(action, SIGNAL(triggered()), SLOT(_zoomOut()));
    return action;
}


// createZoomOutAction
// -------------------
//! Returns a newly created zoom out action which has the provided parent.

QAction *
NsGraphView::createZoomAction(QObject *parent, const int zoomScale) const
{
    const QString text = QString::number(zoomScale) + "%";
    QAction *action = new QAction(text, parent); // Child.
    action->setStatusTip(tr("Set the zoom scale of the graph view to ") + text);
    connect(action, SIGNAL(triggered()), SLOT(_setZoomScale()));
    return action;
}

QAction*
NsGraphView::createFrameSelectedGraphAction(QObject *parent) const
{
    QAction *action = new QAction(tr("&Frame Selection"), parent); // Child.
    action->setStatusTip(tr("Adjust the zoom scale to fit the selection"));
    action->setShortcut(Qt::Key_F);
    connect(action, SIGNAL(triggered()), SLOT(_frameSelected()));
    return action;
}

QAction*
NsGraphView::createFrameAllGraphAction(QObject *parent) const
{
    QAction *action = new QAction(tr("&Frame All"), parent); // Child.
    action->setStatusTip(tr("Adjust the zoom scale to fit the entire graph"));
    action->setShortcut(Qt::Key_A);
    connect(action, SIGNAL(triggered()), SLOT(_frameAll()));
    return action;
}

// -----------------------------------------------------------------------------

// onItemRemoved
// -------------
//! Must be called when an item is removed from the graph, so that the list of
//! drag items can be updated. [slot]

void
NsGraphView::onItemRemoved(NsGraphItem *item)
{
    //qDebug() << "NsGraphView::onItemRemoved" << item;
    _dragItems.removeAll(item);
}

// -----------------------------------------------------------------------------

//! [slot]
void
NsGraphView::onFramed(const bool selected, const int pad, const bool success)
{
    if (success) {
        selected ? _frameSelected(pad) : _frameAll(pad);
    }
}


void
NsGraphView::onCurrentVisibleFrameChanged(const int  cvf,
                                          const bool update3DView,
                                          const bool success)
{
    if (success) {
        QGraphicsView::ViewportUpdateMode push = viewportUpdateMode();
        setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
//        QPainter p(viewport());
//        drawBackground(&p, viewport()->rect());
        update();
        setViewportUpdateMode(push); // Pop.
    }
}


//! [slot]
void
NsGraphView::onBeginTimeStep(const NtTimeBundle &tb)
{
    _liveText = tr("Live Step ") + 
                "F" + QString::number(tb.frame) + "|" + 
                "T" + QString::number(tb.timestep);
    update();
}


//! [slot]
void
NsGraphView::onEndStep(const NtTimeBundle &tb)
{
    _liveText.clear();
    update();
}

// -----------------------------------------------------------------------------

void
NsGraphView::paintEvent(QPaintEvent *event)
{
#if 0
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
#else
    NsGraphicsView::paintEvent(event);

    if (!_liveText.isEmpty()) {
        // Draw text over the viewport.

        QPainter p(viewport());
        p.setPen(QPen(QColor("#666666"), 5));
        p.setFont(QFont("Sans", 32));
        p.setRenderHint(QPainter::TextAntialiasing);
        const QRectF textRect(10., 
                              10., 
                              p.fontMetrics().width(_liveText), 
                              p.fontMetrics().height());
        p.drawText(textRect, Qt::AlignCenter, _liveText);
        p.end();
    }
#endif
}


void
NsGraphView::drawBackground(QPainter *painter, const QRectF &rect)
{
    int cvf = 1;
    queryCurrentVisibleFrame(&cvf);
    painter->fillRect(
        rect,
        QBrush(
            (queryIsFrameLive(cvf) ?
                NsPreferences::instance()->graphViewLiveBackground() :
                NsPreferences::instance()->graphViewBackground())));
//    queryIsFrameLive(cvf) ?


//    QBrush fb;
//    if (queryIsFrameLive(cvf)) {
//    //if (cvf % 2 == 0) {   //
//        fb = QBrush(NsPreferences::instance()->graphViewLiveBackground());
//    }
//    else {
//        fb = QBrush(NsPreferences::instance()->graphViewBackground());
//    }
//    painter->fillRect(rect, fb);
//    painter->resetTransform();
//    painter->setClipRect(viewport()->rect());
//    painter->fillRect(viewport()->rect(), fb);
    //NsGraphicsView::drawBackground(painter, rect); 
}


// event
// -----
//! Overridden to make it possible to abort pending connection when mouse
//! leaves view.

bool
NsGraphView::event(QEvent *event)
{
    switch (dragMode()) {
    case None:
    case Select:
    case Move:
    case Pan:
    case Zoom:
        if (QEvent::KeyPress == event->type()) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);   // Safe.

            if (Qt::Key_Tab == keyEvent->key()) {
                // Map mouse position from global (i.e. screen) coords
                // to this widget.

                if (rect().contains(mapFromGlobal(QCursor::pos()))) {
                    // Mouse cursor is inside viewport.

                    NsTabMenu tabMenu(*this, undoStack());
                    tabMenu.exec(QCursor::pos());
                }
                return true;    // Event was processed.
            }
        }
        break;
    case PendingFeed:
        if (QEvent::MouseMove == event->type()) {
            // Somewhat strange way to do this, but it's the only
            // way I found that works.

            endPendingFeed();
        }
        break;
    default:
        EM_ASSERT(false);
    }

    return NsGraphicsView::event(event);
}


// mousePressEvent
// ---------------
//! Overridden to provide our own moving, selection, panning and zooming.

void
NsGraphView::mousePressEvent(QMouseEvent *event)
{
    // TODO: Make mouse clicks and keyboard modifiers customisable

    qDebug() << "NsGraphView::mousePressEvent";

    // Get modifiers and buttons.

    const Qt::KeyboardModifiers modifiers = event->modifiers();
    const Qt::MouseButtons buttons        = event->buttons();

    const bool alt   = modifiers & Qt::AltModifier;
    const bool meta  = modifiers & Qt::MetaModifier;
    const bool ctrl  = modifiers & Qt::ControlModifier;
    const bool shift = modifiers & Qt::ShiftModifier;
    const bool lmb   = buttons   & Qt::LeftButton;
    const bool mmb   = buttons   & Qt::MidButton;
    const bool rmb   = buttons   & Qt::RightButton;

    switch (dragMode()) {
    case None:
    //case Select:
    //case Move:
    //case Pan:
    //case Zoom:
        EM_ASSERT(!hasPendingFeed());

        // If <alt> and/or <meta> are pressed we are doing 2D camera moves.

        if ((alt && rmb) || (alt && meta && lmb)) {
            // <alt> + RMB or <alt> + <meta> + LMB = zooming.

            startDragMode(Zoom, *event);
        }
        else if ((alt && mmb) || (meta && lmb)) {
            // <alt> + MMB or <meta> + LMB = panning.

            startDragMode(Pan, *event);
        }
        else {  // Handle cases where no modifiers are present.
            if (lmb) {
                // Left mouse button.

                QGraphicsItem *item = itemAt(event->pos());

                //NsGraphItem *item(
                //    dynamic_cast<NsGraphItem*>(
                //        itemAt(event->pos())));

                if (0 != item) {
                    // An item was left-clicked.

                    NsGraphItem *graphItem = dynamic_cast<NsGraphItem*>(item);
                    if (0 != graphItem && graphItem->isItemSelectable()) {
                        const bool selected = graphItem->isSelected();
                        const bool select =
                            toggleSelection(event->modifiers()) ?
                                !selected : true;

                        if (select != selected) {
                            // Item selection needs to be changed.

                            if (clearSelection(event->modifiers())) {
                                EM_ASSERT(0 != undoStack());
                                NsCmdSelectAll::exec(false, *undoStack());
                            }

                            graphItem->setSelected(select,undoStack(),false);
                        }

                        if (select && graphItem->isItemMovable()) {
                            // A moveable item was selected.

                            startDragMode(Move, *event);
                        }
                        // else: no drag mode entered.
                        break;
                    }
                    
                    NsGraphPlugItem *plugItem(
                        dynamic_cast<NsGraphPlugItem*>(item));
                    if (0 != plugItem) {
                        // A plug item was clicked. Note that plug items are not
                        // selectable.

                        initPendingFeed(*plugItem, *event);
                        startDragMode(PendingFeed, *event);
                        break;
                    }

                    NsGraphOpStateItem *stateItem(
                        dynamic_cast<NsGraphOpStateItem*>(item));
                    if (0 != stateItem) {
                        EM_ASSERT(0 != undoStack());
                        if (fromNbStr(NI_ACTIVE) == stateItem->op()->state()) {
                            NsCmdSetOpState::exec(
                                NsCmdSetOpState::ArgsList() <<
                                    NsCmdSetOpState::Args(
                                        stateItem->longName(),
                                        fromNbStr(NI_INACTIVE)),
                                    *undoStack());
                        }
                        else {
                            NsCmdSetOpState::exec(
                                NsCmdSetOpState::ArgsList() <<
                                    NsCmdSetOpState::Args(
                                        stateItem->longName(),
                                        fromNbStr(NI_ACTIVE)),
                                    *undoStack());
                        }
                        break;
                    }
                }
                else {
                    // If no item was clicked, start rubber band selection.

                    if (clearSelection(event->modifiers())) {
                        EM_ASSERT(0 != undoStack());
                        NsCmdSelectAll::exec(false, *undoStack());
                    }

                    clearDragItems();
                    startDragMode(Select, *event);
                }
            }
            else if (rmb) {
                if (ctrl) {
                    NsGraphPlugItem *plugItem(
                        dynamic_cast<NsGraphPlugItem*>(
                            itemAt(event->pos())));

                    if (0 != plugItem) {
                        EM_ASSERT(0 != undoStack());
                        NsCmdSmack::exec(
                            NsCmdSmack::ArgsList() <<
                                NsCmdSmack::Args(
                                    plugItem->plugObject()->longName(),
                                    !plugItem->plugObject()->isSmacked()),
                                *undoStack());
                    }
                }
                else {
                    NsGraphPlugItem *item(
                        dynamic_cast<NsGraphPlugItem*>(
                                itemAt(event->pos())));
                    if (0 != item) {
                        // TODO: Check plug mode!

                        EM_ASSERT(0 != undoStack());
                        NsCmdSetGroupPlug::exec(
                            item->plugObject()->longName(), 
                            !item->plugObject()->isGroup(), 
                            *undoStack());
                    }
                }
            }
        }
        break;
    case PendingFeed:
        if (hasPendingFeed()) {
            EM_ASSERT(hasPendingFeed());
            if ((alt && rmb) || (alt && meta && lmb)) {
                // <alt> + RMB or <alt> + <meta> + LMB = zooming.

                startDragMode(Zoom, *event);
            }
            else if ((alt && mmb) || (meta && lmb)) {
                // <alt> + MMB or <meta> + LMB = panning.

                startDragMode(Pan, *event);
            }
            else {
                if (lmb) {
                    foreach (QGraphicsItem *item, items(event->pos())) {
                        NsGraphPlugItem *plugItem =
                            dynamic_cast<NsGraphPlugItem *>(item);
                        if (0 != plugItem &&
                            validateFeedPlugs(*plugItem, *pendingPlug())) {
                            commitPendingFeed(*plugItem);
                            break;  // Valid plug found, we are done!
                        }
                    }

                    endPendingFeed();
                    //setMouseTracking(false);
                    setDragMode(None);
                }
            }
        }
        else {
            setDragMode(None);
        }
        break;
    case Select:
    case Move:
    case Pan:
    case Zoom:
    default:
        break;
        //EM_ASSERT(false);
    }

    //event->accept();    // Don't propagate to scene.
    NsGraphicsView::mousePressEvent(event);
}


// mouseMoveEvent
// --------------
//! Overridden to provide our own moving, selection, panning and zooming.

void
NsGraphView::mouseMoveEvent(QMouseEvent *event)
{
    //qDebug() << "NsGraphView::mouseMoveEvent";

    switch (dragMode()) {
    case None:
        break;
    case Select:
        dragSelect(*event);
        break;
    case PendingFeed:
        dragPendingFeed(*event);
        break;
    case Move:
        dragMove(*event);
        break;
    case Pan:
        dragPan(*event);
        break;
    case Zoom:
        dragZoom(*event);
        break;
    default:
        EM_ASSERT(false);
    }

    // Update position of mouse _after_ the current drag event
    // has been processed.

    setDragLast(event->pos());

    //event->accept();    // Don't propagate to scene.
    NsGraphicsView::mouseMoveEvent(event);
}


// mouseReleaseEvent
// -----------------
//! Overridden to provide our own moving, selection, panning and zooming.

void
NsGraphView::mouseReleaseEvent(QMouseEvent *event)
{
    qDebug() << "NsGraphView::mouseReleaseEvent";

    switch (dragMode()) {
    case None:
        break;
    case Select:
        endDragSelect(*event);
        break;
    case PendingFeed:
        //endDragPendingFeed(*event);
        break;
    case Move:
        endDragMove(*event);
        break;
    case Pan:
        endDragPan(*event);
        break;
    case Zoom:
        endDragZoom(*event);
        break;
    default:
        EM_ASSERT(false);
    }

    //event->accept();  // Don't propagate to scene.
    NsGraphicsView::mouseReleaseEvent(event);
}

// -----------------------------------------------------------------------------

//! [static]
const qreal NsGraphView::_zoomScaleIncrement(0.2);

//! The minimum scale. [static]
const qreal NsGraphView::_zoomScaleMin(0.02);

//! The maximum zoom scale. [static]
const qreal NsGraphView::_zoomScaleMax(2.);

//!< The scale increment in relation to the drag. [static]
const qreal NsGraphView::_zoomScaleDragIncrement(5.);

// -----------------------------------------------------------------------------

// _zoomIn
// -------
//! Increases the zoom scale. [slot]

void
NsGraphView::_zoomIn()
{
    _setZoomScale(zoomScaleX() + _zoomScaleIncrement,
                  zoomScaleY() + _zoomScaleIncrement);
}


// _zoomOut
// --------
//! Decreases the zoom scale. [slot]

void
NsGraphView::_zoomOut()
{
    _setZoomScale(zoomScaleX() - _zoomScaleIncrement,
                  zoomScaleY() - _zoomScaleIncrement);
}


// _frameAll
// ---------
//! Adjusts the zoom scale to fit all items.

void
NsGraphView::_frameAll(const int pad)
{
    if (0 != graphScene()) {
        _frame(graphScene()->itemsBoundingRect(), pad);
    }
}


// _frameSelected
// --------------
//! Adjusts the zoom scale to fit all items.

void
NsGraphView::_frameSelected(const int pad)
{
    NsGraphScene *gs = graphScene();
    if (0 != gs) {
        QRectF boundingRect;
        bool selection = false;
        foreach (QGraphicsItem *item, gs->items()) {
            NsGraphItem *graphItem = dynamic_cast<NsGraphItem*>(item);
            if ((0 != graphItem) && graphItem->isSelected()) {
                boundingRect |= graphItem->sceneBoundingRect(); // Union.
                selection = true;
            }
        }
        selection ? _frame(boundingRect, pad) : _frameAll(pad);
    }
}


// _setZoomScale
// -------------
//! Sets the zoom scale to the value held by the sender. [slot]

void
NsGraphView::_setZoomScale()
{
    QAction *action = qobject_cast<QAction *>(sender());
    if (action) {
        const QString text(action->text().remove('%').trimmed());
        bool ok(false);
        const int zoomPercent(text.toInt(&ok));
        if (ok) {
            // Scale from percent to actual scale.

            _setZoomScale(0.01*zoomPercent);
        }
    }
}


// _setZoomScale
// -------------
//! Sets the zoom scale to clamped to the allowed range.

void
NsGraphView::_setZoomScale(const qreal zsx, const qreal zsy)
{
    NsGraphicsView::setZoomScale(
        qMax(_zoomScaleMin, qMin(_zoomScaleMax, zsx)),
        qMax(_zoomScaleMin, qMin(_zoomScaleMax, zsy)));
}


// _setZoomScale
// -------------
//! Sets the zoom scale to clamped to the allowed range.

void
NsGraphView::_setZoomScale(const qreal zs)
{
    NsGraphicsView::setZoomScale(qMax(_zoomScaleMin, qMin(_zoomScaleMax, zs)));
}


// _frame
// ------
//! Ensure the provided rectangle is in view.

void
NsGraphView::_frame(const QRectF &rect, const int pad)
{
    fitInView(rect.adjusted(-pad, -pad, pad, pad), Qt::KeepAspectRatio);
    _setZoomScale(zoomScaleX(), zoomScaleY());  // Check zoom scale.
}

// -----------------------------------------------------------------------------

void
NsGraphView::startDragMode(const DragMode dragMode, const QMouseEvent &event)
{
    switch (dragMode) {
    case Select:
        startDragSelect(event);
        break;
    case PendingFeed:
        startDragPendingFeed(event);
        break;
    case Move:
        startDragMove(event);
        break;
    case Pan:
        startDragPan(event);
        break;
    case Zoom:
        startDragZoom(event);
        break;
    case None:
    default:
        EM_ASSERT(false);
    }

    setDragMode(dragMode);
    setDragStart(event.pos());
    setDragLast(event.pos());
}

void
NsGraphView::startDragSelect(const QMouseEvent &event)
{
    qDebug() << "Start drag Select";
}

void
NsGraphView::startDragPendingFeed(const QMouseEvent &event)
{
    qDebug() << "Start drag Pending Feed";
    viewport()->setMouseTracking(true);
}

void
NsGraphView::startDragMove(const QMouseEvent &event)
{
    qDebug() << "Start drag Move";
}

void
NsGraphView::startDragPan(const QMouseEvent &event)
{
    qDebug() << "Start drag Pan";
    viewport()->setCursor(Qt::ClosedHandCursor);
}

void
NsGraphView::startDragZoom(const QMouseEvent &event)
{
    qDebug() << "Start drag Zoom";
    // TODO: Set zoom cursor?!
}

// -----------------------------------------------------------------------------

void
NsGraphView::dragSelect(const QMouseEvent &event)
{
    // Check that drag is large enough to start rubberband selection.

    const QRect dragRect = QRect(dragStart(), event.pos()).normalized();
    const int dragDist = dragRect.width() + dragRect.height();

    if (dragDist >= QApplication::startDragDistance()) {
        // Start rubber band selection.

        _rubberBand.setGeometry(dragRect);
        _rubberBand.show();

        // Get list of items with shapes intersecting the drag rectangle.

        const QList<QGraphicsItem*> rectItems =
            graphScene()->items(mapToScene(dragRect),
                                Qt::IntersectsItemShape,
                                Qt::AscendingOrder);

        // Select or toggle selection for new items in rubberband rect.

        //QList<NsGraphItem*> newDragItems;
        QList<QGraphicsItem*> oldDragItems = _dragItems;//(dragItems());

        foreach (QGraphicsItem *item, rectItems) {
            if (!dragItems().contains(item)) {
                // New drag select item.

                NsGraphItem *graphItem(dynamic_cast<NsGraphItem*>(item));
                if (0 != graphItem && graphItem->isItemSelectable()) {
                    graphItem->setItemSelected(//true);
                        toggleSelection(event.modifiers()) ? 
                            !graphItem->isSelected() : true);
                    _dragItems.append(item);
                    //newDragItems.append(item);
                }
            }
        }

        // Unselect or toggle selection for items no longer in rubberband rect.

        //QList<NsGraphItem*> oldDragItems = ;
        foreach (QGraphicsItem *item, oldDragItems) {
            if (!rectItems.contains(item)) {
                // Item no longer in rubber-band.

                NsGraphItem *graphItem(dynamic_cast<NsGraphItem*>(item));
                if (0 != graphItem && graphItem->isItemSelectable()) {
                    graphItem->setItemSelected(//false);
                        toggleSelection(event.modifiers()) ? 
                            graphItem->isSelected() : false);
                    _dragItems.removeAll(item);
                    //oldDragItems.append(item);
                }
            }
        }

        //setDragItems(rectItems);
    }
}


void
NsGraphView::dragPendingFeed(const QMouseEvent &event)
{
    NsGraphScene *gs = graphScene();
    if (hasPendingFeed() && 0 != gs) {
        _pendingFeedItem.setLine(
            QLineF(_pendingFeedItem.line().p1(),
                   mapToScene(event.pos())));

        bool reset = true;
        foreach (QGraphicsItem *item, items(event.pos())) {
            // Check for plug items underneath the mouse cursor.

            NsGraphPlugItem *plugItem(dynamic_cast<NsGraphPlugItem*>(item));
            if (0 != plugItem && plugItem != pendingPlug()) {
                // Item underneath the mouse is a plug and it is not the
                // pending plug.

                reset = false;
                if (validateFeedPlugs(*plugItem, *pendingPlug())) {
                    _pendingFeedItem.setPen(
                     QPen(
                       NsPreferences::instance()->graphViewFeedValidLineColor(),
                       2.));

                    QString inputLongName;
                    QString plugLongName;
                    feedPlugLongNames(*plugItem, *pendingPlug(),
                                      inputLongName, plugLongName);

                    _replacedFeed =
                        gs->queryMutableFeedItem(
                            inputLongName);
                    if (0 != _replacedFeed) {
                        _replacedFeed->setStyle(NsGraphFeedItem::Replaced);
                    }
                }
                else {
                    _pendingFeedItem.setPen(
                     QPen(
                      NsPreferences::instance()->graphViewFeedInvalidLineColor(),
                      2.));
                }
            }
        }

        if (reset) {
            _pendingFeedItem.setPen(
                QPen(NsPreferences::instance()->graphViewFeedLineColor(), 2.));
            if (0 != _replacedFeed) {
                _replacedFeed->setStyle(NsGraphFeedItem::Normal);
                _replacedFeed = 0;
            }
        }
    }
}


void
NsGraphView::dragMove(const QMouseEvent &event)
{
    NsGraphScene *gs = graphScene();
    if (0 != gs) {
        const QPointF dp = mapToScene(event.pos()) - mapToScene(dragLast());
        foreach (NsGraphOpItem *opItem, gs->selectedOpItems()) {
            opItem->setItemPosition(opItem->pos() + dp);
        }
    }
}


void
NsGraphView::dragPan(const QMouseEvent &event)
{
    pan(event.pos().x() - dragLast().x(), event.pos().y() - dragLast().y());
}


// dragZoom
// --------
//! DOCS

void
NsGraphView::dragZoom(const QMouseEvent &event)
{
    // TODO: Should we center zoom around startPos and not center of view?

    const QPoint dp = event.pos() - dragLast();
    qreal sx = zoomScaleX();
    qreal sy = zoomScaleY();
    if (qAbs(dp.x()) >= qAbs(dp.y())) {
        const qreal ds(dp.x()*(0.001*_zoomScaleDragIncrement));
        sx += ds;
        sy += ds;
    }
    else {
        const qreal ds(dp.y()*(0.001*_zoomScaleDragIncrement));
        sx -= ds;
        sy -= ds;
    }
    _setZoomScale(sx, sy);
}

// -----------------------------------------------------------------------------

void
NsGraphView::endDragSelect(const QMouseEvent &event)
{
    EM_ASSERT(!hasPendingFeed());
    qDebug() << "End Drag Select";
    if (!dragItems().isEmpty()) {
        NsCmdSelectOp::ArgsList opArgsList;
        NsCmdSelectBody::ArgsList bodyArgsList;
        NsCmdSelectFeed::ArgsList feedArgsList;

        bool opFirst = false;

        foreach (QGraphicsItem *item, dragItems()) {
            NsGraphOpItem *opItem = dynamic_cast<NsGraphOpItem*>(item);
            if (0 != opItem) {
                opArgsList += 
                    NsCmdSelectOp::Args(
                        opItem->longName(), opItem->isItemSelected());
                opItem->setItemSelected(false);

                if (bodyArgsList.isEmpty()) {
                    opFirst = true;
                }

                continue;
            }

            NsGraphBodyItem *bodyItem = dynamic_cast<NsGraphBodyItem*>(item);
            if (0 != bodyItem) {
                bodyArgsList += 
                    NsCmdSelectBody::Args(
                        bodyItem->longName(), bodyItem->isItemSelected());
                bodyItem->setItemSelected(false);
                continue;
            }

            NsGraphFeedItem *feedItem = dynamic_cast<NsGraphFeedItem*>(item);
            if (0 != feedItem) {
                feedArgsList += 
                    NsCmdSelectFeed::Args(
                        feedItem->longName(), feedItem->isItemSelected());
                feedItem->setItemSelected(false);
                continue;
            }
        } 

        NsCmdSelect *cmd = new NsCmdSelect("Select");
        if (opFirst) {
            NsCmdSelectBody::exec(bodyArgsList, *cmd);
            NsCmdSelectOp::exec(opArgsList, *cmd);
        }
        else {
            NsCmdSelectOp::exec(opArgsList, *cmd);
            NsCmdSelectBody::exec(bodyArgsList, *cmd);
        }
        NsCmdSelectFeed::exec(feedArgsList, *cmd);

        if (0 != undoStack()) {
            undoStack()->push(cmd); // Stack owns.
        }
        else {
            delete cmd;
        }
    }

    _rubberBand.hide();
    clearDragItems();
    setDragMode(None);
}

void
NsGraphView::endDragMove(const QMouseEvent &event)
{
    EM_ASSERT(!hasPendingFeed());
    qDebug() << "End Drag Move";

    NsGraphScene *gs = graphScene();
    if (0 != gs) {
        NsCmdSetOpPosition::ArgsList argsList;
        foreach (NsGraphOpItem *opItem, graphScene()->selectedOpItems()) {
            const QPointF ip = opItem->itemPosition();
            if (ip != opItem->op()->position()) {
                argsList +=
                    NsCmdSetOpPosition::Args(
                        opItem->op()->longName(), ip.x(), ip.y());
            }
        }

        if (!argsList.isEmpty()) {
            if (0 != undoStack()) {
                NsCmdSetOpPosition::exec(argsList, *undoStack(), false);
            }
            else {
                NsCmdSetOpPosition::exec(argsList);
            }
        }
    }
    setDragMode(None);
}

void
NsGraphView::endDragPan(const QMouseEvent &event)
{
    if (!hasPendingFeed()) {
        //EM_ASSERT(!hasPendingFeed());
        qDebug() << "End Drag Pan";
        viewport()->unsetCursor();
        setDragMode(None);
    }
    else {
        setDragMode(PendingFeed);
    }
}

void
NsGraphView::endDragZoom(const QMouseEvent &event)
{
    if (!hasPendingFeed()) {
        //EM_ASSERT(!hasPendingFeed());
        qDebug() << "End Drag Zoom";
        setDragMode(None);
    }
    else {
        setDragMode(PendingFeed);
    }
}

// -----------------------------------------------------------------------------

// initPendingFeed
// ---------------
//! DOCS.

void
NsGraphView::initPendingFeed(NsGraphPlugItem &plug, const QMouseEvent &event)
{
    if (!hasPendingFeed()) {
        setPendingPlug(&plug);
        _replacedFeed = 0;
        NsGraphScene *gs = graphScene();
        if (0 != gs) {
            _pendingFeedItem.setPen(
                QPen(NsPreferences::instance()->graphViewFeedLineColor(), 2));
            _pendingFeedItem.setLine(
                QLineF(plug.opItem()->mapToScene(
                           plug.mapFromItem(plug.opItem(), plug.plugPos())),
                       mapToScene(event.pos())));
            _pendingFeedItem.setZValue(-1.);
            gs->addItem(&_pendingFeedItem);
        }
    }
}


// commitPendingFeed
// -----------------
//! This function does nothing if there is no pending feed.

void
NsGraphView::commitPendingFeed(const NsGraphPlugItem &gpi)
{
    if (hasPendingFeed()) {
        if (validateFeedPlugs(gpi, *pendingPlug())) {
            QString inputLongName;
            QString plugLongName;
            feedPlugLongNames(gpi, *pendingPlug(), inputLongName, plugLongName);
            if (0 != undoStack()) {
                NsCmdFeed::exec(inputLongName, plugLongName, *undoStack());
            }
            else {
                NsCmdFeed::exec(inputLongName, plugLongName);
            }
            endPendingFeed();
        }
    }
}


// endPendingFeed
// --------------
//! Reset values and remove temporary item from graph.

void
NsGraphView::endPendingFeed()
{
    if (hasPendingFeed()) {
        setPendingPlug(0);
        _replacedFeed = 0;
        NsGraphScene *gs = graphScene();
        if (0 != gs) {
            gs->removeItem(&_pendingFeedItem);
        }
    }
}

// -----------------------------------------------------------------------------

// feedPlugLongNames
// -----------------
//! Should always validate the provided plugs, otherwise the strings
//! might be bogus. [static]

void
NsGraphView::feedPlugLongNames(const NsGraphPlugItem &gpi0,
                               const NsGraphPlugItem &gpi1,
                               QString               &inputLongName,
                               QString               &plugLongName)
{
    if (validateFeedPlugs(gpi0, gpi1)) {
        // If the provided plugs are valid we are guaranteed to be able
        // to return valid long names.

        if (gpi0.plugObject()->isDummy()) {
            inputLongName = gpi0.plugObject()->longName();
            plugLongName = gpi1.plugObject()->longName();
        }
        else if (gpi1.plugObject()->isDummy()) {
            inputLongName = gpi1.plugObject()->longName();
            plugLongName = gpi0.plugObject()->longName();
        }
        else {
            // No dummy plugs.

            if (0 != dynamic_cast<const NsGraphInputPlugItem*>(&gpi0)) {
                inputLongName = gpi0.plugObject()->longName();
                plugLongName = gpi1.plugObject()->longName();
            }
            else {
                inputLongName = gpi1.plugObject()->longName();
                plugLongName = gpi0.plugObject()->longName();
            }
        }
    }
}


// validateFeedPlugs
// -----------------
//! Returns true if it is possible to create a feed between the two provided
//! plugs. [static]

bool
NsGraphView::validateFeedPlugs(const NsGraphPlugItem &gpi0,
                               const NsGraphPlugItem &gpi1)
{
#if 0
    qDebug() << "PlugLongName0:" << gpi0.plugObject()->longName();
    qDebug() << "PlugLongName1:" << gpi1.plugObject()->longName();
    qDebug() << "Valid:" << queryValidFeed(
            gpi0.plugObject()->longName(), gpi1.plugObject()->longName());
#endif
    return queryValidFeed(
        gpi0.plugObject()->longName(), gpi1.plugObject()->longName());

}

// -----------------------------------------------------------------------------





//#include <QtGui>
//#include <QtOpenGL>

//#include <NiTypes.h>

//#include "NsGraphView.h"
//#include "NsGraph.h"
//#include "NsBodyOpItem.h"
//#include "NsCameraScopeItem.h"  // thinks
//#include "NsScopeItem.h"
//#include "NsStateItem.h"
//#include "NsPlugItem.h"
//#include "NsConnectionItem.h"

//#include "NsQuery.h"
//#include "NsCmdSetOpState.h"



//// NsGraphView
//// -----------
////! Constructor, creates a new graph view.

//NsGraphView::NsGraphView(QWidget *parent)
//    : NsGraphicsView(parent),
//      mergeSelect(false),
//      mergeMove(false),
//      pendingConnection(0),
//      replacedConnection(0),
//      validColor(QColor(Qt::green).darker(140)),
//      invalidColor(QColor(Qt::red).darker(140)),
//      replacedColor(QColor(Qt::gray).lighter(135))
//{
//    createActions();

//    setSceneRect(-1e9, -1e9, 2 * 1e9, 2 * 1e9);
//    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
//    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

//    connect(QApplication::clipboard(), SIGNAL(dataChanged()),
//            SLOT(onClipboardChanged()));
//    connect(Ns::ns(), SIGNAL(selectionChanged()),
//            SLOT(onSelectionChanged()));
//    connect(Ns::ns(), SIGNAL(showOps(const QStringList &)),
//            SLOT(onShowOps(const QStringList &)));
//    connect(Ns::ns(), SIGNAL(readSettings(const QString &)),
//            SLOT(onReadSettings(const QString &)));
//    connect(Ns::ns(), SIGNAL(writeSettings(const QString &)),
//            SLOT(onWriteSettings(const QString &)));

//    connect(Ns::ns(), SIGNAL(currentFrameChanged(const int)),
//            SLOT(updateStateItems()));

//    connect(Ns::ns(), SIGNAL(opParameterChanged
//                             (const QString &, const QString &,
//                              const QString &, NtComponent)),
//            SLOT(updateStateItems()));

//    connect(Ns::ns(), SIGNAL(beginOp()),
//            SLOT(repaint()));

//    connect(Ns::ns(), SIGNAL(endOp()),
//            SLOT(repaint()));

//    connect(Ns::ns(), SIGNAL(errorMessage(const QString &)),
//            SLOT(frameGraph()));
//}





//// mouseReleaseEvent
//// -----------------
////! Overridden to enable ending/aborting of pending connection on mouse release
////! on any plug except the plug from which the pending connection was started.

//void
//NsGraphView::mouseReleaseEvent(QMouseEvent *event)
//{
//    if (pendingConnection) {
//        QGraphicsItem *item = itemAt(event->pos());
//        if (item) {
//            NsPlugItem *plugItem = dynamic_cast<NsPlugItem *>(item);
//            if (plugItem &&
//                plugItem != pendingConnection->source() &&
//                plugItem != pendingConnection->terminal()) {
//                if (validateConnection(pendingConnection, plugItem))
//                    endPendingConnection(plugItem);
//                else
//                    abortPendingConnection();
//                event->accept();
//                return;
//            }
//        }
//    }

//    NsGraphicsView::mouseReleaseEvent(event);
//}


//// keyPressEvent
//// -------------
////! Overridden to provide arrow key moving.

//void
//NsGraphView::keyPressEvent(QKeyEvent *event)
//{
//    NsGraph *graph = qobject_cast<NsGraph *>(graph());
//    if (graph) {
//        QStringList ops = graph->selectedOps();
//        if (!ops.isEmpty()) {
//            switch (event->key()) {
//                case Qt::Key_Up:
//                    Ns::moveOpsCommand(ops, 0, -2, true);
//                    return;
//                case Qt::Key_Down:
//                    Ns::moveOpsCommand(ops, 0, 2, true);
//                    return;
//                case Qt::Key_Left:
//                    Ns::moveOpsCommand(ops, -2, 0, true);
//                    return;
//                case Qt::Key_Right:
//                    Ns::moveOpsCommand(ops, 2, 0, true);
//                    return;
//                default:
//                    break;
//            }
//        }
//    }

//    NsGraphicsView::keyPressEvent(event);
//}


//// event
//// -----
////! Overridden to make it possible to abort pending connection when mouse
////! leaves view.

//bool
//NsGraphView::event(QEvent *event)
//{
//    // Somewhat strange way to do this, but it's the only way I found that works

//    if (event->type() == QEvent::MouseMove && pendingConnection)
//        abortPendingConnection();

//    return NsGraphicsView::event(event);
//}







//// move
//// ----
////! Overridden. Moves the given Ops the given distance using a move command.
////! Makes sure move commands from the same mouse movement are merged.

//void
//NsGraphView::move(const QList<QGraphicsItem *> &items, qreal dx, qreal dy)
//{
//    QStringList opNames;
//    foreach (QGraphicsItem *item, items) {
//        NsOpItem *opItem = dynamic_cast<NsOpItem *>(item);
//        if (opItem)
//            opNames += opItem->name();
//    }

//    NsGraph *graph = qobject_cast<NsGraph *>(graph());
//    if (graph && !opNames.isEmpty()) {
//        Ns::moveOpsCommand(opNames, dx, dy, mergeMove);
//        mergeMove = true;
//    }
//}


















//// onShowOps
//// ----------
////! Make the given Ops visible in the view.

//void
//NsGraphView::onShowOps(const QStringList &names)
//{
//    NsGraph *graph = qobject_cast<NsGraph *>(graph());
//    if (!graph)
//        return;

//    QRectF rect;
//    bool first = true;

//    foreach(const QString &name, names) {
//        NsOpItem *item = graph->findOp(name);
//        if (item) {
//            if (first) {
//                rect = item->sceneBoundingRect();
//                first = false;
//            } else
//                rect = rect.united(item->sceneBoundingRect());
//        }
//    }

//    ensureVisible(rect, 25, 25);
//}


//// onReadSettings
//// --------------
////! Reads settings from the given group.

//void
//NsGraphView::onReadSettings(const QString &group)
//{
//    reset();

//    QSettings settings;
//    settings.beginGroup(group);
//    settings.beginGroup("NsGraphView");
//    bool ok;

//    qreal zoomScale = settings.value("ZoomScale").toReal(&ok);
//    if (ok) NsGraphicsView::setZoomScale(zoomScale);

//    int x = settings.value("X").toInt(&ok);
//    if (ok) horizontalScrollBar()->setValue(x);

//    int y = settings.value("Y").toInt(&ok);
//    if (ok) verticalScrollBar()->setValue(y);

//    settings.endGroup();
//    settings.endGroup();
//}


//// onWriteSettings
//// ---------------
////! Writes settings to the given group.

//void
//NsGraphView::onWriteSettings(const QString &group)
//{
//    QSettings settings;
//    settings.beginGroup(group);
//    settings.beginGroup("NsGraphView");

//    settings.setValue("ZoomScale", zoomScale());
//    settings.setValue("X", horizontalScrollBar()->value());
//    settings.setValue("Y", verticalScrollBar()->value());

//    settings.endGroup();
//    settings.endGroup();
//}
