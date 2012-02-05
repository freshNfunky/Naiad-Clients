// -----------------------------------------------------------------------------
//
// NsCmdBase.cc
//
// Naiad Studio command base class source file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
//
// This file is part of Open Naiad Studio..
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

#include "NsCmdBase.h"
#include <QtDebug>

// -----------------------------------------------------------------------------

// redo
// ----
//! Implementation of QUndoCommand redo()-method. QUndoCommands are pushed
//! onto a QUndoStack, where they are stored and traversed through undo()- and
//! redo()-actions. This functionality is integrated with other parts of Qt and
//! is a nice feature. However, when a QUndoCommand is pushed onto a
//! QUndoStack, its redo()-method is called. Since we want to evaluate the
//! success of a command before pushing it on the undostack, it is necessary
//! to disable the behaviour where pushed commands are executed. This is
//! implemented by making the first call to redo() do nothing except flag
//! that the next call to redo will actually perform some task.
//! -----
//! Note: Derived classes should NOT reimplement this function.

void
NsCmdBase::redo()
{
    // The first time this function is called, delayedRedo() is not called.
    // Subsequent calls will call delayedRedo().

    if (_firstRedo) {
        _firstRedo = false;
    }
    else {
        delayedRedo();
    }
}


// undo
// ----
//! Default implementation.

void
NsCmdBase::undo()
{
    QUndoCommand::undo();
}


// delayedRedo
// -----------
//! Default implementation.

void
NsCmdBase::delayedRedo() const
{
    for (int i(0); i < childCount(); ++i) {
        const NsCmdBase *cmd(dynamic_cast<const NsCmdBase*>(child(i)));
        if (0 != cmd) {
            cmd->delayedRedo();
        }
    }
}
