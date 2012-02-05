// -----------------------------------------------------------------------------
//
// NsGraphOpItemArgs.h
//
// Naiad Studio graph op item args, header file.
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

#ifndef NS_GRAPH_OP_ITEM_ARGS_H
#define NS_GRAPH_OP_ITEM_ARGS_H

class NsOpObject;
class NsUndoStack;
class QGraphicsItem;
class QGraphicsScene;

// -----------------------------------------------------------------------------

// NsGraphOpItemArgs
// -----------------
//! Arguments.

class NsGraphOpItemArgs
{
public:

    //! CTOR.
    explicit
    NsGraphOpItemArgs(
        NsOpObject     *op,
        NsUndoStack    *undoStack,
        QGraphicsItem  *parent             = 0,
        QGraphicsScene *scene              = 0,
        const qreal     diameter           = 64.)
        : _op(op)
        , _undoStack(undoStack)
        , _parent(parent)
        , _scene(scene)
        , _diameter(diameter)
    {}

    //! DTOR.
    virtual
    ~NsGraphOpItemArgs()
    {}

    NsOpObject*
    op() const
    { return _op; }

    NsUndoStack*
    undoStack() const
    { return _undoStack; }

    QGraphicsItem*
    parent() const
    { return _parent; }

    QGraphicsScene*
    scene() const
    { return _scene; }

    qreal
    diameter() const
    { return _diameter; }

private:    // Member variables.

    NsOpObject     *_op;        //!< Op object, non-null.
    NsUndoStack    *_undoStack;
    QGraphicsItem  *_parent;    //!< Parent item.
    QGraphicsScene *_scene;     //!< Scene.
    const qreal     _diameter;  //!< Op item diameter.
};

#endif // NS_GRAPH_OP_ITEM_ARGS_H
