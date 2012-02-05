// -----------------------------------------------------------------------------
//
// NsCmdCopy.h
//
// Naiad Studio command class for copying to clipboard, header file.
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

#ifndef NS_CMD_COPY_H
#define NS_CMD_COPY_H

#include "NsCmdBase.h"
#include <QMap>
#include <QString>
#include <QPointF>

class QStringList;

// -----------------------------------------------------------------------------

// NsCmdCopy
// ---------
//! Command class for copying.

class NsCmdCopy : public NsCmdBase
{
public:     // Static interface.

    static void
    exec(const QStringList &opInstances);

    static void
    exec(const QStringList &opInstances,
         QUndoCommand      &parent);

public:     // NsCmdBase interface.

    //! DTOR.
    virtual
    ~NsCmdCopy()
    {}

    //! Do nothing
    virtual void
    undo()
    {}

    //! Do nothing
    virtual void
    delayedRedo() const
    {}

    virtual bool
    mergeWith(const QUndoCommand *command)
    { return false; }

private:    // Static implementation.

    static void
    _request(const QStringList &opInstances,
             QUndoCommand      *parent = 0);

private:

    //! CTOR.
    explicit
    NsCmdCopy(QUndoCommand *parent)
        : NsCmdBase("", parent)
    {}
};

#endif // NS_CMD_COPY_H
