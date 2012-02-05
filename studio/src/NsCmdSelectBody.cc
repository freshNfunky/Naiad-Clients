// -----------------------------------------------------------------------------
//
// NsCmdSelectBody.cc
//
// Naiad Studio command class for selecting a body, source file.
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

#include "NsCmdSelectBody.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsOpStore.h"
#include "NsBodyObject.h"
#include "NsStringUtils.h"
#include <NbBody.h>
#include <QDebug>

// -----------------------------------------------------------------------------


// -----------------------------------------------------------------------------

// undo
// ----
//! Undo the selection.

void
NsCmdSelectBody::undo()
{
    qDebug() << "NsCmdSelectBody::undo()";
    ArgsList argsList;
    foreach (const _Callback &cb, _callbacks) {
        argsList += Args(cb.bodyLongName(), cb.oldSelected());
    }
    _request(argsList);

    //_request(_bodyLongName, !_selected);   // Don't create instance.
}


// delayedRedo
// -----------
//! Do the selection.

void
NsCmdSelectBody::delayedRedo() const
{
    qDebug() << "NsCmdSelectBody::delayedRedo()";
    ArgsList argsList;
    foreach (const _Callback &cb, _callbacks) {
        argsList += Args(cb.bodyLongName(), cb.newSelected());
    }
    _request(argsList);
    //_request(_bodyLongName, _selected);   // Don't create instance.
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Do the actual work. [static]

NsCmdSelectBody::_CallbackList
NsCmdSelectBody::_request(const ArgsList &argsList)
{
    _CallbackList successCallbacks;
    QStringList bodyLongNames;
    foreach (const Args &args, argsList) {
        NsBodyObject *body = 
            NsOpStore::instance()->queryMutableBody(args.bodyLongName());

        if (0 != body ) {
            const bool oldSelected = body->isSelected();
            if (oldSelected != args.selected()) {
                body->nbBody().setMetaValue(
                    fromQStr(NsBodyObject::selectionMetaValueType()),
                    fromQStr(NsBodyObject::selectedMetaValue(args.selected())));

                successCallbacks +=
                    _Callback(args.bodyLongName(), oldSelected, args.selected());
                bodyLongNames += args.bodyLongName();
            }
        }
    }

    if (!bodyLongNames.isEmpty()) {
        NsCmdCentral::instance()->onBodySelectionChanged(bodyLongNames,true);
    }

    return successCallbacks;
}


// _store
// ------
//! DOCS [static]

void
NsCmdSelectBody::_store(const _CallbackList &callbacks,
                        NsUndoStack         &stack,
                        const bool           merge,
                        const QString       &text)
{
    if (!callbacks.empty()) {
        stack.push(new NsCmdSelectBody(callbacks, merge, text)); //Stack owns. 
    }
}


// _store
// ------
//! DOCS [static]

void
NsCmdSelectBody::_store(const _CallbackList &callbacks,
                        QUndoCommand        *parent)
{
    if (0 != parent && !callbacks.empty()) {
        NsCmdSelectBody *child(0);
        child = new NsCmdSelectBody(callbacks, parent); // Child.
    }
}
