// -----------------------------------------------------------------------------
//
// NglShaderUniformBase.cc
//
// Base class for uniform variables in shader programs.
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

#include "NglShaderUniformBase.h"

#include <Nbx.h>

namespace Ngl
{
// -----------------------------------------------------------------------------

// ShaderUniform
// -------------
//! Constructor.

ShaderUniformBase::ShaderUniformBase(const NtString& name,
                                     const GLenum    type,
                                     const GLint     size,
                                     const GLint     location)
    : _name     (name),
      _type     (_validType(type)),
      _size     (_validSize(size)),
      _location (_validLocation(location)),
      _transpose(GL_FALSE)
{}


// _validType
// ----------
//! Throws if input is invalid, otherwise returns a valid type.

GLenum
ShaderUniformBase::_validType(const GLenum type)
{
#if 1
    if (GL_FLOAT             != type &&
        GL_FLOAT_VEC2        != type &&
        GL_FLOAT_VEC3        != type &&
        GL_FLOAT_VEC4        != type &&
        GL_INT               != type &&
        GL_INT_VEC2          != type &&
        GL_INT_VEC3          != type &&
        GL_INT_VEC4          != type &&
        GL_BOOL              != type &&
        GL_BOOL_VEC2         != type &&
        GL_BOOL_VEC3         != type &&
        GL_BOOL_VEC4         != type &&
        GL_FLOAT_MAT2        != type &&
        GL_FLOAT_MAT3        != type &&
        GL_FLOAT_MAT4        != type &&
        GL_FLOAT_MAT2x3      != type &&
        GL_FLOAT_MAT2x4      != type &&
        GL_FLOAT_MAT3x2      != type &&
        GL_FLOAT_MAT3x4      != type &&
        GL_FLOAT_MAT4x2      != type &&
        GL_FLOAT_MAT4x3      != type &&
        GL_SAMPLER_1D        != type &&
        GL_SAMPLER_2D        != type &&
        GL_SAMPLER_3D        != type &&
        GL_SAMPLER_CUBE      != type &&
        GL_SAMPLER_1D_SHADOW != type &&
        GL_SAMPLER_2D_SHADOW != type) {
        NB_THROW("Invalid shader uniform type: " << type);
    }
#endif

    return type;
}


// _validSize
// ----------
//! Throws if input is invalid, otherwise returns a valid size.

GLint
ShaderUniformBase::_validSize(const GLint size)
{
#if 1
    if (size < 1) {
        NB_THROW("Invalid shader uniform size: " << size);
    }
#endif

    return size;
}


// _validLocation
// --------------
//! Throws if input is invalid, otherwise return a valid location.

GLint
ShaderUniformBase::_validLocation(const GLint location)
{
#if 0
    // Note that built-in OpenGL uniforms have location -1.

    if (location < 0) {
        NG_THROW("Invalid shader uniform location: " << location);
    }
#endif

    return location;
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
