// -----------------------------------------------------------------------------
//
// NsMessageCallback.cc
//
// Naiad Studio message callback source file.
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

#include "NsMessageCallback.h"
#include "NsOpStore.h"
#include "NsOpObject.h"
#include "NsStringUtils.h"
#include <Ni.h>

// -----------------------------------------------------------------------------

// NsMessageCallback
// -----------------
//! CTOR.

NsMessageCallback::NsMessageCallback()
    : NtMessageCallback()
{
    NiRegisterMessageCallback(this);    // Registers itself with NI.
}


// ~NsMessageCallback
// ------------------
//! DTOR.

NsMessageCallback::~NsMessageCallback()
{}

// -----------------------------------------------------------------------------

// info
// ----
//! Called to display a general information message from Naiad

void
NsMessageCallback::info(const NtString &text)
{
    Message msg;
    _createMessage(QObject::tr(text.c_str()),
                   valueObject(),
                   msg);
    _infoQueue.enqueue(msg);
    _notifier.emitInfo();
}

// warning
// -------
//! Called to display a warning from Naiad

void
NsMessageCallback::warning(const NtString &text)
{
    Message msg;
    _createMessage(QObject::tr(text.c_str()),
                   valueObject(),
                   msg);
    _warningQueue.enqueue(msg);
    _notifier.emitWarning();
}

// error
// -----
//! Called to display an error from Naiad

void
NsMessageCallback::error(const NtString &text)
{
    Message msg;
    _createMessage(QObject::tr(text.c_str()),
                   valueObject(),
                   msg);
    _errorQueue.enqueue(msg);
    _notifier.emitError();
}

// -----------------------------------------------------------------------------

// pollInfo
// --------
//! Returns true if the queue is non-empty. Sets the provided string to
//! the front of the queue.

bool
NsMessageCallback::pollInfo(Message &msg)
{
    if (!_infoQueue.isEmpty()) {
        msg = _infoQueue.dequeue();
        return true;
    }
    return false;
}


// pollWarning
// -----------
//! Returns true if the queue is non-empty. Sets the provided string to
//! the front of the queue.

bool
NsMessageCallback::pollWarning(Message &msg)
{
    if (!_warningQueue.isEmpty()) {
        msg = _warningQueue.dequeue();
        return true;
    }
    return false;
}


// pollError
// ---------
//! Returns true if the queue is non-empty. Sets the provided string to
//! the front of the queue.

bool
NsMessageCallback::pollError(Message &msg)
{
    if (!_errorQueue.isEmpty()) {
        msg = _errorQueue.dequeue();
        return true;
    }
    return false;
}

// -----------------------------------------------------------------------------

void
NsMessageCallback::_createMessage(const QString         &text,
                                  const Nb::ValueObject *vob,
                                  Message               &msg)
{
    msg.text = text;
    if (vob) {
        msg.vobName = fromNbStr(vob->name());
    }
}

// -----------------------------------------------------------------------------
