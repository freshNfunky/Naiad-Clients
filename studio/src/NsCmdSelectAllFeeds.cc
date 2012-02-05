// -----------------------------------------------------------------------------
//
// NsCmdSelectAllFeeds.cc
//
// Naiad Studio command class for selecting all feeds, source file.
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

#include "NsCmdSelectAllFeeds.h"
#include "NsCmdSelectFeed.h"
#include "NsUndoStack.h"
#include "NsOpStore.h"
#include "NsInputPlugObject.h"

// -----------------------------------------------------------------------------

// exec
// ----
//! Execute once.

void
NsCmdSelectAllFeeds::exec(const bool selected)
{
    _request(selected, 0);
}


// exec
// ----
//! Execute as child of a parent command.

void
NsCmdSelectAllFeeds::exec(const bool    selected,
                          QUndoCommand &parent)
{
    _request(selected, &parent);
}


// exec
// ----
//! Execute and place on provided undo stack.

void
NsCmdSelectAllFeeds::exec(const bool   selected,
                          NsUndoStack &stack,
                          const bool   merge)
{
    NsCmdSelectAllFeeds *cmd(
        new NsCmdSelectAllFeeds(
                merge,
                (selected ? "Select" : "Unselect") + QString(" all Feeds")));

    exec(selected, *cmd);

    stack.push(cmd);    // Stack owns memory.
}

// -----------------------------------------------------------------------------

void
NsCmdSelectAllFeeds::_request(const bool      selected,
                              QUndoCommand   *parent)
{
    if (!NsOpStore::instance()->constOps().isEmpty()) {

        NsCmdSelectFeed::ArgsList argsList;
        foreach(const NsInputPlugObject *ipo, 
                NsOpStore::instance()->constInputs()) {
            if (ipo->isSelected() != selected) {
                argsList += NsCmdSelectFeed::Args(ipo->longName(), selected);
            }
        }

        if (0 != parent) {
            NsCmdSelectFeed::exec(argsList, *parent);
        }
        else {
            NsCmdSelectFeed::exec(argsList);
        }
    }
}
