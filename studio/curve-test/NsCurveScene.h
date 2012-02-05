// -----------------------------------------------------------------------------
//
// NsCurveScene.h
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

#ifndef NS_CURVESCENE_H
#define NS_CURVESCENE_H

#include <QGraphicsScene>


// NsCurveScene
// ------------
//! Holds information about selected nodes and their parameters (time values),
//! such that the user can visually edit these parameters, which
//! are then updated accordingly.

class NsCurveScene : public QGraphicsScene
{
    Q_OBJECT        // Must include this if using Qt signals/slots.

public:

    NsCurveScene(QObject *parent = 0);

    // NB: Items of the scene must inherit from QGraphicsItem.
    // Add items to the scene through: addItem(QGraphicsItem* item)
    // Similarly, remove items with:   removeItem(QGraphicsItem* item)

    //void addParameter(const NsParameter& prm);
    //void removeParameter(const NsParameter& prm);

//public slots:

    //void onNodeSelectionChanged(const QStringList &selection);
    //void onNodeNameChanged(const QString &oldName, const QString &newName);

protected:

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
};

#endif // NS_CURVESCENE_H
