// -----------------------------------------------------------------------------
//
// NsCmdFeed.cc
//
// Naiad Studio command class for creating feeds, source file.
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

#include "NsCmdFeed.h"
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
NsCmdFeed::exec(const QString  &inputLongName,
                const QString  &plugLongName)
{
    _request(inputLongName, plugLongName);
}


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdFeed::exec(const QString  &inputLongName,
                const QString  &plugLongName,
                QUndoCommand   &parent)
{
    _request(inputLongName, plugLongName, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack. [static]

void
NsCmdFeed::exec(const QString  &inputLongName,
                const QString  &plugLongName,
                NsUndoStack &stack,
                const bool      merge)
{
    _request(inputLongName,
             plugLongName,
             stack,
             merge,
             plugLongName.isEmpty()?QObject::tr("Unfeed"):QObject::tr("Feed"));
}

// -----------------------------------------------------------------------------

// undo
// ----
//! This method is called to undo the command.

void
NsCmdFeed::undo()
{
    qDebug() << "NsCmdFeed::undo()";
    _request(fromNbStr(_cb.inputLongName), fromNbStr(_cb.oldPlugLongName));
}


// delayedRedo
// -----------
//! This method is called by an undo stack in order to redo the command.

void
NsCmdFeed::delayedRedo() const
{
    qDebug() << "NsCmdFeed::delayedRedo()";
    _request(fromNbStr(_cb.inputLongName), fromNbStr(_cb.newPlugLongName));
}


// success
// -------
//! Called from successful NI request.

void
NsCmdFeed::_Callback::success()
{
    qDebug() << "NsCmdFeed::_Callback::success()";

    if (0 != _stack || 0 != _parent) {
        // Create a command instance.

        const NtFeedCallback *base(
            static_cast<const NtFeedCallback *>(this));

        if (0 != _parent && 0 == _stack) {
            NsCmdFeed *child(0);
            child = new NsCmdFeed(*base, _parent); // Child.
        }
        else if (0 != _stack && 0 == _parent) {
            // Stack owns memory.

            _stack->push(new NsCmdFeed(*base, _merge, _text));
        }
    }

    _notify(true);
}


// failure
// -------
//! Called from unsuccessful NI request. Must never throw.

void
NsCmdFeed::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdFeed::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Emit signal through command central.

void
NsCmdFeed::_Callback::_notify(const bool success) const
{
    NsCmdCentral::instance()->onFeedChanged(
        fromNbStr(this->inputLongName),
        fromNbStr(this->newPlugLongName),
        success);
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Send request to NI.

void
NsCmdFeed::_request(const QString  &inputLongName,
                    const QString  &plugLongName,
                    NsUndoStack &stack,
                    const bool      merge,
                    const QString  &text)
{
    // Create a callback object and provide it to NI.

    _Callback cb(stack, merge, text);
    NiPushRequestCallback(&cb);
    NiFeed(fromQStr(inputLongName), fromQStr(plugLongName));
}


// _request
// --------
//! Send request to NI.

void
NsCmdFeed::_request(const QString  &inputLongName,
                    const QString  &plugLongName,
                    QUndoCommand   *parent)
{
    // Create a callback object and provide it to NI.

    _Callback cb(parent);
    NiPushRequestCallback(&cb);
    NiFeed(fromQStr(inputLongName), fromQStr(plugLongName));
}
