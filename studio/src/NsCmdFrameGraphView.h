// -----------------------------------------------------------------------------
//
// NsCmdFrameGraphView.h
//
// Naiad Studio command class for framing the graph view, header file.
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

#ifndef NS_CMD_FRAME_GRAPH_VIEW_H
#define NS_CMD_FRAME_GRAPH_VIEW_H

#include "NsCmdBase.h"

// -----------------------------------------------------------------------------

// NsCmdFrameGraphView
// -------------------
//! Command class for framing the graph view.


class NsCmdFrameGraphView : public NsCmdBase
{
public:     // Static interface.

    static void
    exec(bool selected, int pad);

    static void
    exec(bool selected, int pad, QUndoCommand &parent);

public:     // NsCmdBase interface.

    //! DTOR.
    virtual
    ~NsCmdFrameGraphView()
    {}

    //! Do nothing.
    virtual void
    undo()
    {}

    //! Do nothing.
    virtual void
    delayedRedo() const
    {}

    virtual bool
    mergeWith(const QUndoCommand *command)
    { return false; }

private:    // Static implementation.

    static void
    _request(bool selected, int pad, QUndoCommand *parent = 0);

    static void
    _notify(bool selected, int pad, bool success);

private:

    //! CTOR.
    explicit
    NsCmdFrameGraphView(QUndoCommand *parent)
        : NsCmdBase("Frame Graph View", parent)
    {}
};

#endif // NS_CMD_FRAME_GRAPH_VIEW_H
