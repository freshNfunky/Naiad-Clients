// -----------------------------------------------------------------------------
//
// NsCmdSetLastVisibleFrame.cc
//
// Naiad Studio command class for changing last visible frame, source file.
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

#include "NsCmdSetLastVisibleFrame.h"
#include "NsCmdCentral.h"
#include "NsUndoStack.h"
#include "NsTimeToolBar.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once. [static]

void
NsCmdSetLastVisibleFrame::exec(const int lvf)
{
    _request(lvf, 0);
}


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdSetLastVisibleFrame::exec(const int     lvf,
                               QUndoCommand &parent)
{
    _request(lvf, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack. [static]

void
NsCmdSetLastVisibleFrame::exec(const int       lvf,
                               NsUndoStack &stack,
                               const bool      merge)
{
    _request(lvf,
             stack,
             merge,
             QObject::tr("Set last visible frame: ") + QString::number(lvf));
}

// -----------------------------------------------------------------------------

// undo
// ----
//! DOCS

void
NsCmdSetLastVisibleFrame::undo()
{
    qDebug() << "NsCmdSetLastVisibleFrame::undo()";

    bool ok(false);
    const int oldLvf(fromNbStr(_cb.oldValue).toInt(&ok));

    if (ok) {
        _request(oldLvf);
    }
}


// delayedRedo
// -----------
//! DOCS

void
NsCmdSetLastVisibleFrame::delayedRedo() const
{
    qDebug() << "NsCmdSetLastVisibleFrame::delayedRedo()";

    bool ok(false);
    const int newLvf(fromNbStr(_cb.newValue).toInt(&ok));

    if (ok) {
        _request(newLvf);
    }
}


// mergeWith
// ---------
//! DOCS

bool
NsCmdSetLastVisibleFrame::mergeWith(const QUndoCommand *command)
{
    const NsCmdSetLastVisibleFrame *other(
        dynamic_cast<const NsCmdSetLastVisibleFrame *>(command));

    if (0 != other && other->_merge) {
        _cb.newValue = other->_cb.newValue;
        return true;
    }

    return false;
}

// -----------------------------------------------------------------------------

// success
// -------
//! ...

void
NsCmdSetLastVisibleFrame::_Callback::success()
{
    qDebug() << "NsCmdSetLastVisibleFrame::_Callback::success()";

    if (0 != _stack || 0 != _parent) {
        // Create a command instance.

        const NtSetMetaCallback *base(
            static_cast<const NtSetMetaCallback *>(this));

        if (0 != _parent && 0 == _stack) {
            NsCmdSetLastVisibleFrame *child(0);
            child = new NsCmdSetLastVisibleFrame(*base, _parent); // Child.
        }
        else if (0 != _stack && 0 == _parent) {
            // Stack owns memory.

            _stack->push(new NsCmdSetLastVisibleFrame(*base, _merge, _text));
        }
    }

    _notify(true);
}


// failure
// -------
//! Must never throw.

void
NsCmdSetLastVisibleFrame::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSetLastVisibleFrame::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Notify undo stack.

void
NsCmdSetLastVisibleFrame::_Callback::_notify(const bool success) const
{
    bool ok(false);
    const int lvf(fromNbStr(this->newValue).toInt(&ok));
    
    EM_ASSERT(ok);
    if (ok) {
        NsCmdCentral::instance()->onLastVisibleFrameChanged(lvf, success);
    }
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Do the actual work. [static]

void
NsCmdSetLastVisibleFrame::_request(const int       lvf,
                                   NsUndoStack &stack,
                                   const bool      merge,
                                   const QString  &text)
{
    // Create a callback object and provide it to NI.

    _Callback cb(stack, merge, text);
    NiPushRequestCallback(&cb);
    NiSetMeta("Global",
              fromQStr(NsTimeToolBar::lastVisibleFrameMetaValueType()),
              fromQStr(QString::number(lvf)));
}


// _request
// --------
//! Do the actual work. [static]

void
NsCmdSetLastVisibleFrame::_request(const int       lvf,
                                   QUndoCommand   *parent)
{
    // Create a callback object and provide it to NI.

    _Callback cb(parent);
    NiPushRequestCallback(&cb);
    NiSetMeta("Global",
              fromQStr(NsTimeToolBar::lastVisibleFrameMetaValueType()),
              fromQStr(QString::number(lvf)));
}

// -----------------------------------------------------------------------------
