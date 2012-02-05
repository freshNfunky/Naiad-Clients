// -----------------------------------------------------------------------------
//
// NsCurveGrid.cc
//
// Background grid for curve editing.
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

#include "NsCurveGrid.h"
#include <QtGui>


NsCurveGrid::NsCurveGrid(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , dragging(false)
{
    setAcceptsHoverEvents(true);
    //setFlags(ItemIsMovable); //| ItemIsSelectable);
    setAcceptedMouseButtons(Qt::LeftButton);
    //setCursor(QCursor(Qt::OpenHandCursor));
}


QRectF
NsCurveGrid::boundingRect() const
{

    return QRectF(-1000.0, -200.0, 2000.0, 400.0);
}


void
NsCurveGrid::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    qDebug() << "VP-Left: " << painter->viewport().bottomLeft() << ", VP-Right: "
             << painter->viewport().bottomRight();
    qDebug() << "Win-Left: " << painter->window().bottomLeft() << ", Win-Right: "
             << painter->window().bottomRight();

    const QPointF minTime(mapToScene(painter->viewport().bottomLeft()));
    const QPointF maxTime(mapToScene(painter->viewport().bottomRight()));
    qDebug() << "minT: " << minTime.x() << " | maxT: " << maxTime.x();


    painter->setPen(Qt::NoPen);
    painter->setBrush(QBrush(QColor(127, 127, 196, 255)));
    painter->drawRect(0, 0, 50, 50);
    painter->setPen(QPen(Qt::black, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawLine(QPointF(-1000.0, 0.0), QPointF(1000.0, 0.0));
    painter->drawLine(QPointF(0.0, -200.0), QPointF(0.0, 200.0));
}


// mousePressEvent
// ---------------
//! Mouse button pressed.

void
NsCurveGrid::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "NsCurveGrid::mousePressEvent";

    // TODO: Redundant?

    if (event->button() != Qt::LeftButton) {
        event->ignore();
        return;
    }

    if (!dragging && event->button() == Qt::LeftButton) {
        dragging = true;
        positionBefore = event->scenePos();
        qDebug() << "DragStart:" << positionBefore;
    }

    setCursor(QCursor(Qt::ClosedHandCursor));

    QGraphicsItem::mousePressEvent(event);
}


// mouseMoveEvent
// ---------------
//! Mouse moved.

void
NsCurveGrid::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() << "Move!";

    if (dragging)
    {
//        const QPointF delta(event->scenePos() - positionBefore);
//        this->moveBy(delta.x(), delta.y());
//        qDebug() << "Delta:" << delta << "\n";
//        foreach (QGraphicsItem *item, items())
//            item->moveBy(delta.x(), delta.y());
//
        //positionBefore = event->scenePos();
        positionBefore = scenePos();
        //setCursor(QCursor(Qt::ClosedHandCursor));
    }

    QGraphicsItem::mouseMoveEvent(event);
}


// mouseReleaseEvent
// ---------------
//! Mouse button released.

void
NsCurveGrid::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Stop dragging when left mouse button released.

    if (event->button() == Qt::LeftButton) {
        dragging = false;
        setCursor(QCursor(Qt::OpenHandCursor));
        qDebug() << "DragEnd:" << event->scenePos();
        qDebug() << "NsCurveGrid::mouseReleaseEvent";
    }

    QGraphicsItem::mouseReleaseEvent(event);
}


// hoverEnterEvent
// ---------------
//! Mouse is hovering above item.

void
NsCurveGrid::hoverEnterEvent(QGraphicsSceneHoverEvent *event )
{
    //qDebug() << "Hover IN!\n";
    setCursor(QCursor(Qt::OpenHandCursor));
    //setBrush(Qt::blue);
}


// hoverLeaveEvent
// ---------------
//! Mouse has stopped hovering above item.

void
NsCurveGrid::hoverLeaveEvent (QGraphicsSceneHoverEvent *event )
{
    //qDebug() << "Hover OUT!\n";
    setCursor(QCursor());
    //setBrush(Qt::green);
}


//void
//NsCurveGrid::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
//{
//    QMenu menu;
//    QAction *removeAction = menu.addAction("Remove");
//    QAction *markAction = menu.addAction("Mark");
//    QAction *selectedAction = menu.exec(event->screenPos());
//    // ...
//}
