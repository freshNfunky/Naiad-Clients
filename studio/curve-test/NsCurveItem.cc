// -----------------------------------------------------------------------------
//
// NsCurveItem.cc
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

#include <QtGui>

#include "NsCurveItem.h"
#include "NsCurveKeyframe.h"

#include "em_keyframe.h"


// NsCurveItem
// -----------
//! Constructor, create new instance.

NsCurveItem::NsCurveItem(const QString &nodeName,
                         const QString &prmName,
                         const em::time_value *timeValue,
                         QGraphicsItem *parent)
    : QGraphicsPathItem(parent)
    , _nodeName(nodeName)
    , _prmName(prmName)
    , _timeValue(timeValue)
{
    // TODO: Handle null pointer?!

    setFlags(ItemIsMovable | ItemIsSelectable);
    //setCacheMode(DeviceCoordinateCache);    // What does it do?

    if (0 != _timeValue && _timeValue->has_keyframes()) {
        int numKF(0);
        const em::keyframe* kf(_timeValue->keyframes(&numKF));

        for(int k = 0; k < numKF; ++k) {
            NsCurveKeyframe *child = new NsCurveKeyframe(QPointF(kf[k].t(),
                                                                 kf[k].v()));
            child->setParentItem(this);
        }
    }

    QPainterPath crvPath;
    const double tMax = 1000.0;
    const double tMin = -1000.0;
    double t0 = tMin;
    double tNext = 0.0;
    crvPath.moveTo(QPointF(t0, _timeValue->eval_path(t0,
                                              tMax,
                                              -100.0,
                                              100.0,
                                              &tNext)));
    while (tNext < tMax) {
        qDebug() << tNext;
        t0 = tNext;
        crvPath.lineTo(t0, _timeValue->eval_path(t0,
                                             tMax,
                                             -100.0,
                                             100.0,
                                             &tNext));
    }

    crvPath.lineTo(tNext, _timeValue->eval(tMax));
    setPath(crvPath);
}


// boundingRect
// ------------
//! Overridden to provide a proper bounding rect.

//QRectF
//NsCurveItem::boundingRect() const
//{
//    return QRectF(0, 0, 0, 0);
//}


// paint
// -----
//! Paints the node.

void
NsCurveItem::paint(QPainter *painter,
                   const QStyleOptionGraphicsItem *option,
                   QWidget *widget)
{


    painter->setPen(isSelected() ? QPen(Qt::white, 1) : QPen(Qt::black, 1));
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());

//    if (_timeValue->has_keyframes()) {
//        int numKF(0);
//        const em::keyframe* kf(_timeValue->keyframes(&numKF));
//
//        painter->setPen(Qt::NoPen);
//        painter->setBrush(QBrush(QColor(196, 64, 64, 255)));
//
//        const double radius(1.0);
//        for(int k = 0; k < numKF; ++k)
//        {
//            painter->drawEllipse(QRectF(kf[k].t() - radius,
//                                        -kf[k].v() + radius,
//                                        2.0*radius,
//                                        2.0*radius));
//        }
//    }
}

//QPainterPath
//NsCurveItem::shape() const
//{
//    QPainterPathStroker stroker;
//    stroker.setWidth(3.0);
//    return stroker.createStroke(crvPath);
//    //return crvPath;
//}
