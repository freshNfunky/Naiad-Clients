// -----------------------------------------------------------------------------
//
// NsCmdSetGroupPlug.cc
//
// Naiad Studio command class for setting a plug group flag, source file.
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

#include "NsCmdSetGroupPlug.h"
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
NsCmdSetGroupPlug::exec(const QString &plugLongName,
                        const bool     group)
{
    _request(plugLongName, group, 0);
}


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdSetGroupPlug::exec(const QString &plugLongName,
                        const bool     group,
                        QUndoCommand  &parent)
{
    _request(plugLongName, group, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack. [static]

void
NsCmdSetGroupPlug::exec(const QString &plugLongName,
                        const bool     group,
                        NsUndoStack   &stack,
                        const bool     merge)
{
    qDebug() << "NsCmdSetGroupPlug::exec - " << plugLongName;
    _request(plugLongName,
             group,
             stack,
             merge,
             "Slap");    // No parent!
}

// -----------------------------------------------------------------------------

// undo
// ----
//! This method is called to undo the command.

void
NsCmdSetGroupPlug::undo()
{
    qDebug() << "NsCmdSetGroupPlug::undo()";
    _request(fromNbStr(_cb.plugLongName), _cb.oldValue);
}


// delayedRedo
// -----------
//! This method is called by an undostack in order to undo the command.

void
NsCmdSetGroupPlug::delayedRedo() const
{
    qDebug() << "NsCmdSetGroupPlug::delayedRedo()";
    _request(fromNbStr(_cb.plugLongName), _cb.newValue);
}


// mergeWith
// ---------
//! Merge two commands.

bool
NsCmdSetGroupPlug::mergeWith(const QUndoCommand *command)
{
    // Check if argument has same type as this object using RTTI.

    const NsCmdSetGroupPlug *other(
        dynamic_cast<const NsCmdSetGroupPlug*>(command));

    // Make sure the two commands refer to the same Op.

    if (0 != other &&
        other->_merge &&
        other->_cb.plugLongName == _cb.plugLongName) {
        _cb.newValue = other->_cb.newValue;
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

// success
// -------
//! Called if a parameter is successfully changed in NI.

void
NsCmdSetGroupPlug::_Callback::success()
{
    qDebug() << "NsCmdSetGroupPlug::_Callback::success()";

    if (0 != _stack || 0 != _parent) {
        // Create a command instance.

        const NtSetGroupPlugCallback *base(
            static_cast<const NtSetGroupPlugCallback*>(this));

        if (0 != _parent && 0 == _stack) {
            NsCmdSetGroupPlug *child(0);
            child = new NsCmdSetGroupPlug(*base, _parent); // Child.
        }
        else if (0 != _stack && 0 == _parent) {
            // Stack owns memory.

            _stack->push(new NsCmdSetGroupPlug(*base, _merge, _text));
        }
    }

    _notify(true);
}


// failure
// -------
//! Must never throw.

void
NsCmdSetGroupPlug::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSetGroupPlug::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Notify the undo stack.

void
NsCmdSetGroupPlug::_Callback::_notify(const bool success) const
{
    NsCmdCentral::instance()->onGroupPlugChanged(
        fromNbStr(this->plugLongName),
        //this->newValue,
        success);
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Send request to NI. [static]

void
NsCmdSetGroupPlug::_request(const QString  &plugLongName,
                            const bool      group,
                            NsUndoStack    &stack,
                            const bool      merge,
                            const QString  &text)
{
    // Create a callback object and provide it to NI.

    _Callback cb(stack, merge, text);
    NiPushRequestCallback(&cb);
    NiSetGroupPlug(fromQStr(plugLongName), group);
}


// _request
// --------
//! Send request to NI. [static]

void
NsCmdSetGroupPlug::_request(const QString &plugLongName,
                            const bool     group,
                            QUndoCommand  *parent)
{
    // Create a callback object and provide it to NI.

    _Callback cb(parent);
    NiPushRequestCallback(&cb);
    NiSetGroupPlug(fromQStr(plugLongName), group);
}
