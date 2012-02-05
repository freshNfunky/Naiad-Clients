// -----------------------------------------------------------------------------
//
// NsCmdSelectOp.cc
//
// Naiad Studio command class for selecting an Op, source file.
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

#include "NsCmdSelectOp.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsOpObject.h"
#include "NsOpStore.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// undo
// ----
//! Undo the selection.

void
NsCmdSelectOp::undo()
{
    qDebug() << "NsCmdSelectOp::undo()";
    ArgsList argsList;
    foreach (const _Callback &cb, _callbacks) {
        argsList += 
            Args(fromNbStr(cb.longName), 
                 NsOpObject::isSelected(fromNbStr(cb.oldValue)));
    }
    _request(argsList);
}


// delayedRedo
// -----------
//! Do the selection.

void
NsCmdSelectOp::delayedRedo() const
{
    qDebug() << "NsCmdSelectOp::delayedRedo()";
    ArgsList argsList;
    foreach (const _Callback &cb, _callbacks) {
        argsList += 
            Args(fromNbStr(cb.longName), 
                 NsOpObject::isSelected(fromNbStr(cb.newValue)));
    }
    _request(argsList);
}

// -----------------------------------------------------------------------------

// success
// -------
//! ...

void
NsCmdSelectOp::_Callback::success()
{
    qDebug() << "NsCmdSelectOp::_Callback::success()";
    _success = true;
}


// failure
// -------
//! Must never throw.

void
NsCmdSelectOp::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSetOpPosition::_Callback::failure()";
    _success = false;
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Do the actual work. [static]

NsCmdSelectOp::_CallbackList
NsCmdSelectOp::_request(const ArgsList &argsList)
{
    _CallbackList successCallbacks;
    QStringList opInstances;
    foreach (const Args &args, argsList) {
        const NsOpObject *op = 
            NsOpStore::instance()->queryConstOp(args.opInstance());
        if (0 != op && op->isSelected() != args.selected()) {
            // Check that Op exists and that change is required.

            _Callback cb; // Create a callback object to provide to NI.
            NiPushRequestCallback(&cb);
            NiSetMeta(fromQStr(args.opInstance()),
                      fromQStr(NsOpObject::selectionMetaValueType()),
                      fromQStr(NsOpObject::selectedMetaValue(args.selected())));
            if (cb.querySuccess()) {
                successCallbacks.append(cb); // Store successful callbacks.
                opInstances += args.opInstance();
            }
        }
    }

    if (!opInstances.isEmpty()) {
        NsCmdCentral::instance()->onOpSelectionChanged(opInstances, true);
    }

    return successCallbacks;
}


// _store
// ------
//! DOCS [static]

void
NsCmdSelectOp::_store(const _CallbackList &callbacks,
                      NsUndoStack         &stack,
                      const bool           merge,
                      const QString       &text)
{
    if (!callbacks.empty()) {
        stack.push(new NsCmdSelectOp(callbacks, merge, text)); //Stack owns. 
    }
}


// _store
// ------
//! DOCS [static]

void
NsCmdSelectOp::_store(const _CallbackList &callbacks,
                      QUndoCommand        *parent)
{
    if (0 != parent && !callbacks.empty()) {
        NsCmdSelectOp *child(0);
        child = new NsCmdSelectOp(callbacks, parent); // Child.
    }
}

// -----------------------------------------------------------------------------
