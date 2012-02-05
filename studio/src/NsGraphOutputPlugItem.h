// -----------------------------------------------------------------------------
//
// NsGraphOutputPlugItem.h
//
// Abstract base class for Op output plug items, header file.
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

#ifndef NS_GRAPH_OUTPUT_PLUG_ITEM_H
#define NS_GRAPH_OUTPUT_PLUG_ITEM_H

#include "NsGraphPlugItem.h"
#include "NsOutputPlugObject.h"

class NsGraphOpItem;

// -----------------------------------------------------------------------------

// NsGraphOutputPlugItem
// --------------------
//! Abstract base class for output plug items.

class NsGraphOutputPlugItem : public NsGraphPlugItem
{
    Q_OBJECT

public:     // NsGraphPlugItem interface.

    virtual
    ~NsGraphOutputPlugItem() = 0;           // Force abstract.

    //! Co-variant return type.
    virtual NsOutputPlugObject* 
    plugObject() const
    { return _opo; }

protected:

    explicit
    NsGraphOutputPlugItem(NsOutputPlugObject *opo,
                          NsGraphOpItem      &parent,
                          const qreal         diameter)
        : NsGraphPlugItem(opo, parent, diameter)
        , _opo(opo)
    {}

private:    // Member variables.

    NsOutputPlugObject *_opo;  //!< Resource.
};

// -----------------------------------------------------------------------------

#endif // NS_GRAPH_OUTPUT_PLUG_ITEM_H
