// -----------------------------------------------------------------------------
//
// NsBodyCache.h
//
// Class for caching bodies, header file.
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

#ifndef NS_BODY_CACHE_H
#define NS_BODY_CACHE_H

#include <QString>
#include <QList>
#include <QHash>

class NsBodyObject;
class NsOpObject;
class NsBodyOutputPlugObject;

namespace Nb { class Body; }

// -----------------------------------------------------------------------------

// BodyCache
// ---------
//! Class for managing a list of pointers to body objects.

class NsBodyCache
{
public:

    explicit
    NsBodyCache();

    virtual
    ~NsBodyCache();

public:

    int
    size() const
    { return _bodies.size(); }

    bool
    empty() const
    { return _bodies.empty(); }


    NsBodyObject*
    addBody(Nb::Body               &nbb,
            NsOpObject             &op,
            NsBodyOutputPlugObject *bopo,
            bool                    live);

    void
    clear();

public:     // Body access.

    //! Returns null if no body with name is found.
    const NsBodyObject*
    queryConstBody(const QString &name) const
    { return _bodies.value(name, 0); }

    //! Returns null if no body with name is found.
    NsBodyObject*
    queryMutableBody(const QString &name)
    { return _bodies.value(name, 0); }


    QList<const NsBodyObject*>
    constBodies() const;

    QList<NsBodyObject*>
    mutableBodies()
    { return _bodies.values(); }

private:    // Member variables.

    typedef QHash<QString,NsBodyObject*> _BodyHashType;
    _BodyHashType _bodies;

private:

    NsBodyCache(const NsBodyCache&);            //!< Disabled copy CTOR.
    NsBodyCache& operator=(const NsBodyCache&); //!< Disabled assign.
};

#endif // NS_BODY_CACHE_H


//    explicit
//    BodyCache(const QList<const Nb::Body*>& bodies,
//              const bool                    live,
//              const bool                    storeBodies,
//              const int                     frame,
//              const bool                    hardcodedEmp,
//              const float                   bodyScale,
//              const bool                    repeatLastFrame);


//    NtTimeBundle _tb;

//    const bool     _live;
//    const int      _frame;
//    const bool     _hardcodedEmp;
//    const float    _bodyScale;
//    const bool     _repeatEndFrame;



//const NtTimeBundle &time() const { return _tb; }


//    NtVec3f bodyCentroid(int& bodyCount, float& zmin);
//    void bodyBounds(NtVec3f &min, NtVec3f &max);

//    const QList<NsBody*> &bodies() const { return _bodies; }
//    bool                  live()   const { return _live; }

//    bool hasShapeData() const;
//    bool liveMatch(const int frame) const;
//    void inherit(BodyCache* bc);
