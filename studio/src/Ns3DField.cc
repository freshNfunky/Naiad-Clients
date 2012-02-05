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

#include "Ns3DField.h"

#include <NglSuperTileLayout.h>
#include <NglSuperTile.h>
#include <NglTextureUtils.h>
#include <NglVertexBuffer.h>

#include <Nbx.h>    // NB_THROW
#include <NbLog.h>  // NB_WARNING
#include <NbField.h> 

#include <sstream>
#include <em_field_base3.h>

// Ns3DField
// ---------------
//! Constructor.

Ns3DField::Ns3DField(const NtString& name, const Nb::TileLayout* layout)
    : Ns3DResourceObject(),_name(name),_layout(layout),_backgroundValue(0)
{
#if 0
    std::cerr << "Create Ns3DField" << std::endl;
#endif
    for(int i=0; i<3; ++i)
        _field[i]=Nb::createField1f(em::centered);
}


// Ns3DField
// ---------------
//! Destructor.

Ns3DField::~Ns3DField()
{    
#if 0
    std::cerr << "Destroy Ns3DField\n";
#endif
    for(int i=0; i<3; ++i) delete _field[i];
    delete _layout;
}
