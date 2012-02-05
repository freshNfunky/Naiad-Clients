// -----------------------------------------------------------------------------
//
// NsCurveItem.h
//
// Represents a node parameter that is drawn in the curve editor.
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

#ifndef NS_CURVEITEM_H
#define NS_CURVEITEM_H

//#include <QGraphicsItem>
#include <QGraphicsPathItem>
#include "em_time_value.h"


class NsCurveItem : public QGraphicsPathItem
{
    //Q_OBJECT        // Must include this if using Qt signals/slots.

public:

    NsCurveItem(const QString &nodeName,
                const QString &prmName,
                const em::time_value *timeValue,
                QGraphicsItem *parent = 0);

    //QRectF boundingRect() const;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option,
               QWidget *widget);

    //QPainterPath shape() const;

    const QString& nodeName() const { return _nodeName; }
    const QString& prmName() const { return _prmName; }

protected:

//    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
//    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
//    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
//    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
//    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private:
    QString _nodeName;
    QString _prmName;
    const em::time_value *_timeValue;
};

#endif // NS_CURVEITEM_H
