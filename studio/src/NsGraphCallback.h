// -----------------------------------------------------------------------------
//
// NsGraphCallback.h
//
// Naiad Studio graph callback, header file.
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

#ifndef NS_GRAPH_CALLBACK_H
#define NS_GRAPH_CALLBACK_H

#include <QObject>
#include <NiTypes.h>

// -----------------------------------------------------------------------------

// NsGraphCallback
// ---------------
//! The NsGraphCallback class, representing the Naiad graph callback.
//! Updates the information displayed in the user interface during graph
//! stepping, and provides a way to abort stepping.

class NsGraphCallback : public QObject
{
    Q_OBJECT

public:     // Singleton interface.

    static NsGraphCallback*
    instance();

    static void 
    createInstance();

    static void 
    destroyInstance();

private:

    static NsGraphCallback *_instance;

private:

    explicit
    NsGraphCallback();

    //! DTOR.
    virtual
    ~NsGraphCallback()
    {}    

public:

    void
    emitBeginStep(const NtTimeBundle &tb)
    { emit beginStep(tb); }

    void
    emitBeginFrame(const NtTimeBundle &tb)
    { emit beginFrame(tb); }

    void
    emitBeginTimeStep(const NtTimeBundle &tb)
    { emit beginTimeStep(tb); }

    void
    emitEndTimestep(const NtTimeBundle &tb)
    { emit endTimeStep(tb); }

    void
    emitEndFrame(const NtTimeBundle &tb)
    { emit endFrame(tb); }

    void
    emitEndStep(const NtTimeBundle &tb)
    { emit endStep(tb); }

    void
    emitReset()
    { emit reset(); }

    void
    emitBeginOp(const NtTimeBundle &tb, const QString &opInstance)
    { emit beginOp(tb, opInstance); }

    void
    emitEndOp(const NtTimeBundle &tb, const QString &opInstance)
    { emit endOp(tb, opInstance); }

    void
    setSteppingStopped(const bool stop)
    { _steppingStopped = stop; }

    bool
    steppingStopped() const
    { return _steppingStopped; }

signals:

    void 
    beginStep(const NtTimeBundle &tb);
    
    void 
    beginFrame(const NtTimeBundle &tb);
    
    void 
    beginTimeStep(const NtTimeBundle &tb);

    void 
    endTimeStep(const NtTimeBundle &tb);
    
    void 
    endFrame(const NtTimeBundle &tb);
    
    void 
    endStep(const NtTimeBundle &tb);

    void 
    reset();

    void 
    beginOp(const NtTimeBundle &tb, const QString &opInstance);
    
    void 
    endOp(const NtTimeBundle &tb, const QString &opInstance);

private:

    class _Callback : public NtGraphCallback
    {
    public:     // NtGraphCallback interface.

        //! DTOR.
        virtual
        ~_Callback()
        {}

        virtual bool
        beginStep(const NtTimeBundle &tb);

        virtual bool
        beginFrame(const NtTimeBundle &tb);

        virtual bool
        beginTimestep(const NtTimeBundle &tb);

        virtual bool
        endTimestep(const NtTimeBundle &tb);

        virtual bool
        endFrame(const NtTimeBundle &tb);

        virtual bool
        endStep(const NtTimeBundle &tb);

        virtual bool
        reset();

        virtual void
        beginOp(const NtTimeBundle &tb,const NtString &opInstance);

        virtual void
        endOp(const NtTimeBundle &tb, const NtString &opInstance);

    public:

        explicit
        _Callback(NsGraphCallback &gcb);

    private:    // Member variables.

        NsGraphCallback *_gcb;  //!< "parent".

    private:
        
        static void
        _wakeGui();

    private:

        _Callback();                                //!< Disabled.
        _Callback(const _Callback&);                //!< Disabled.
        _Callback& operator=(const _Callback&);     //!< Disabled.
    };

private:    // Member variables.

    _Callback _cb;              //!< Callback instance.
    bool      _steppingStopped;

private:

    NsGraphCallback(const NsGraphCallback&);                //!< Disabled.
    NsGraphCallback& operator=(const NsGraphCallback&);     //!< Disabled.
};

#endif  // NS_GRAPH_CALLBACK_H
