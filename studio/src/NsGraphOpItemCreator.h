// -----------------------------------------------------------------------------
//
// NsGraphOpItemCreator.h
//
// Naiad Studio graph op item creator, header file.
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

#ifndef NS_GRAPH_OP_ITEM_CREATOR_H
#define NS_GRAPH_OP_ITEM_CREATOR_H

#include "NsFactoryCreatorBase.h"
#include "NsGraphOpItemFactory.h"

class NsGraphOpItem;
class NsGraphOpItemArgs;

// -----------------------------------------------------------------------------

//! Creator base type.
typedef NsFactoryCreatorBase<NsGraphOpItem, NsGraphOpItemArgs>
        NsGraphOpItemCreatorBase;

// -----------------------------------------------------------------------------

// NsGraphOpItemCreator
// --------------------
//! This template is instantiated in derived classes to provide
//! a simple mechanism for specifying creators therein.

template<class T>
class NsGraphOpItemCreator : public NsGraphOpItemCreatorBase
{
public:

    //! CTOR. Registers the template type (T) with the factory.
    explicit
    NsGraphOpItemCreator(NsGraphOpItemFactory &factory)
        : NsGraphOpItemCreatorBase()
    {
        factory.registerCreator(T::key, this);
    }

    //! DTOR.
    virtual 
    ~NsGraphOpItemCreator() 
    {}

    // Copy and assign disabled in base class.


    //! Create instance of template type.
    virtual NsGraphOpItemCreatorBase::ProductType *
    create(const NsGraphOpItemCreatorBase::ArgsType &args) const
    {
        return new T(args);
    }
};

#endif // NS_GRAPH_OP_ITEM_CREATOR_H
