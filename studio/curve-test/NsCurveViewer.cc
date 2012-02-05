// -----------------------------------------------------------------------------
//
// NsCurveViewer.cc
//
// A QWidget that draws parameters and functions as a
// combined time line/curve editor.
//
// Copyright (c) 2009 Exotic Matter AB.  All rights reserved.
//
// This material contains the confidential and proprietary information of
// Exotic Matter AB and may not be disclosed, copied or duplicated in
// any form, electronic or hardcopy, in whole or in part, without the
// express prior written consent of Exotic Matter AB. This copyright notice
// does not imply publication.
//
// -----------------------------------------------------------------------------

#include "NsCurveViewer.h"
#include <QtGui>
#include <QGLWidget>


// NsCurveViewer
// -------------
//! Construct the viewer.

NsCurveViewer::NsCurveViewer(QGraphicsScene *scene, QWidget *parent)
    : QGraphicsView(scene, parent)
    , zooming(false)
    , panning(false)
{
    setDragMode(RubberBandDrag);
    setWindowTitle("CurveEditor");
    setInteractive(true);

    setRenderHint(QPainter::Antialiasing);
    setRenderHint(QPainter::TextAntialiasing);
    setRenderHint(QPainter::HighQualityAntialiasing);

    // Scale so that positive Y is upwards in scene.

    scale(1.0, -1.0);

    setTransformationAnchor(AnchorViewCenter);

    // Setup OpenGL.

    setViewport(new QGLWidget(QGLFormat(
        QGL::DoubleBuffer    |
        QGL::DepthBuffer     |
        QGL::Rgba            | // Don't use indexed colors.
        QGL::DirectRendering | // Render directly to GPU buffers.
        QGL::SampleBuffers   | // Largest number of available samples per pixel.
        QGL::NoAlphaChannel  |
        QGL::NoAccumBuffer   |
        QGL::NoStencilBuffer |
        QGL::NoStereoBuffers |
        QGL::NoOverlay,
        0)));                     // Plane = 0, no overlays (or underlays).
}


// mousePressEvent
// ---------------
//! Mouse button has been pressed.

void
NsCurveViewer::mousePressEvent(QMouseEvent* event)
{
    qDebug() << "NsCurveViewer::mousePressEvent";

    if (!zooming && event->button() == Qt::RightButton &&
                    event->modifiers() == Qt::ControlModifier) {
        zooming = true;
        panning = false;
        dragStart = event->posF();
        //qDebug() << "ZoomStart:" << zoomBefore;

        // TODO: Zoom cursor?!
    }
    else if (!panning && event->button() == Qt::LeftButton &&
                         event->modifiers() == Qt::ControlModifier) {
        zooming = false;
        panning = true;
        dragStart = event->posF();
    }

    QGraphicsView::mousePressEvent(event);
}


// mouseMoveEvent
// --------------
//! Mouse has moved.

void
NsCurveViewer::mouseMoveEvent(QMouseEvent* event)
{

    if (zooming) {
        const QPointF delta(event->posF() - dragStart);
        //const qreal deltaLen(sqrt(delta.x()*delta.x() + delta.y()*delta.y()));
        //qDebug() << delta;

        if (!delta.isNull() /* && 1.0 < deltaLen*/) {
            //const qreal component(std::max<qreal>(delta.x(), delta.y()));
            const int minDim(std::min<int>(width(), height()));

            qreal zoomFactor(1.0);
            if (abs(delta.y()) > abs(delta.x())) {
                zoomFactor -= 2.0*delta.y()/minDim;
            }
            else
            {
                zoomFactor -= delta.x()/minDim;
            }
            //const (1.0 - (2.0*delta.y() + delta.x())/minDim);

            scale(zoomFactor, zoomFactor);
            dragStart = event->posF();
        }

        //positionBefore = event->scenePos();
        //zoomBefore = event->posF();//scenePos();
        //setCursor(QCursor(Qt::ClosedHandCursor));
    }
    else if (panning) {
        const QPointF delta(event->posF() - dragStart);
        //const qreal deltaLen(sqrt(delta.x()*delta.x() + delta.y()*delta.y()));
        //qDebug() << delta;

        if (!delta.isNull() /* && 1.0 < deltaLen*/) {
            qDebug() << matrix();
            //translate(delta.x(), delta.y());
            horizontalScrollBar()->setValue(
                horizontalScrollBar()->value() - delta.x());
            verticalScrollBar()->setValue(
                verticalScrollBar()->value() - delta.y());
            qDebug() << matrix();
            dragStart = event->posF();
        }
    }

    QGraphicsView::mouseMoveEvent(event);
}


// mouseReleaseEvent
// -----------------
//! Mouse button has been released.

void
NsCurveViewer::mouseReleaseEvent(QMouseEvent* event)
{
    qDebug() << "NsCurveViewer::mouseReleaseEvent";

    if (zooming && event->button() == Qt::RightButton) {
        zooming = false;
        //setCursor(QCursor(Qt::OpenHandCursor));
        //qDebug() << "ZoomEnd:" << event->posF();
//        qDebug() << "sceneRect: " << sceneRect().bottomLeft() << ", "
//                                  << sceneRect().bottomRight();
//        const QPointF left(mapToScene(0, 0));
//        const QPointF right(mapToScene(width() - 1, 0));
//        qDebug() << "SceneLeft: " << left << " | " << "SceneRight: "
                                  //<< right;
        // TODO: Unset zoom cursor?!
    }

    if (panning && event->button() == Qt::LeftButton) {
        panning = false;
    }

    QGraphicsView::mouseReleaseEvent(event);
}
