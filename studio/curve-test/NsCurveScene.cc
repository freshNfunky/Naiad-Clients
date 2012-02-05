// -----------------------------------------------------------------------------
//
// NsCurveScene.cc
//
// Manages items, propagates events to items and manages stats, e.g. selection.
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

#include "NsCurveScene.h"
#include <QtGui>
#include <Qt>
#include <QBrush>
#include <QColor>


// NsCurveScene
// ---------------
//! Constructor.

NsCurveScene::NsCurveScene(QObject *parent)
    : QGraphicsScene(parent)
{   // TODO: Hard-coded background color.

    setBackgroundBrush(QBrush(QColor(127, 127, 127, 255)));
    setForegroundBrush(Qt::NoBrush);
}


// mousePressEvent
// ---------------
//! Mouse button pressed.

void
NsCurveScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "NsCurveScene::mousePressEvent";
    QGraphicsScene::mousePressEvent(event);
}


// mouseMoveEvent
// ---------------
//! Mouse moved.

void
NsCurveScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
}


// mouseReleaseEvent
// ---------------
//! Mouse button released.

void
NsCurveScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    qDebug() << "NsCurveScene::mouseReleaseEvent";
    QGraphicsScene::mouseReleaseEvent(event);
}
