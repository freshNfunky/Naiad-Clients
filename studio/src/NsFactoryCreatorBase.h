// -----------------------------------------------------------------------------
//
// NsFactoryCreatorBase.h
//
// Naiad Studio factory creator base header file.
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

#ifndef NS_FACTORY_CREATOR_BASE_H
#define NS_FACTORY_CREATOR_BASE_H


// NsFactoryCreatorBase
// ------------------
//! Abstract base class template for creators used with factories.

template<class P, class A>
class NsFactoryCreatorBase
{
public:     // Types.

    typedef P ProductType;
    typedef A ArgsType;

public:     // Interface.

    //! DTOR.
    virtual ~NsFactoryCreatorBase() = 0;    // Force abstract.

    //! Override in derived classes.
    virtual P *create(const A &args) const = 0;

protected:

    //! CTOR.
    NsFactoryCreatorBase() {}

private:

    //! Disable copy.
    NsFactoryCreatorBase(const NsFactoryCreatorBase<P,A> &);

    //! Disable assign.
    NsFactoryCreatorBase &operator=(const NsFactoryCreatorBase<P,A> &);
};

// -----------------------------------------------------------------------------

// ~NsFactoryCreatorBase
// ---------------------
//! DTOR.

template<class P, class A>
NsFactoryCreatorBase<P,A>::~NsFactoryCreatorBase()
{}

#endif // NS_FACTORY_CREATOR_BASE_H
