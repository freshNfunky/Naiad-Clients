// -----------------------------------------------------------------------------
//
// NsCmdSelectFeed.cc
//
// Naiad Studio command class for selecting a feed, source file.
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

#include "NsCmdSelectFeed.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include "NsInputPlugObject.h"
#include "NsOpStore.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// undo
// ----
//! Undo the selection.

void
NsCmdSelectFeed::undo()
{
    qDebug() << "NsCmdSelectFeed::undo()";
    ArgsList argsList;
    foreach (const _Callback &cb, _callbacks) {
        argsList += 
            Args(fromNbStr(cb.longName), // Input long name.
                 NsInputPlugObject::isSelected(fromNbStr(cb.oldValue)));
    }
    _request(argsList);
}


// delayedRedo
// -----------
//! Do the selection.

void
NsCmdSelectFeed::delayedRedo() const
{
    qDebug() << "NsCmdSelectFeed::delayedRedo()";
    ArgsList argsList;
    foreach (const _Callback &cb, _callbacks) {
        argsList += 
            Args(fromNbStr(cb.longName), // Input long name.
                 NsInputPlugObject::isSelected(fromNbStr(cb.newValue)));
    }
    _request(argsList);
}

// -----------------------------------------------------------------------------

// success
// -------
//! DOCS

void
NsCmdSelectFeed::_Callback::success()
{
    qDebug() << "NsCmdSelectFeed::_Callback::success()";
    _success = true;
}


// failure
// -------
//! Must never throw.

void
NsCmdSelectFeed::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSelectFeed::_Callback::failure()";
    _success = false;
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Do the actual work. [static]

NsCmdSelectFeed::_CallbackList
NsCmdSelectFeed::_request(const ArgsList &argsList)
{
    _CallbackList successCallbacks;
    QStringList inputLongNames;
    foreach (const Args &args, argsList) {
        const NsInputPlugObject *ipo = 
            NsOpStore::instance()->queryConstInput(args.inputLongName());
        if (0 != ipo && ipo->isSelected() != args.selected()) {
            // Input exists and change is required.

            _Callback cb; // Create a callback object to provide to NI.
            NiPushRequestCallback(&cb);
            NiSetMeta(
                fromQStr(args.inputLongName()),
                fromQStr(NsInputPlugObject::selectionMetaValueType()),
                fromQStr(NsInputPlugObject::selectedMetaValue(args.selected())));
            if (cb.querySuccess()) {
                successCallbacks.append(cb); // Store successful callbacks.
                inputLongNames += args.inputLongName();
            }
        }
    }

    if (!inputLongNames.isEmpty()) {
        NsCmdCentral::instance()->onFeedSelectionChanged(inputLongNames, true);
    }

    return successCallbacks;
}


// _store
// ------
//! DOCS [static]

void
NsCmdSelectFeed::_store(const _CallbackList &callbacks,
                        NsUndoStack         &stack,
                        const bool           merge,
                        const QString       &text)
{
    if (!callbacks.empty()) {
        stack.push(new NsCmdSelectFeed(callbacks, merge, text)); //Stack owns. 
    }
}


// _store
// ------
//! DOCS [static]

void
NsCmdSelectFeed::_store(const _CallbackList &callbacks,
                        QUndoCommand        *parent)
{
    if (0 != parent && !callbacks.empty()) {
        NsCmdSelectFeed *child(0);
        child = new NsCmdSelectFeed(callbacks, parent); // Child.
    }
}

// -----------------------------------------------------------------------------
