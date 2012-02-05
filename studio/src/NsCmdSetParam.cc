// -----------------------------------------------------------------------------
//
// NsCmdSetParam.cc
//
// Naiad Studio command class for setting Op parameters source file.
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

#include "NsCmdSetParam.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsQuery.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include <QDebug>

// -----------------------------------------------------------------------------

NsCmdSetParam::Args::Args(const QString &opInstance,
                          const QString &paramName,
                          const QString &value,
                          const int      comp)
    : _paramLongName(queryParamLongName(opInstance, paramName))
    , _value(value)
    , _comp(comp)
{}

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once. [static]

void
NsCmdSetParam::exec(const ArgsList &argsList)
{ _request(argsList); }


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdSetParam::exec(const ArgsList &argsList,
                    QUndoCommand   &parent)
{ _store(_request(argsList), &parent); }


// exec
// ----
//! Execute and place on provided undo stack. [static]

void
NsCmdSetParam::exec(const ArgsList &argsList,
                    NsUndoStack    &stack,
                    const bool      merge)
{ _store(_request(argsList), stack, merge, QObject::tr("Set parameter")); }

// -----------------------------------------------------------------------------

// undo
// ----
//! This method is called to undo the command.

void
NsCmdSetParam::undo()
{
    qDebug() << "NsCmdSetParam::undo()";
    ArgsList argsList;
    foreach (const _CallbackList::value_type &cb, _callbacks) {
        argsList += Args(fromNbStr(cb.paramLongName),
                         fromNbStr(cb.oldValue),
                         cb.component);
    }

    _request(argsList);
}


// delayedRedo
// -----------
//! This method is called by an undostack in order to redo the command.

void
NsCmdSetParam::delayedRedo() const
{
    qDebug() << "NsCmdSetParam::delayedRedo()";
    ArgsList argsList;
    foreach (const _CallbackList::value_type &cb, _callbacks) {
        argsList += Args(fromNbStr(cb.paramLongName),
                         fromNbStr(cb.newValue),
                         cb.component);
    }

    _request(argsList);
}


// mergeWith
// ---------
//! DOCS

bool
NsCmdSetParam::mergeWith(const QUndoCommand *command)
{
    // Check if argument has same type as this object using RTTI.

    const NsCmdSetParam *other(dynamic_cast<const NsCmdSetParam*>(command));

    // Make sure the two commands refer to the same parameter.

    if (0 != other && other->_merge) {
        QList<int> merges;

        for (int i(0); i < _callbacks.size(); ++i) {
            const _CallbackList::value_type &cb0(_callbacks[i]);
            const _CallbackList::value_type &cb1(other->_callbacks[i]);

            if (cb0.paramLongName == cb1.paramLongName &&
                cb0.component     == cb1.component) {
                merges.append(i);
            }
            else {
                return false;   // Early exit!
            }
        }

        // If we got this fair everything is ok. Let the merge take place.

        foreach (int i, merges) {
            _callbacks[i].newValue = other->_callbacks[i].newValue;
        }

        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

// success
// -------
//! Called if a parameter is successfully changed in NI.

void
NsCmdSetParam::_Callback::success()
{
    qDebug() << "NsCmdSetParam::_Callback::success()";
    _success = true;
    _notify(_success);
}


// failure
// -------
//! Must never throw.

void
NsCmdSetParam::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSetParam::_Callback::failure()";
    _success = false;
    _notify(_success);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Emit signals through command central.

void
NsCmdSetParam::_Callback::_notify(const bool success) const
{
    NsCmdCentral::instance()->onValueChanged(
        fromNbStr(this->paramLongName),
        fromNbStr(this->newValue),
        this->component,
        success);
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Send request to NI. [static]

NsCmdSetParam::_CallbackList
NsCmdSetParam::_request(const ArgsList &argsList)
{
    _CallbackList successCallbacks;

    foreach (const Args &args, argsList) {
        _Callback cb; // Create a callback object to provide to NI.
        NiPushRequestCallback(&cb);
        NiSetParam(fromQStr(args.paramLongName()),
                   fromQStr(args.value()),
                   args.comp());

        if (cb.querySuccess()) {
            successCallbacks.append(cb); // Store successful callbacks.
        }
    }

    return successCallbacks;
}


// _store
// ------
//! DOCS [static]

void
NsCmdSetParam::_store(const _CallbackList &callbacks,
                      NsUndoStack         &stack,
                      const bool           merge,
                      const QString       &text)
{
    if (!callbacks.empty()) {
        stack.push(new NsCmdSetParam(callbacks, merge, text)); // Stack owns.
    }
}


// _store
// ------
//! DOCS [static]

void
NsCmdSetParam::_store(const _CallbackList &callbacks,
                      QUndoCommand        *parent)
{
    if (0 != parent && !callbacks.empty()) {
        NsCmdSetParam *child(0);
        child = new NsCmdSetParam(callbacks, parent); // Child.
    }
}
