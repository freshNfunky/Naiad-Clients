// -----------------------------------------------------------------------------
//
// NsCmdClearGraph.h
//
// Naiad Studio command class for clearing the graph, header file.
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

#ifndef NS_CMD_CLEAR_GRAPH_H
#define NS_CMD_CLEAR_GRAPH_H

#include "NsCmdBase.h"
#include <Ni.h>

class NsUndoStack;

// -----------------------------------------------------------------------------

// NsCmdClearGraph
// ---------------
//! Command class for clearing the graph.

class NsCmdClearGraph : public NsCmdBase
{
public:     // Static interface.

    static void
    exec();

public:     // NsCmdBase interface.

    //! DTOR.
    virtual 
    ~NsCmdClearGraph() {}   

    //! Command is not undoable.
    virtual void
    undo()
    {}

    //! Command is not redoable.
    virtual void
    delayedRedo() const
    {}

    //! Merging disabled for this command.
    virtual bool
    mergeWith(const QUndoCommand *command)
    { return false; }

private:

    // _Callback
    // ---------
    //! Class for handling response from NiClearGraph().

    class _Callback : public NtClearGraphCallback
    {
    public:

        //! CTOR.
        explicit
        _Callback()
            : NtClearGraphCallback()
        {}

        //! DTOR.
        virtual 
        ~_Callback() {} 

        virtual void
        success();

        virtual void
        failure(const NtString &msg);

    private:

        void 
        _notify(bool success) const;
    };

private:    // Static implementation.

    static void
    _request();
};

#endif // NS_CMD_CLEAR_GRAPH_H
