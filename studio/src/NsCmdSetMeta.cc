// -----------------------------------------------------------------------------
//
// NsCmdSetMeta.cc
//
// Naiad Studio command class for setting meta values source file.
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

#include "NsCmdSetMeta.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include <QtDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once. [static]

void
NsCmdSetMeta::exec(const QString  &longName,
                   const QString  &valueType,
                   const QString  &value)
{
    _request(longName, valueType, value, 0);
}


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdSetMeta::exec(const QString &longName,
                   const QString &valueType,
                   const QString &value,
                   QUndoCommand  &parent)
{
    _request(longName, valueType, value, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack. [static]

void
NsCmdSetMeta::exec(const QString  &longName,
                   const QString  &valueType,
                   const QString  &value,
                   NsUndoStack &stack,
                   const bool merge)
{
    _request(longName, valueType, value, stack, merge, "Set Meta");
}

// -----------------------------------------------------------------------------

// undo
// ----
//! This method is called by an undostack in order to undo the command.

void
NsCmdSetMeta::undo()
{
    qDebug() << "NsCmdSetMeta::undo()";
    _request(fromNbStr(_cb.longName), 
             fromNbStr(_cb.valueType), 
             fromNbStr(_cb.oldValue));
}


// delayedRedo
// -----------
//! This method is called by an undostack in order to undo the command.

void
NsCmdSetMeta::delayedRedo() const
{
    qDebug() << "NsCmdSetMeta::delayedRedo()";
    _request(fromNbStr(_cb.longName),
             fromNbStr(_cb.valueType),
             fromNbStr(_cb.newValue));
}

// -----------------------------------------------------------------------------

// mergeWith
// ---------
//! ...

bool
NsCmdSetMeta::mergeWith(const QUndoCommand *command)
{
    return false;

//    // Check if argument has same type as this object using RTTI.

//    const NsCmdSetMeta *other(dynamic_cast<const NsCmdSetMeta *>(command));

//    // Make sure the two commands refer to the same parameter of
//    // the same Op.

//    if (0 != other && other->_merge /*&&
//        other->_cb.longName == _cb.longName &&
//        other->_cb.valueType == _cb.valueType*/) {

////        foreach (const _State)

////        _State state;
////        state.longName =
////        _states.append();

//        _cb.newValue = other->_cb.newValue;
//        //oldValue = other->_oldValue;

//        return true;
//    }

//    return false;
}

// -----------------------------------------------------------------------------

// success
// -------
//! ...

void
NsCmdSetMeta::_Callback::success()
{
    //qDebug() << "NsCmdSetMeta::_Callback::success()";

    if (0 != _stack || 0 != _parent) {
        const NtSetMetaCallback *base(
            static_cast<const NtSetMetaCallback *>(this));

        if (0 != _parent && 0 == _stack) {
            NsCmdSetMeta *child(0);
            child = new NsCmdSetMeta(*base, _parent); // Child.
        }
        else if (0 != _stack && 0 == _parent) {
            // Stack owns memory.

            _stack->push(new NsCmdSetMeta(*base, _merge, _text));
        }
    }

    _notify(true);
}


// failure
// -------
//! Must never throw.

void
NsCmdSetMeta::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSetMeta::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Notify undo stack.

void
NsCmdSetMeta::_Callback::_notify(const bool success) const
{
    NsCmdCentral::instance()->onMetaChanged(
        fromNbStr(this->longName),
        fromNbStr(this->valueType),
        fromNbStr(this->newValue),
        success);
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Make request to NI to have the specified meta value changed. Response
//! to this request is provided through a callback object.

void
NsCmdSetMeta::_request(const QString  &longName,
                       const QString  &valueType,
                       const QString  &value,
                       NsUndoStack &stack,
                       const bool      merge,
                       const QString  &text)
{
    // Create a callback object and provide it to NI.

    NsCmdSetMeta::_Callback cb(stack, merge, text);
    NiPushRequestCallback(&cb);
    NiSetMeta(fromQStr(longName), fromQStr(valueType), fromQStr(value));
}


// _request
// --------
//! Make request to NI to have the specified meta value changed. Response
//! to this request is provided through a callback object.

void
NsCmdSetMeta::_request(const QString  &longName,
                       const QString  &valueType,
                       const QString  &value,
                       QUndoCommand   *parent)
{
    // Create a callback object and provide it to NI.

    NsCmdSetMeta::_Callback cb(parent);
    NiPushRequestCallback(&cb);
    NiSetMeta(fromQStr(longName), fromQStr(valueType), fromQStr(value));
}
