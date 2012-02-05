// -----------------------------------------------------------------------------
//
// NsCmdSetCurrentVisibleFrame.cc
//
// Naiad Studio command class for changing visible frame, source file.
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

#include "NsCmdSetCurrentVisibleFrame.h"
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
NsCmdSetCurrentVisibleFrame::exec(const int cvf, const bool update3DView)
{
    _request(cvf, update3DView, 0);
}


// exec
// ----
//! Execute as child of a parent command. [static]

void
NsCmdSetCurrentVisibleFrame::exec(const int     cvf,
                                  QUndoCommand &parent,
                                  const bool    update3DView)
{
    _request(cvf, update3DView, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack. [static]

void
NsCmdSetCurrentVisibleFrame::exec(const int    cvf,
                                  NsUndoStack &stack,
                                  const bool   merge,
                                  const bool   update3DView)
{
    _request(cvf,
             update3DView,
             stack,
             merge,
             QObject::tr("Set current visible frame"));
}

// -----------------------------------------------------------------------------

// undo
// ----
//! DOCS

void
NsCmdSetCurrentVisibleFrame::undo()
{
    qDebug() << "NsCmdSetCurrentVisibleFrame::undo()";

    bool ok(false);
    const int oldCvf(fromNbStr(_cb.oldValue).toInt(&ok));

    if (ok) {
        _request(oldCvf, _update3DView);
    }
}


// delayedRedo
// -----------
//! DOCS

void
NsCmdSetCurrentVisibleFrame::delayedRedo() const
{
    qDebug() << "NsCmdSetCurrentVisibleFrame::delayedRedo()";

    bool ok(false);
    const int newCvf(fromNbStr(_cb.newValue).toInt(&ok));

    if (ok) {
        _request(newCvf, _update3DView);
    }
}


// mergeWith
// ---------
//! DOCS

bool
NsCmdSetCurrentVisibleFrame::mergeWith(const QUndoCommand *command)
{
    const NsCmdSetCurrentVisibleFrame *other(
        dynamic_cast<const NsCmdSetCurrentVisibleFrame *>(command));

    if (0 != other && other->_merge && _update3DView == other->_update3DView) {
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
NsCmdSetCurrentVisibleFrame::_Callback::success()
{
    qDebug() << "NsCmdSetCurrentVisibleFrame::_Callback::success()";

    if (0 != _stack || 0 != _parent) {
        // Create a command instance.

        const NtSetMetaCallback *base(
            static_cast<const NtSetMetaCallback *>(this));

        if (0 != _parent && 0 == _stack) {
            NsCmdSetCurrentVisibleFrame *child(0);
            child = 
                new NsCmdSetCurrentVisibleFrame(*base, _parent, _update3DView);
        }
        else if (0 != _stack && 0 == _parent) {
            // Stack owns memory.

            _stack->push(
                new NsCmdSetCurrentVisibleFrame(*base, _merge, _text, 
                                                _update3DView));
        }
    }

    _notify(true);
}


// failure
// -------
//! Must never throw.

void
NsCmdSetCurrentVisibleFrame::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdSetCurrentVisibleFrame::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Notify undo stack.

void
NsCmdSetCurrentVisibleFrame::_Callback::_notify(const bool success) const
{
    bool ok(false);
    const int cvf(fromNbStr(this->newValue).toInt(&ok));

    EM_ASSERT(ok);
    if (ok) {
        NsCmdCentral::instance()->onCurrentVisibleFrameChanged(
            cvf, _update3DView, success);
    }
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Do the actual work. [static]

void
NsCmdSetCurrentVisibleFrame::_request(const int      cvf,
                                      const bool     update3DView,
                                      NsUndoStack   &stack,
                                      const bool     merge,
                                      const QString &text)
{
    // Create a callback object and provide it to NI.

    _Callback cb(stack, merge, text, update3DView);
    NiPushRequestCallback(&cb);
    NiSetMeta("Global",
              fromQStr(NsTimeToolBar::currentVisibleFrameMetaValueType()),
              fromQStr(QString::number(cvf)));
}


// _request
// --------
//! Do the actual work. [static]

void
NsCmdSetCurrentVisibleFrame::_request(const int     cvf,
                                      const bool    update3DView,
                                      QUndoCommand *parent)
{
    // Create a callback object and provide it to NI.

    _Callback cb(parent, update3DView);
    NiPushRequestCallback(&cb);
    NiSetMeta("Global",
              fromQStr(NsTimeToolBar::currentVisibleFrameMetaValueType()),
              fromQStr(QString::number(cvf)));
}

// -----------------------------------------------------------------------------
