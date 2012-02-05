// -----------------------------------------------------------------------------
//
// NsCmdRename.cc
//
// Naiad Studio command class for renaming an Op instance source file.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved.
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

#include "NsCmdRename.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include <QtDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once. [static]

void
NsCmdRename::exec(const QString &oldOpInstance,
                  const QString &newOpInstance)
{
    _request(oldOpInstance, newOpInstance, 0);
}


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdRename::exec(const QString &oldOpInstance,
                  const QString &newOpInstance,
                  QUndoCommand  &parent)
{
    _request(oldOpInstance, newOpInstance, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack. [static]

void
NsCmdRename::exec(const QString  &oldOpInstance,
                  const QString  &newOpInstance,
                  NsUndoStack &stack,
                  const bool      merge)
{
    _request(oldOpInstance, newOpInstance, stack, merge, QObject::tr("Rename"));
}

// -----------------------------------------------------------------------------

// undo
// ----
//! This method is called to undo the command.

void
NsCmdRename::undo()
{
    qDebug() << "NsCmdRename::undo()";
    _request(fromNbStr(_cb.newOpInstance), fromNbStr(_cb.oldOpInstance));
}


// delayedRedo
// -----------
//! This method is called by an undo stack in order to undo the command.

void
NsCmdRename::delayedRedo() const
{
    qDebug() << "NsCmdRename::delayedRedo()";
    _request(fromNbStr(_cb.oldOpInstance), fromNbStr(_cb.newOpInstance));
}

// -----------------------------------------------------------------------------

// success
// -------
//! Called if the Op name was successfully changed in NI.

void
NsCmdRename::_Callback::success()
{
    qDebug() << "NsCmdRename::_Callback::success()";

    if (0 != _stack || 0 != _parent) {
        // Create a command instance.

        const NtRenameCallback *base(
            static_cast<const NtRenameCallback *>(this));

        if (0 != _parent && 0 == _stack) {
            NsCmdRename *child(0);
            child = new NsCmdRename(*base, _parent); // Child.
        }
        else if (0 != _stack && 0 == _parent) {
            // Stack owns memory.

            _stack->push(new NsCmdRename(*base, _merge, _text));
        }
    }

    _notify(true);
}


// failure
// -------
//! Must never throw.

void
NsCmdRename::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdRename::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Notify command central of command result.

void
NsCmdRename::_Callback::_notify(const bool success) const
{
    NsCmdCentral::instance()->onOpNameChanged(
        fromNbStr(this->oldOpInstance),
        fromNbStr(this->newOpInstance),
        success);
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Send request to NI.

void
NsCmdRename::_request(const QString  &oldOpInstance,
                      const QString  &newOpInstance,
                      NsUndoStack &stack,
                      const bool      merge,
                      const QString  &text)
{
    // Create a callback object and provide it to NI.

    _Callback cb(stack, merge, text);
    NiPushRequestCallback(&cb);
    NiRename(fromQStr(oldOpInstance), fromQStr(newOpInstance));
}


// _request
// --------
//! Send request to NI.

void
NsCmdRename::_request(const QString &oldOpInstance,
                      const QString &newOpInstance,
                      QUndoCommand  *parent)
{
    // Create a callback object and provide it to NI.

    _Callback cb(parent);
    NiPushRequestCallback(&cb);
    NiRename(fromQStr(oldOpInstance), fromQStr(newOpInstance));
}
