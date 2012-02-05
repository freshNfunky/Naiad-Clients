// -----------------------------------------------------------------------------
//
// NsCmdUnfeedAndErase.cc
//
// Naiad Studio command class for erasing an Op, header file.
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

#include "NsCmdUnfeedAndErase.h"
#include "NsCmdFeed.h"
#include "NsCmdErase.h"
#include "NsUndoStack.h"
#include "NsQuery.h"
#include <QDebug>

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once.

void
NsCmdUnfeedAndErase::exec(const QString &opInstance)
{
    _request(opInstance);
}


// exec
// ----
//! Execute as child of a parent command.

void
NsCmdUnfeedAndErase::exec(const QString &opInstance,
                          QUndoCommand  &parent)
{
    _request(opInstance, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack.

void
NsCmdUnfeedAndErase::exec(const QString  &opInstance,
                          NsUndoStack &stack,
                          const bool      merge)
{
    _request(opInstance, stack, merge);
}

// -----------------------------------------------------------------------------

// _request
// --------
//! DOCS

void
NsCmdUnfeedAndErase::_request(const QString &opInstance,
                              QUndoCommand  *parent)
{
    QSet<QString> feeds;
    queryFeeds(opInstance, feeds);

    qDebug() << "Feeds: " << feeds;

    if (0 != parent) {
        foreach (const QString &feed, feeds) {
            NsCmdFeed::exec(feed, "", *parent);
        }

        NsCmdErase::exec(opInstance, *parent);
    }
    else {
        foreach (const QString &feed, feeds) {
            NsCmdFeed::exec(feed, "");
        }

        NsCmdErase::exec(opInstance);
    }
}


// _request
// --------
//! DOCS

void
NsCmdUnfeedAndErase::_request(const QString  &opInstance,
                              NsUndoStack &stack,
                              const bool      merge)
{
    NsCmdUnfeedAndErase *cmd(
        new NsCmdUnfeedAndErase(
                merge,
                QObject::tr("Unfeed and erase")));

    _request(opInstance, cmd);

    stack.push(cmd);    // Stack owns memory.
}
