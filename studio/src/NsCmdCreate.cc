// -----------------------------------------------------------------------------
//
// NsCmdCreate.cc
//
// Naiad Studio command class for creating an Op source file.
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

#include "NsCmdCreate.h"
#include "NsCmdCentral.h"
#include "NsCmdErase.h"
#include "NsCmdSelectOp.h"
#include "NsCmdSelectAll.h"
#include "NsUndoStack.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once.

void
NsCmdCreate::exec(const QString &opType,
                  const QString &suggestedOpInstance,
                  QString       *createdOpInstance)
{
    _request(opType, suggestedOpInstance, 0, createdOpInstance);
}


// exec
// ----
//! Execute as child of a parent command.

void
NsCmdCreate::exec(const QString &opType,
                  const QString &suggestedOpInstance,
                  QUndoCommand  &parent,
                  QString       *createdOpInstance)
{
    _request(opType, suggestedOpInstance, &parent, createdOpInstance);
}


// exec
// ----
//! Execute and place on provided undo stack.

void
NsCmdCreate::exec(const QString  &opType,
                  const QString  &suggestedOpInstance,
                  NsUndoStack &stack,
                  const bool      merge,
                  QString        *createdOpInstance)
{
    _request(opType,
             suggestedOpInstance,
             stack,
             merge,
             QObject::tr("Create") + ": " + opType,
             createdOpInstance);
}

// -----------------------------------------------------------------------------

// undo
// ----
//! This method is called to undo the command.

void
NsCmdCreate::undo()
{
    qDebug() << "NsCmdCreate::undo()";
    NsCmdErase::exec(fromNbStr(_cb.opInstance));
}


// delayedRedo
// -----------
//! This method is called by an undo stack in order to redo the command.

void
NsCmdCreate::delayedRedo() const
{
    qDebug() << "NsCmdCreate::delayedRedo()";
    _request(fromNbStr(_cb.opType), fromNbStr(_cb.opInstance));
}

// -----------------------------------------------------------------------------

// success
// -------
//! Called from successful NI request.

void
NsCmdCreate::_Callback::success()
{
    qDebug() << "NsCmdCreate::_Callback::success()";
    qDebug() << "Op Type:" << fromNbStr(this->opType);
    qDebug() << "Op Instance:" << fromNbStr(this->opInstance);

    if (0 != _stack || 0 != _parent) {
        // Create a command instance.

        const NtCreateCallback *base(
            static_cast<const NtCreateCallback*>(this));

        if (0 != _parent && 0 == _stack) {
            NsCmdCreate *child(0);
            child = new NsCmdCreate(*base, _parent); // Child.
        }
        else if (0 != _stack && 0 == _parent) {
            // Stack owns memory.

            _stack->push(new NsCmdCreate(*base, _merge, _text));
        }

    }

    if (0 != _createdOpInstance) {
        *_createdOpInstance = fromNbStr(this->opInstance);
    }

    _notify(true);
}


// failure
// -------
//! Called from unsuccessful NI request. Must never throw.

void
NsCmdCreate::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdCreate::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! TODO

void
NsCmdCreate::_Callback::_notify(const bool success) const
{
    NsCmdCentral::instance()->onOpCreated(fromNbStr(this->opInstance), success);
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Send request to NI.

void
NsCmdCreate::_request(const QString  &opType,
                      const QString  &suggestedOpInstance,
                      NsUndoStack &stack,
                      const bool      merge,
                      const QString  &text,
                      QString        *createdOpInstance)
{
    // Create a callback object and provide it to NI. NiCreate() will call
    // either success() or failure() of the callback object.

    _Callback cb(stack, merge, text, createdOpInstance);
    NiPushRequestCallback(&cb);
    NiCreate(fromQStr(opType), fromQStr(suggestedOpInstance));
}


// _request
// --------
//! Send request to NI.

void
NsCmdCreate::_request(const QString  &opType,
                      const QString  &suggestedOpInstance,
                      QUndoCommand   *parent,
                      QString        *createdOpInstance)
{
    // Create a callback object and provide it to NI. NiCreate() will call
    // either success() or failure() of the callback object.

    _Callback cb(parent, createdOpInstance);
    NiPushRequestCallback(&cb);
    NiCreate(fromQStr(opType), fromQStr(suggestedOpInstance));
}
