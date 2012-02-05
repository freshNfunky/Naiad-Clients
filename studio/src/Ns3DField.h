// -----------------------------------------------------------------------------
//
// Ns3DField.h
//
// ...
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

#ifndef NS3D_FIELD_H
#define NS3D_FIELD_H

#include "Ns3DResourceObject.h"

#include <NglExtensions.h>

#include <Ni.h>
#include <NbTileLayout.h>

namespace Ngl {
class SuperTileLayout;
class Texture3D;
class VertexBuffer;
}


class Ns3DField : public Ns3DResourceObject
{
public:     // Interface

    explicit
    Ns3DField(const NtString& name, const Nb::TileLayout* layout=0);

    virtual
    ~Ns3DField();

    void
    setLayout(const Nb::TileLayout* layout)
    { delete _layout; _layout=layout->clone(); }

    void
    setBackgroundValue(const NtVec3f& backgroundValue)
    { _backgroundValue=backgroundValue; }

    //! Returns the resource's background value
    virtual NtVec3f backgroundValue() const { return _backgroundValue; }
    
    //! Returns the name of the resource.
    virtual const NtString
    name() const 
    { return _name; }

    //! Returns the resource's tile-layout, if it exists
    virtual const Nb::TileLayout* 
    constLayoutPtr() const 
    { return _layout; }

    //! Returns the resource's world space bounding box
    virtual void
    bounds(NtVec3f& bmin, NtVec3f& bmax) const
    { 
        if(_layout) 
            _layout->allTileBounds(bmin,bmax); 
        else
            bmin=bmax=NtVec3f(0);
    }

    //! Returns the given named field
    virtual const Nb::Field1f&
    constNbField(const NtString& fieldName) const
    { return *_field[0]; }

    virtual Nb::Field1f&
    mutableNbField(const NtString& fieldName)
    { return *_field[0]; }

    //! Returns the given named component field
    virtual const Nb::Field1f&
    constNbField(const NtString& fieldName, const int component) const
    { return *_field[component]; }

    virtual Nb::Field1f&
    mutableNbField(const NtString& fieldName, const int component)
    { return *_field[component]; }

private:        // Member variables

    NtString            _name;     //!< The name of the field.
    Nb::Field1f*          _field[3]; //!< The raw field data
    const Nb::TileLayout* _layout;   //!< The tile layout for the field data
    NtVec3f             _backgroundValue; //!< Constant field value

};

#endif // NS3D_FIELD_H
