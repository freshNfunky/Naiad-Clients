// -----------------------------------------------------------------------------
//
// NsCmdSelectBody.h
//
// Naiad Studio command class for selecting a body, header file.
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

#ifndef NS_CMD_SELECT_BODY_H
#define NS_CMD_SELECT_BODY_H

#include "NsCmdBase.h"
#include <Ni.h>

class NsUndoStack;

// -----------------------------------------------------------------------------

// NsCmdSelectBody
// ---------------
//! Command class for selecting a body. This command sends no request to Ni,
//! it is purely an Open Naiad Studio matter.

class NsCmdSelectBody : public NsCmdBase
{
public:

    class Args
    {
    public:

        //! CTOR.
        explicit
        Args(const QString &bodyLongName, const bool selected)
             : _bodyLongName(bodyLongName)
             , _selected(selected)
        {}

        const QString&
        bodyLongName() const
        { return _bodyLongName; }

        bool
        selected() const
        { return _selected; }

    private:    // Member variables.

        QString _bodyLongName;
        bool    _selected;
    };

    typedef QList<Args> ArgsList;

public:     // Static interface.

    //! Execute once.
    static void
    exec(const ArgsList &args)
    { _request(args); }

    //! Execute as child of a parent command.
    static void
    exec(const ArgsList &args,
         QUndoCommand   &parent)
    { _store(_request(args), &parent); }

    //! Execute and place on provided undo stack.
    static void
    exec(const ArgsList &args, 
         NsUndoStack    &stack, 
         const bool      merge = false)
    { _store(_request(args), stack, merge, QObject::tr("Select")); }

public:     // NsCmdBase interface.

    //! DTOR.
    virtual 
    ~NsCmdSelectBody() 
    {}   

    virtual void
    undo();

    virtual void
    delayedRedo() const;

    virtual bool
    mergeWith(const QUndoCommand *command)
    { return false; }   // TODO: Implement!

private:

    // _Callback
    // --------
    //! DOCS.

    class _Callback
    {
    public:

        //! CTOR.
        _Callback(const QString &bodyLongName,
                  const bool oldSelected,
                  const bool newSelected)
            : _bodyLongName(bodyLongName)
            , _oldSelected(oldSelected)
            , _newSelected(newSelected)
        {}

        const QString&
        bodyLongName() const
        { return _bodyLongName; }

        bool
        oldSelected() const
        { return _oldSelected; }

        bool
        newSelected() const
        { return _newSelected; }

    private:    // Member variables.

        QString _bodyLongName;
        bool    _oldSelected;
        bool    _newSelected;
    };

    typedef QList<_Callback> _CallbackList;

private:    // Static implementation.

    static _CallbackList
    _request(const ArgsList &argsList);

    static void
    _store(const _CallbackList &callbacks,
           NsUndoStack         &stack,
           bool                 merge,
           const QString       &text);

    static void
    _store(const _CallbackList &callbacks,
           QUndoCommand        *parent = 0);


    /*
    static void
    _request(const QString  &bodyLongName,
             bool            selected,
             NsUndoStack &stack,
             bool            merge,
             const QString  &text);

    static void
    _request(const QString  &bodyLongName,
             bool            selected,
             QUndoCommand   *parent = 0);

    static void
    _notify(const QString &bodyLongName,
            bool           selected,
            bool           success);
            */
private:

    //! CTOR.
    explicit
    NsCmdSelectBody(const _CallbackList &callbacks,
                    const bool           merge,
                    const QString       &text)
        : NsCmdBase(text)
        , _callbacks(callbacks)
        , _merge(merge)
    {}

    //! CTOR.
    explicit
    NsCmdSelectBody(const _CallbackList &callbacks,
                    QUndoCommand        *parent)
        : NsCmdBase("", parent)
        , _callbacks(callbacks)
        , _merge(false)
    {}

private:    // Member variables.

    _CallbackList _callbacks;
    bool          _merge;         //!< Whether command may be merged.
};

#endif // NS_CMD_SELECT_BODY_H
