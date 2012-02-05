// -----------------------------------------------------------------------------
//
// NsCurveKeyframe.cc
//
// Item for editing curve keyframes through the Qt mechanisms.
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

#include "NsCurveKeyframe.h"
#include <QtGui>

#include <sstream>


NsCurveKeyframe::NsCurveKeyframe(const QPointF &pos, QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    setAcceptsHoverEvents(true);
    setFlags(ItemIsMovable | ItemIsSelectable);
    //setAcceptedMouseButtons(Qt::LeftButton);
    //setCursor(QCursor(Qt::OpenHandCursor));

    setPos(pos);
}


NsCurveKeyframe::NsCurveKeyframe(qreal posX,
                                 qreal posY,
                                 QGraphicsItem *parent)
    : QGraphicsItem(parent)
{
    setAcceptsHoverEvents(true);
    setFlags(ItemIsMovable | ItemIsSelectable);
    //setAcceptedMouseButtons(Qt::LeftButton);
    //setCursor(QCursor(Qt::OpenHandCursor));

    setPos(posX, posY);
}


QRectF
NsCurveKeyframe::boundingRect() const
{
    return QRectF(-2.0, -2.0, 4.0, 4.0);
}


void
NsCurveKeyframe::paint(QPainter *painter,
                       const QStyleOptionGraphicsItem *option,
                       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
    painter->setPen(Qt::NoPen);
    painter->setBrush(isSelected() ? QBrush(QColor(196, 127, 127, 255)) :
                                     QBrush(QColor(196, 196, 127, 255)));
    painter->drawRect(QRectF(-2.0, -2.0, 4.0, 4.0));
}


// mousePressEvent
// ---------------
//! Mouse button pressed.

void
NsCurveKeyframe::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "NsCurveKeyframe::mousePressEvent";

    // TODO: Redundant?

//    if (event->button() != Qt::LeftButton) {
//        event->ignore();
//        return;
//    }
//
//    if (!dragging && event->button() == Qt::LeftButton) {
//        dragging = true;
//        positionBefore = event->scenePos();
//        qDebug() << "DragStart:" << positionBefore;
//    }
//
//    setCursor(QCursor(Qt::ClosedHandCursor));

    QGraphicsItem::mousePressEvent(event);
}


// mouseMoveEvent
// ---------------
//! Mouse moved.

void
NsCurveKeyframe::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
//    qDebug() << "Move!";

//    if (dragging)
//    {
//        const QPointF delta(event->scenePos() - positionBefore);
//        this->moveBy(delta.x(), delta.y());
////        qDebug() << "Delta:" << delta << "\n";
////        foreach (QGraphicsItem *item, items())
////            item->moveBy(delta.x(), delta.y());
////
//        //positionBefore = event->scenePos();
//        positionBefore = scenePos();
//        //setCursor(QCursor(Qt::ClosedHandCursor));
//    }

    QGraphicsItem::mouseMoveEvent(event);
}


// mouseReleaseEvent
// ---------------
//! Mouse button released.

void
NsCurveKeyframe::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    // Stop dragging when left mouse button released.

//    if (event->button() == Qt::LeftButton) {
//        dragging = false;
//        setCursor(QCursor(Qt::OpenHandCursor));
//        qDebug() << "DragEnd:" << event->scenePos();
//        qDebug() << "NsCurveGrid::mouseReleaseEvent";
//    }
    std::stringstream ss;
    ss << scenePos().x() << ":" << scenePos().y();
    setToolTip(ss.str().c_str());

    QGraphicsItem::mouseReleaseEvent(event);
}


// hoverEnterEvent
// ---------------
//! Mouse is hovering above item.

void
NsCurveKeyframe::hoverEnterEvent(QGraphicsSceneHoverEvent *event )
{
    //qDebug() << "Hover IN!\n";
    //setCursor(QCursor(Qt::OpenHandCursor));
//    std::stringstream ss;
//    ss << scenePos().x() << ":" << scenePos().y();
//    setToolTip(ss.str().c_str());
    //setBrush(QColor(216, 127, 127, 255));
}


// hoverLeaveEvent
// ---------------
//! Mouse has stopped hovering above item.

void
NsCurveKeyframe::hoverLeaveEvent (QGraphicsSceneHoverEvent *event )
{
    //qDebug() << "Hover OUT!\n";
    //setCursor(QCursor());
    //setBrush(QColor(196, 127, 127, 255));
    //setToolTip("ToolTip [out]!");
}
