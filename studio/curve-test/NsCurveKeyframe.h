// -----------------------------------------------------------------------------
//
// NsCurveKeyframe.h
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

#ifndef NS_CURVE_KEYFRAME_H
#define NS_CURVE_KEYFRAME_H

#include <QGraphicsItem>


class NsCurveKeyframe : public QGraphicsItem
{
    //Q_OBJECT        // Must include this if using Qt signals/slots.

public:
    NsCurveKeyframe(const QPointF &pos, QGraphicsItem *parent = 0);

    NsCurveKeyframe(qreal posX, qreal posY, QGraphicsItem *parent = 0);

    QRectF boundingRect() const;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

protected:
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:

};

#endif  // NS_CURVE_KEYFRAME_H
