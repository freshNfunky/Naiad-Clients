// -----------------------------------------------------------------------------
//
// NsCmdSelectAllOps.h
//
// Naiad Studio command class for selecting all Ops, header file.
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

#ifndef NS_CMD_SELECT_ALL_OPS_H
#define NS_CMD_SELECT_ALL_OPS_H

#include "NsCmdBase.h"

class NsUndoStack;

// -----------------------------------------------------------------------------

// NsCmdSelectAllOps
// -----------------
//! Class for selecting all Ops with a single command.

class NsCmdSelectAllOps : public NsCmdBase
{
public:     // Static interface.

    static void
    exec(bool selected);

    static void
    exec(bool          selected,
         QUndoCommand &parent);

    static void
    exec(bool            selected,
         NsUndoStack &stack,
         bool            merge  = false);

public:     // NsCmdBase interface.

    virtual ~NsCmdSelectAllOps() {} //!< DTOR.

    virtual void
    undo()
    { NsCmdBase::undo(); }

    virtual void
    delayedRedo() const
    { NsCmdBase::delayedRedo(); }

    virtual bool
    mergeWith(const QUndoCommand *command)
    { return false; }   // TODO: Implement!

private:    // Static implementation.

    static void
    _request(bool          selected,
             QUndoCommand *parent = 0);

private:

    //! CTOR.
    explicit
    NsCmdSelectAllOps(const bool     merge,
                      const QString &text)
        : NsCmdBase(text, 0)
        , _merge(merge)
    {}

    //! CTOR.
    explicit
    NsCmdSelectAllOps(QUndoCommand *parent)
        : NsCmdBase("", parent)
        , _merge(false)
    {}

private:    // Member variables.

    bool _merge;         //!< Whether command may be merged.
};

#endif // NS_CMD_SELECT_ALL_OPS_H
