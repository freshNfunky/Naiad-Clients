// -----------------------------------------------------------------------------
//
// NsCmdSetParam3.h
//
// Naiad Studio command class for setting vector params, header file.
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

#ifndef NS_CMD_SET_PARAM3_H
#define NS_CMD_SET_PARAM3_H

#include "NsCmdBase.h"
#include <Ni.h>

class NsUndoStack;

// -----------------------------------------------------------------------------

// NsCmdSetParam3
// --------------
//! DOCS

class NsCmdSetParam3 : public NsCmdBase
{
public:

    class Args
    {
    public:

        //! CTOR.
        explicit
        Args(const QString &paramLongName,
             const QString &value0,
             const QString &value1,
             const QString &value2)
             : _paramLongName(paramLongName)
             , _value0(value0)
             , _value1(value1)
             , _value2(value2)
        {}

        //! CTOR.
        explicit
        Args(const QString &opInstance,
             const QString &paramName,
             const QString &value0,
             const QString &value1,
             const QString &value2);

        const QString&
        paramLongName() const
        { return _paramLongName; }

        const QString&
        value0() const
        { return _value0; }

        const QString&
        value1() const
        { return _value1; }

        const QString&
        value2() const
        { return _value2; }

    private:    // Member variables.

        QString _paramLongName;
        QString _value0;
        QString _value1;
        QString _value2;
    };

    typedef QList<Args> ArgsList;

public:     // Static interface.

    static void
    exec(const ArgsList &args);

    static void
    exec(const ArgsList &args,
         QUndoCommand   &parent);

    static void
    exec(const ArgsList &args,
         NsUndoStack    &stack,
         bool            merge  = false);

public:     // NsCmdBase interface.

    //! DTOR.
    virtual 
    ~NsCmdSetParam3() 
    {}    

    virtual void
    undo();

    virtual void
    delayedRedo() const;

    virtual bool
    mergeWith(const QUndoCommand *command);

private:

    // _Callback
    // ---------
    //! Class for handling response from NiSetParam3().

    class _Callback : public NtSetParam3Callback
    {
    public:

        //! CTOR.
        explicit
        _Callback()
            : NtSetParam3Callback()
            , _success(false)
        {}

        //! DTOR.
        virtual 
        ~_Callback() 
        {}

        virtual void
        success();

        virtual void
        failure(const NtString &msg);

        _Callback&
        operator=(const _Callback &rhs)
        {
            NtSetParam3Callback::operator=(rhs); // Base method.
            _success = rhs._success;
            return *this;
        }

        bool
        querySuccess() const
        { return _success; }

    private:

        void
        _notify(bool success) const;

    private:    // Member variables.

        bool _success;
    };

    typedef QList<_Callback> _CallbackList;

private:

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

private:

    //! CTOR.
    explicit
    NsCmdSetParam3(const _CallbackList &callbacks,
                   const bool           merge,
                   const QString       &text)
        : NsCmdBase(text, 0)
        , _callbacks(callbacks)
        , _merge(merge)
    {}

    //! CTOR.
    explicit
    NsCmdSetParam3(const _CallbackList &callbacks,
                   QUndoCommand        *parent)
        : NsCmdBase(QString(), parent)
        , _callbacks(callbacks)
        , _merge(false)
    {}

private:    // Member variables.

    _CallbackList _callbacks;   //!< Information about the request.
    bool          _merge;       //!< Whether command may be merged.
};

#endif // NS_CMD_SET_PARAM3_H
