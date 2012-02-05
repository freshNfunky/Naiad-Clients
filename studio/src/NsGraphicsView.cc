// -----------------------------------------------------------------------------
//
// NsGraphicsView.cc
//
// Naiad Studio graphics view, source file.
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

#include "NsGraphicsView.h"
#include "NsGraphicsScene.h"
#include <QGraphicsItem>

// -----------------------------------------------------------------------------

// ~NsGraphicsView
// ---------------
//! DTOR.

NsGraphicsView::~NsGraphicsView()
{}

// -----------------------------------------------------------------------------

// NsGraphicsView
// --------------
//! CTOR.

NsGraphicsView::NsGraphicsView(NsUndoStack *undoStack,
                               QWidget     *parent)
    : QGraphicsView(parent)
    , _undoStack(undoStack)
{}


// NsGraphicsView
// --------------
//! CTOR.

NsGraphicsView::NsGraphicsView(NsGraphicsScene *graphicsScene,
                               NsUndoStack     *undoStack,
                               QWidget         *parent)
    : QGraphicsView(graphicsScene, parent)
    , _undoStack(undoStack)
{}

// -----------------------------------------------------------------------------

// graphicsScene
// -------------
//! Returns pointer to the graphics scene used by this class. May be null.

NsGraphicsScene*
NsGraphicsView::graphicsScene() const
{ return qobject_cast<NsGraphicsScene*>(scene()); }

// -----------------------------------------------------------------------------

// isTopLevel
// -----------
//! True if item has the provided parent. By default parent is zero, which
//! means that true is only returned for top-level items. [static]

bool
NsGraphicsView::isTopLevel(const QGraphicsItem &item,
                           const QGraphicsItem *parent)
{ return (parent == item.parentItem()); }

// -----------------------------------------------------------------------------
