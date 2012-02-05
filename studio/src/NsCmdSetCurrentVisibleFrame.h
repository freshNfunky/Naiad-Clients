// -----------------------------------------------------------------------------
//
// NsCmdSetCurrentVisibleFrame.h
//
// Naiad Studio command class for setting current visible frame, header file.
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

#ifndef NS_CMD_SET_CURRENT_VISIBLE_FRAME_H
#define NS_CMD_SET_CURRENT_VISIBLE_FRAME_H

#include "NsCmdBase.h"
#include <Ni.h>

class NsUndoStack;

// -----------------------------------------------------------------------------

// NsCmdSetCurrentVisibleFrame
// ---------------------------
//! Command class setting the current visible frame.

class NsCmdSetCurrentVisibleFrame : public NsCmdBase
{
public:     // Static interface.

    static void
    exec(int cvf, bool update3DView = true);

    static void
    exec(int           cvf,
         QUndoCommand &parent,
         bool          update3DView = true);

    static void
    exec(int          cvf,
         NsUndoStack &stack,
         bool         merge        = true,
         bool         update3DView = true);

public:     // NsCmdBase interface.

    //! DTOR.
    virtual 
    ~NsCmdSetCurrentVisibleFrame() 
    {} 

    virtual void
    undo();

    virtual void
    delayedRedo() const;

    virtual bool
    mergeWith(const QUndoCommand *command);

private:

    // _Callback
    // --------
    //! Class for handling response from NiSetMeta().

    class _Callback : public NtSetMetaCallback
    {
    public:

        //! CTOR.
        explicit
        _Callback(NsUndoStack   &stack,
                  const bool     merge,
                  const QString &text,
                  const bool     update3DView)
            : NtSetMetaCallback()
            , _update3DView(update3DView)
            , _stack(&stack)
            , _merge(merge)
            , _text(text)
            , _parent(0)
        {}

        //! CTOR.
        explicit
        _Callback(QUndoCommand *parent,
                  const bool    update3DView)
            : NtSetMetaCallback()
            , _stack(0)
            , _merge(false)
            , _text("")
            , _parent(parent)
        {}

        //! DTOR.
        virtual 
        ~_Callback() 
        {}     

        virtual void
        success();

        virtual void
        failure(const NtString &msg);

    private:

        void
        _notify(bool success) const;

    private:    // Member variables.
        
        bool           _update3DView;
        NsUndoStack   *_stack;
        const bool     _merge;
        const QString  _text;
        QUndoCommand  *_parent;
    };

private:    // Static implementation.

    static void
    _request(int            cvf,
             bool           update3DView,
             NsUndoStack   &stack,
             bool           merge,
             const QString &text);

    static void
    _request(int           cvf,
             bool          update3DView,
             QUndoCommand *parent = 0);

private:

    //! CTOR.
    explicit
    NsCmdSetCurrentVisibleFrame(const NtSetMetaCallback &cb,
                                const bool               merge,
                                const QString           &text,
                                bool                     update3DView)
        : NsCmdBase(text, 0)
        , _cb(cb)
        , _update3DView(update3DView)
        , _merge(merge)
    {}

    //! CTOR.
    explicit
    NsCmdSetCurrentVisibleFrame(const NtSetMetaCallback &cb,
                                QUndoCommand            *parent,
                                bool                     update3DView)
        : NsCmdBase("", parent)
        , _cb(cb)
        , _update3DView(update3DView)
        , _merge(false)
    {}

private:    // Member variables.

    NtSetMetaCallback _cb;      //!< Information about the request.
    bool              _update3DView;
    bool              _merge;   //!< Whether command may be merged.
};

// -----------------------------------------------------------------------------

#endif // NS_CMD_SET_CURRENT_VISIBLE_FRAME_H
