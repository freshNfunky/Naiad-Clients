// -----------------------------------------------------------------------------
//
// NsBodyOutputPlugObject.h
//
// Naiad Studio class for representing a Ng::BodyOutput, header file.
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

#ifndef NS_BODY_OUTPUT_PLUG_OBJECT_H
#define NS_BODY_OUTPUT_PLUG_OBJECT_H

#include "NsOutputPlugObject.h"   // Base.
#include "NsBodyCache.h"
#include <NiTypes.h>
#include <NgBodyOutput.h>         // Resource type.

class NsOpObject;                 // Parent type.

// -----------------------------------------------------------------------------

// NsBodyOutputPlugObject
// ----------------------
//! Class for objects representing body output plugs.

class NsBodyOutputPlugObject : public NsOutputPlugObject
{
    Q_OBJECT

public:     // Plug object interface.

    //! DTOR.
    virtual
    ~NsBodyOutputPlugObject()
    {}  

    //! Covariant return type.
    virtual const Ng::BodyOutput*
    constNgPlug() const
    { return _bodyOutput; }

    //! Covariant return type.
    virtual Ng::BodyOutput*
    mutableNgPlug() 
    { return _bodyOutput; }

    //! Return list of bodies from cache.
    virtual QList<const NsBodyObject*>
    constLiveBodies() const
    { return _liveBodyCache.constBodies(); }

    //! Return list of bodies from cache.
    virtual QList<NsBodyObject*>
    mutableLiveBodies()
    { return _liveBodyCache.mutableBodies(); }

public:     // Body output.

    const Ng::BodyOutput*
    constBodyOutput() const
    { return _bodyOutput; }

    Ng::BodyOutput*
    mutableBodyOutput()
    { return _bodyOutput; }

public:

    explicit
    NsBodyOutputPlugObject(Ng::BodyOutput &bodyOutput, NsOpObject &parent);

    const NsBodyCache&
    constLiveBodyCache() const
    { return _liveBodyCache; }

    NsBodyCache&
    mutableLiveBodyCache()
    { return _liveBodyCache; }

protected:

    void
    emitLiveBodyCacheChanged();

protected:

    virtual void
    groupPlugChanged();

protected slots:

    //void
    //onFeedChanged(const QString &inputLongName,
    //              const QString &plugLongName,
    //              bool           success);

    void
    onBeginOp(const NtTimeBundle &tb);

    void
    onEndOp(const NtTimeBundle &tb);

private:    // Live body caching.

    enum _BodyCachePolicy 
    {
        NoCache = 0,
        StatCache,
        FullCache
    };

    void
    _feedConfig(int &downstreamInputCount, int &dummyInputCount) const;

    static _BodyCachePolicy
    _bodyCachePolicy(int downstreamInputCount, int dummyInputCount);

    void
    _updateBodyCachePolicy();

    void
    _updateLiveBodyCache(_BodyCachePolicy bcp);

private:    // Member variables.

    Ng::BodyOutput *_bodyOutput;  //!< Server resource.
    NsBodyCache     _liveBodyCache;
};

// -----------------------------------------------------------------------------

#endif // NS_BODY_OUTPUT_PLUG_OBJECT_H
