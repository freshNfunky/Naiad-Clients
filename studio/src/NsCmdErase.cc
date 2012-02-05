// -----------------------------------------------------------------------------
//
// NsCmdErase.cc
//
// Naiad Studio command class for erasing an Op source file.
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

#include "NsCmdErase.h"
#include "NsCmdFeed.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsInputPlugObject.h"
#include "NsParserCallback.h"
#include "NsMessageWidget.h"
#include <Ni.h>
#include <QtDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once.

void
NsCmdErase::exec(const QString &opInstance)
{
    _request(opInstance, 0);
}


// exec
// ----
//! Execute as child of a parent command.

void
NsCmdErase::exec(const QString &opInstance,
                 QUndoCommand  &parent)
{
    _request(opInstance, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack.

void
NsCmdErase::exec(const QString  &opInstance,
                 NsUndoStack &stack,
                 const bool      merge)
{
    _request(opInstance,
             stack,
             merge,
             QObject::tr("Erase") + ": '" + opInstance + "'");
}

// -----------------------------------------------------------------------------

// undo
// ----
//! This method is called to undo the command.

void
NsCmdErase::undo()
{
    qDebug() << "NsCmdErase::undo()";

    NsParserCallback pcb(false);
    NiParseBuffer(_niOp, &pcb);
}


// delayedRedo
// -----------
//! This method is called by an undo stack in order to redo the command.

void
NsCmdErase::delayedRedo() const
{
    qDebug() << "NsCmdErase::delayedRedo()";
    _request(fromNbStr(_cb.opInstance));
}

// -----------------------------------------------------------------------------

// success
// -------
//! Called from successful NI request.

void
NsCmdErase::_Callback::success()
{
    qDebug() << "NsCmdErase::_Callback::success()";

    if (0 != _stack || 0 != _parent) {
        // Create a command instance.

        const NtEraseCallback *base(
            static_cast<const NtEraseCallback *>(this));

        if (0 != _parent && 0 == _stack) {
            NsCmdErase *child(0);
            child = new NsCmdErase(niOp(), *base, _parent); // Child.
        }
        else if (0 != _stack && 0 == _parent) {
            // Stack owns memory.

            _stack->push(new NsCmdErase(niOp(), *base, _merge, _text));
        }
    }

    _notify(true);
}

// -----------------------------------------------------------------------------

// failure
// -------
//! Called from unsuccessful NI request. Must never throw.

void
NsCmdErase::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdErase::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! TODO

void
NsCmdErase::_Callback::_notify(const bool success) const
{
    NsCmdCentral::instance()->onOpErased(fromNbStr(this->opInstance), success);
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Send request to NI.

void
NsCmdErase::_request(const QString  &opInstance,
                     NsUndoStack &stack,
                     const bool      merge,
                     const QString  &text)
{
    const NsOpObject *op(NsOpStore::instance()->queryConstOp(opInstance));

    if (0 != op) {
        // Op exists.

        const QByteArray niOp(NiExportOp(fromQStr(opInstance), ".ni").c_str());

        // Create a callback object and provide it to NI.

        _Callback cb(niOp, stack, merge, text);
        NiPushRequestCallback(&cb);
        NiErase(fromQStr(opInstance));
    }
}


// _request
// --------
//! Send request to NI.

void
NsCmdErase::_request(const QString &opInstance,
                     QUndoCommand  *parent)
{
    const NsOpObject *op(NsOpStore::instance()->queryConstOp(opInstance));

    if (0 != op) {
        // Op exists.

        QByteArray niOp;
        if (0 != parent) {
            niOp = NiExportOp(fromQStr(opInstance), ".ni").c_str();
        }

        // Create a callback object and provide it to NI.

        NsCmdErase::_Callback cb(niOp, parent);
        NiPushRequestCallback(&cb);
        NiErase(fromQStr(opInstance));
    }
}
