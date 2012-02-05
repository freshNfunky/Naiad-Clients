// -----------------------------------------------------------------------------
//
// NsCmdErase.h
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

#ifndef NS_CMD_ERASE_H
#define NS_CMD_ERASE_H

#include "NsCmdBase.h"
#include <Ni.h>

class NsUndoStack;

// -----------------------------------------------------------------------------

// NsCmdErase
// -----------
//! Command class for erasing an Op.

class NsCmdErase : public NsCmdBase
{
public:     // Static interface.

    static void
    exec(const QString &opInstance);

    static void
    exec(const QString &opInstance,
         QUndoCommand  &parent);

    static void
    exec(const QString  &opInstance,
         NsUndoStack &stack,
         bool            merge = false);

public:     // NsCmdBase interface.

    virtual ~NsCmdErase() {}    //!< DTOR.

    virtual void
    undo();

    virtual void
    delayedRedo() const;

    virtual bool
    mergeWith(const QUndoCommand *command)
    { return false; }   // TODO: Implement!

private:

    // _Callback
    // ---------
    //! Class for handling response from NiErase().

    class _Callback : public NtEraseCallback
    {
    public:

        //! CTOR.
        explicit
        _Callback(const QByteArray &niOp,
                  NsUndoStack   &stack,
                  bool              merge,
                  const QString    &text)
            : NtEraseCallback()
            , _niOp(niOp)
            , _stack(&stack)
            , _merge(merge)
            , _text(text)
            , _parent(0)
        {}

        //! CTOR.
        explicit
        _Callback(const QByteArray  &niOp,
                  QUndoCommand      *parent)
            : NtEraseCallback()
            , _niOp(niOp)
            , _stack(0)
            , _merge(false)
            , _text("")
            , _parent(parent)
        {}

        virtual ~_Callback() {}     //!< DTOR.

        virtual void
        success();

        virtual void
        failure(const NtString &msg);

        const QByteArray&
        niOp() const
        { return _niOp; }

    private:

        void
        _notify(bool success) const;

    private:    // Member variables.

        QByteArray      _niOp;
        NsUndoStack *_stack;
        const bool      _merge;
        const QString   _text;
        QUndoCommand   *_parent;
    };

private:    // Static implementation.

    static void
    _request(const QString  &opInstance,
             NsUndoStack &stack,
             bool            merge,
             const QString  &text);

    static void
    _request(const QString  &opInstance,
             QUndoCommand   *parent = 0);

private:

    //! CTOR.
    explicit
    NsCmdErase(const QByteArray      &niOp,
               const NtEraseCallback &cb,
               const bool             merge,
               const QString         &text)
        : NsCmdBase(text, 0)
        , _niOp(niOp)
        , _cb(cb)
        , _merge(merge)
    {}

    //! CTOR.
    explicit
    NsCmdErase(const QByteArray      &niOp,
               const NtEraseCallback &cb,
               QUndoCommand          *parent)
        : NsCmdBase("", parent)
        , _niOp(niOp)
        , _cb(cb)
        , _merge(false)
    {}

private:    // Member variables.

    //QStringList names;  //!< The names of the deleted Ops.
    //QByteArray niOp;  //!< The deleted Ops, in NI export format.

    QByteArray      _niOp;
    NtEraseCallback _cb;     //!< Information about the request.
    bool            _merge;  //!< Whether command may be merged.
};

#endif // NS_CMD_ERASE_H
