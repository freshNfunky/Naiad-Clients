// -----------------------------------------------------------------------------
//
// Ns3DOpItem.h
//
// Interface for renderable objects.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved. 
//
// This file is part of Open Naiad Studio..
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
//
// -----------------------------------------------------------------------------

#ifndef NS3D_OPITEM_H
#define NS3D_OPITEM_H

#include "Ns3DGraphicsItem.h"
#include "NsQuery.h"
#include <NglTypes.h>
#include <QString>

class NsOpObject;
class NsUndoStack;

// -----------------------------------------------------------------------------

class Ns3DOpItem : public Ns3DGraphicsItem
{
public:

    //! CTOR.
    explicit
    Ns3DOpItem(NsOpObject    *opObject,
               const int32_t  id)
        : Ns3DGraphicsItem(id, true)    // All OpItems are selectable.
        , _opObject(opObject)
    {}

    //! DTOR.
    virtual
    ~Ns3DOpItem()
    {}

    NsOpObject*
    opObject() const
    { return _opObject; }

public:

    virtual Ngl::vec3f
    translate() const
    { return getParam3f(translateParamName()); }

    virtual Ngl::vec3f
    rotate() const
    { return getParam3f(rotateParamName()); }

    virtual Ngl::vec3f
    scale() const
    { return getParam3f(scaleParamName()); }

public:

    virtual bool
    isVisible() const;

protected:

    float
    getParam1f(const QString &paramName) const;

    Ngl::vec3f
    getParam3f(const QString &paramName) const;

private:    // Member variables.

    NsOpObject *_opObject;
};

#endif // NS3D_OPITEM_H
