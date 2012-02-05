// -----------------------------------------------------------------------------
//
// NsCmdSmack.cc
//
// Naiad Studio command class for setting input plug mode, source file.
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

#include "NsCmdSmack.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include "NsPlugObject.h"
#include "NsOpStore.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// undo
// ----
//! Undo the selection.

void
NsCmdSmack::undo()
{
    qDebug() << "NsCmdSmack::undo()";
    ArgsList argsList;
    foreach (const _Callback &cb, _callbacks) {
        argsList +=
            Args(fromNbStr(cb.longName), // Plug long name.
                 NsPlugObject::isSmacked(fromNbStr(cb.oldValue)));
    }
    _request(argsList);
}


// delayedRedo
// -----------
//! Do the selection.

void
NsCmdSmack::delayedRedo() const
{
    qDebug() << "NsCmdSmack::delayedRedo()";
    ArgsList argsList;
    foreach (const _Callback &cb, _callbacks) {
        argsList +=
            Args(fromNbStr(cb.longName), // Plug long name.
                 NsPlugObject::isSmacked(fromNbStr(cb.newValue)));
    }
    _request(argsList);
}

// -----------------------------------------------------------------------------

// success
// -------
//! DOCS

void
NsCmdSmack::_Callback::success()
{
    qDebug() << "NsCmdSmack::_Callback::success()";
    _success = true;
}


// failure
// -------
//! Must never throw.

void
NsCmdSmack::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSmack::_Callback::failure()";
    _success = false;
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Do the actual work. [static]

NsCmdSmack::_CallbackList
NsCmdSmack::_request(const ArgsList &argsList)
{
    _CallbackList successCallbacks;
    QStringList plugLongNames;
    foreach (const Args &args, argsList) {
        const NsPlugObject *plug =
            NsOpStore::instance()->queryConstPlug(args.plugLongName());
        if (0 != plug && plug->isSmacked() != args.smacked()) {
            // Check that plug exists and that change is required.

            _Callback cb; // Create a callback object to provide to NI.
            NiPushRequestCallback(&cb);
            NiSetMeta(
                fromQStr(args.plugLongName()),
                fromQStr(NsPlugObject::smackMetaValueType()),
                fromQStr(NsPlugObject::smackedMetaValue(args.smacked())));
            if (cb.querySuccess()) {
                successCallbacks.append(cb); // Store successful callbacks.
                plugLongNames += args.plugLongName();
            }
        }
    }

    if (!plugLongNames.isEmpty()) {
        NsCmdCentral::instance()->onSmackChanged(plugLongNames, true);
    }

    return successCallbacks;
}


// _store
// ------
//! DOCS [static]

void
NsCmdSmack::_store(const _CallbackList &callbacks,
                   NsUndoStack         &stack,
                   const bool           merge,
                   const QString       &text)
{
    if (!callbacks.empty()) {
        stack.push(new NsCmdSmack(callbacks, merge, text)); //Stack owns.
    }
}


// _store
// ------
//! DOCS [static]

void
NsCmdSmack::_store(const _CallbackList &callbacks,
                   QUndoCommand        *parent)
{
    if (0 != parent && !callbacks.empty()) {
        NsCmdSmack *child = 0;
        child = new NsCmdSmack(callbacks, parent); // Child.
    }
}

// -----------------------------------------------------------------------------
