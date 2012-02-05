// -----------------------------------------------------------------------------
//
// NsGraphicsView.h
//
// Naiad Studio graphics view, header file.
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

#ifndef NS_GRAPHICS_VIEW_H
#define NS_GRAPHICS_VIEW_H

#include <QGraphicsView>
#include <QScrollBar>

class NsGraphicsScene;
class NsUndoStack;

// -----------------------------------------------------------------------------

// NsGraphicsView
// --------------
//! Abstract base class for graphics views in Naiad Studio.

class NsGraphicsView : public QGraphicsView
{
    Q_OBJECT

public:

    virtual
    ~NsGraphicsView() = 0;  // Abstract.

protected:

    explicit
    NsGraphicsView(NsUndoStack *undoStack,
                   QWidget     *parent = 0);

    explicit
    NsGraphicsView(NsGraphicsScene *graphicsScene,
                   NsUndoStack     *undoStack,
                   QWidget         *parent = 0);

protected:

    NsGraphicsScene*
    graphicsScene() const;

    //! Returns pointer to the undo stack used by this instance. May be null.
    NsUndoStack*
    undoStack() const
    { return _undoStack; }

protected:

    //! Returns the current zoom scale in the x-direction.
    qreal
    zoomScaleX() const
    { return transform().m11(); }

    //! Returns the current zoom scale in the y-direction.
    qreal
    zoomScaleY() const
    { return transform().m22(); }

    //! Sets the current zoom scales.
    void
    setZoomScale(const qreal scaleX, const qreal scaleY)
    { setTransform(QTransform::fromScale(scaleX, scaleY)); }

    //! Sets the current zoom scale as being the same in both directions.
    void
    setZoomScale(qreal scale)
    { setZoomScale(scale, scale); }

    //! Called when the view is panned using the mouse. Pans the view the
    //! given distance.
    void
    pan(const int dx, const int dy)
    {
        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - dx);
        verticalScrollBar()->setValue(verticalScrollBar()->value() - dy);
    }

protected:

    static bool
    isTopLevel(const QGraphicsItem &item,
               const QGraphicsItem *parent = 0);

private:    // Member variables.

    NsUndoStack  *_undoStack;     //!< May be null.
};

#endif // NS_GRAPHICS_VIEW_H
