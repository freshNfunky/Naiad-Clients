// -----------------------------------------------------------------------------
//
// NsCurveViewer.h
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

#ifndef NS_CURVEVIEWER_H
#define NS_CURVEVIEWER_H

//#include "NsCurveCamera.h"
//#include "NsParameter.h"    // TODO: Fwd declaration?

#include <QGraphicsView>


class NsCurveViewer : public QGraphicsView
{
    Q_OBJECT        // Must include this if using Qt signals/slots

public:

    NsCurveViewer(QGraphicsScene *scene, QWidget *parent = 0);

protected:

    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);

    //virtual void move(const QPointF &delta);


private:    // Members.

    //NsCurveCamera _cam;
    bool zooming;
    bool panning;
    QPointF dragStart;
};

#endif  // NS_CURVEVIEWER_H
