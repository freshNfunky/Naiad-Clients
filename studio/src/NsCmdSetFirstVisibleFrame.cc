// -----------------------------------------------------------------------------
//
// NsCmdSetFirstVisibleFrame.cc
//
// Naiad Studio command class for changing first visible frame, source file.
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

#include "NsCmdSetFirstVisibleFrame.h"
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
NsCmdSetFirstVisibleFrame::exec(const int fvf)
{
    _request(fvf, 0);
}


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdSetFirstVisibleFrame::exec(const int     fvf,
                                QUndoCommand &parent)
{
    _request(fvf, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack. [static]

void
NsCmdSetFirstVisibleFrame::exec(const int       fvf,
                                NsUndoStack &stack,
                                const bool      merge)
{
    _request(fvf,
             stack,
             merge,
             QObject::tr("Set first visible frame: ") + QString::number(fvf));
}

// -----------------------------------------------------------------------------

// undo
// ----
//! DOCS

void
NsCmdSetFirstVisibleFrame::undo()
{
    qDebug() << "NsCmdSetFirstVisibleFrame::undo()";

    bool ok(false);
    const int fvf(fromNbStr(_cb.oldValue).toInt(&ok));

    if (ok) {
        _request(fvf);
    }
}


// delayedRedo
// -----------
//! DOCS

void
NsCmdSetFirstVisibleFrame::delayedRedo() const
{
    qDebug() << "NsCmdSetFirstVisibleFrame::delayedRedo()";

    bool ok(false);
    const int fvf(fromNbStr(_cb.newValue).toInt(&ok));

    if (ok) {
        _request(fvf);
    }
}


// mergeWith
// ---------
//! DOCS

bool
NsCmdSetFirstVisibleFrame::mergeWith(const QUndoCommand *command)
{
    const NsCmdSetFirstVisibleFrame *other(
        dynamic_cast<const NsCmdSetFirstVisibleFrame *>(command));

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
NsCmdSetFirstVisibleFrame::_Callback::success()
{
    qDebug() << "NsCmdSetFirstVisibleFrame::_Callback::success()";

    if (0 != _stack || 0 != _parent) {
        // Create a command instance.

        const NtSetMetaCallback *base(
            static_cast<const NtSetMetaCallback *>(this));

        if (0 != _parent && 0 == _stack) {
            NsCmdSetFirstVisibleFrame *child(0);
            child = new NsCmdSetFirstVisibleFrame(*base, _parent); // Child.
        }
        else if (0 != _stack && 0 == _parent) {
            // Stack owns memory.

            _stack->push(new NsCmdSetFirstVisibleFrame(*base, _merge, _text));
        }
    }

    _notify(true);
}


// failure
// -------
//! Must never throw.

void
NsCmdSetFirstVisibleFrame::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSetFirstVisibleFrame::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Notify undo stack.

void
NsCmdSetFirstVisibleFrame::_Callback::_notify(const bool success) const
{
    bool ok(false);
    const int fvf(fromNbStr(this->newValue).toInt(&ok));
    
    EM_ASSERT(ok);
    if (ok) {
        NsCmdCentral::instance()->onFirstVisibleFrameChanged(fvf, success);
    }
}

// -----------------------------------------------------------------------------

// _request
// --------
//! DOCS [static]

void
NsCmdSetFirstVisibleFrame::_request(const int       fvf,
                                    NsUndoStack &stack,
                                    const bool      merge,
                                    const QString  &text)
{
    // Create a callback object and provide it to NI.

    _Callback cb(stack, merge, text);
    NiPushRequestCallback(&cb);
    NiSetMeta("Global",
              fromQStr(NsTimeToolBar::firstVisibleFrameMetaValueType()),
              fromQStr(QString::number(fvf)));
}


// _request
// --------
//! DOCS [static]

void
NsCmdSetFirstVisibleFrame::_request(const int     fvf,
                                    QUndoCommand *parent)
{
    // Create a callback object and provide it to NI.

    _Callback cb(parent);
    NiPushRequestCallback(&cb);
    NiSetMeta("Global",
              fromQStr(NsTimeToolBar::firstVisibleFrameMetaValueType()),
              fromQStr(QString::number(fvf)));
}

// -----------------------------------------------------------------------------
