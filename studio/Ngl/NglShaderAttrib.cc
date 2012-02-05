// -----------------------------------------------------------------------------
//
// NglShaderAttrib.cc
//
// Class for vertex attributes in shader programs.
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
// -----------------------------------------------------------------------------

#include "NglShaderAttrib.h"
#include <Nbx.h>

namespace Ngl
{
// -----------------------------------------------------------------------------

// ShaderAttrib
// ------------
//! Constructor.

ShaderAttrib::ShaderAttrib(const GLenum type,
                           const GLint  size,
                           const GLint  location)
    : _type    (_validType(type)),
      _size    (_validSize(size)),
      _location(_validLocation(location))
{}


// type
// ----
//! Attribute type.

GLenum
ShaderAttrib::type() const
{
    return _type;
}


// size
// ----
//! Attribute array size, one if not an array.

GLint
ShaderAttrib::size() const
{
    return _size;
}


// location
// --------
//! Attribute location.

GLint
ShaderAttrib::location() const
{
    return _location;
}


// _validType
// ----------
//! Throws if input is invalid, otherwise returns a valid type.

GLenum
ShaderAttrib::_validType(const GLenum type)
{
#if 1
    if (GL_FLOAT        != type &&
        GL_FLOAT_VEC2   != type &&
        GL_FLOAT_VEC3   != type &&
        GL_FLOAT_VEC4   != type &&
        GL_FLOAT_MAT2   != type &&
        GL_FLOAT_MAT3   != type &&
        GL_FLOAT_MAT4   != type &&
        GL_FLOAT_MAT2x3 != type &&
        GL_FLOAT_MAT2x4 != type &&
        GL_FLOAT_MAT3x2 != type &&
        GL_FLOAT_MAT3x4 != type &&
        GL_FLOAT_MAT4x2 != type &&
        GL_FLOAT_MAT4x3 != type) {
        NB_THROW("Invalid shader attribute type: " << type);
    }
#endif

    return type;
}


// _validSize
// ----------
//! Throws if input is invalid, otherwise returns a valid size.

GLint
ShaderAttrib::_validSize(const GLint size)
{
#if 1
    if (size < 1) {
        NB_THROW("Invalid shader attribute size: " << size);
    }
#endif

    return size;
}


// _validLocation
// ----------
//! Throws if input is invalid, otherwise returns a valid location.

GLint
ShaderAttrib::_validLocation(const GLint location)
{
#if 0
    // Note that built-in OpenGL attributes have location -1.

    if (location < 0) {
        NG_THROW("Invalid shader attribute location: " << location);
    }
#endif

    return location;
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
