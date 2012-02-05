// -----------------------------------------------------------------------------
//
// NsCmdClearGraph.cc
//
// Naiad Studio command class for clearing the graph source file.
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

#include "NsCmdClearGraph.h"
#include "NsCmdCentral.h"
#include "NsStringUtils.h"
#include "NsMessageWidget.h"
#include <QtDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Executes the request.

void
NsCmdClearGraph::exec()
{
    qDebug() << "NsCmdClearGraph::exec";
    _request();
}

// -----------------------------------------------------------------------------

// success
// -------
//! Called if a parameter is successfully changed in NI.

void
NsCmdClearGraph::_Callback::success()
{
    qDebug() << "NsCmdClearGraph::_Callback::success()";
    _notify(true);
}


// failure
// -------
//! Must never throw.

void
NsCmdClearGraph::_Callback::failure(const NtString &msg)
{
    qDebug() << "NsCmdClearGraph::_Callback::failure()";
    _notify(false);
    NsMessageWidget::instance()->serverError(fromNbStr(msg));
}


// _notify
// -------
//! Emit signal through command central.

void
NsCmdClearGraph::_Callback::_notify(const bool success) const
{
    NsCmdCentral::instance()->onGraphCleared(success);
}

// -----------------------------------------------------------------------------

// _request
// --------
//! Send request to NI.

void
NsCmdClearGraph::_request()
{
    qDebug() << "NsCmdClearGraph::request";

    // Create a callback object and provide it to NI.

    NsCmdClearGraph::_Callback cb;
    NiPushRequestCallback(&cb);
    NiClearGraph();
}
