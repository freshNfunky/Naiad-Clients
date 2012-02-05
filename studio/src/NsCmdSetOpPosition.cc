// -----------------------------------------------------------------------------
//
// NsCmdSetOpPosition.cc
//
// Naiad Studio command class for setting Op position, source file.
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

#include "NsCmdSetOpPosition.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsValueObject.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once. [static]

void
NsCmdSetOpPosition::exec(const ArgsList &argsList)
{ _request(argsList); }


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdSetOpPosition::exec(const ArgsList &argsList,
                         QUndoCommand   &parent)
{ _store(_request(argsList), &parent); }


// exec
// ----
//! Execute and place on provided undo stack. [static]

void
NsCmdSetOpPosition::exec(const ArgsList &argsList,
                         NsUndoStack    &stack,
                         const bool      merge)
{ _store(_request(argsList), stack, merge, QObject::tr("Move")); }

// -----------------------------------------------------------------------------

// undo
// ----
//! DOCS

void
NsCmdSetOpPosition::undo()
{
    qDebug() << "NsCmdSetOpPosition::undo()";

    qreal x(0.);
    qreal y(0.);
    ArgsList argsList;
    foreach (const _CallbackList::value_type &cb, _callbacks) {
        const QString val(fromNbStr(cb.oldValue));
        if (NsValueObject::positionCoordinates(val, x, y)) {
            argsList += Args(fromNbStr(cb.longName), x, y);
        }
    }

    _request(argsList);
}


// delayedRedo
// -----------
//! DOCS

void
NsCmdSetOpPosition::delayedRedo() const
{
    qDebug() << "NsCmdSetOpPosition::delayedRedo()";

    qreal x(0.);
    qreal y(0.);
    ArgsList argsList;
    foreach (const _CallbackList::value_type &cb, _callbacks) {
        const QString val(fromNbStr(cb.newValue));
        if (NsValueObject::positionCoordinates(val, x, y)) {
            argsList += Args(fromNbStr(cb.longName), x, y);
        }
    }

    _request(argsList);
}


// mergeWith
// ---------
//! DOCS

bool
NsCmdSetOpPosition::mergeWith(const QUndoCommand *command)
{
    // Check if argument has same type as this object using RTTI.

    const NsCmdSetOpPosition *other(
        dynamic_cast<const NsCmdSetOpPosition*>(command));

    // Make sure the two commands refer to the same parameter.

    if (0 != other && other->_merge) {
        QList<int> merges;

        for (int i(0); i < _callbacks.size(); ++i) {
            const _CallbackList::value_type &cb0(_callbacks[i]);
            const _CallbackList::value_type &cb1(other->_callbacks[i]);

            if (cb0.longName == cb1.longName) {
                merges.append(i);
            }
            else {
                return false;   // Early exit!
            }
        }

        // If we got this far everything is ok. Let the merge take place.

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
//! DOCS

void
NsCmdSetOpPosition::_Callback::success()
{
    qDebug() << "NsCmdSetOpPosition::_Callback::success()";
    _success = true;
    _notify(_success);
}


// failure
// -------
//! Must never throw.

void
NsCmdSetOpPosition::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSetOpPosition::_Callback::failure()";
    _success = false;
    _notify(_success);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Notify undo stack.

void
NsCmdSetOpPosition::_Callback::_notify(const bool success) const
{
    qreal x(0.);
    qreal y(0.);
    const QString val(fromNbStr(this->newValue));

    if (NsValueObject::positionCoordinates(val, x, y)) {
        NsCmdCentral::instance()->onOpPositionChanged(
                fromNbStr(this->longName), // Op instance.
                x,
                y,
                success);
    }
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Do the actual work. [static]

NsCmdSetOpPosition::_CallbackList
NsCmdSetOpPosition::_request(const ArgsList &argsList)
{
    _CallbackList successCallbacks;

    foreach (const Args &args, argsList) {
        _Callback cb; // Create a callback object to provide to NI.
        NiPushRequestCallback(&cb);
        NiSetMeta(fromQStr(args.opInstance()),
                  fromQStr(NsValueObject::positionMetaValueType()),
                  fromQStr(NsValueObject::positionString(args.x(), args.y())));

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
NsCmdSetOpPosition::_store(const _CallbackList &callbacks,
                      NsUndoStack         &stack,
                      const bool           merge,
                      const QString       &text)
{
    if (!callbacks.empty()) {
        stack.push(new NsCmdSetOpPosition(callbacks, merge, text));//Stack owns.
    }
}


// _store
// ------
//! DOCS [static]

void
NsCmdSetOpPosition::_store(const _CallbackList &callbacks,
                           QUndoCommand        *parent)
{
    if (0 != parent && !callbacks.empty()) {
        NsCmdSetOpPosition *child(0);
        child = new NsCmdSetOpPosition(callbacks, parent); // Child.
    }
}
