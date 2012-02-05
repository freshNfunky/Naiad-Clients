// -----------------------------------------------------------------------------
//
// NglShaderUniformBase.h
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

#ifndef NGL_SHADER_UNIFORM_BASE_H
#define NGL_SHADER_UNIFORM_BASE_H

#include "NglExtensions.h"

#include <Nbx.h>
#include <Ni.h>

namespace Nb {
class TimeBundle;
}

namespace Ngl
{
// -----------------------------------------------------------------------------

class ShaderUniformBase
{
public:     // Interface

    ShaderUniformBase(const NtString& name,
                      GLenum          type,
                      GLint           size,
                      GLint           location);

    virtual
    ~ShaderUniformBase() 
    {}

    const NtString& name()      const { return _name;      }
    GLenum          type()      const { return _type;      }
    GLint           size()      const { return _size;      }
    GLint           location()  const { return _location;  }
    GLboolean       transpose() const { return _transpose; }

    void 
    setTranspose(const GLboolean transpose)
    { _transpose = transpose; }

    virtual void
    upload(const Nb::TimeBundle& tb)
    { NB_THROW("ShaderUniform::upload() not implemented for this type."); }

private:    // Member variables.

    const NtString _name;
    GLenum         _type;
    GLint          _size;
    GLint          _location;
    GLboolean      _transpose;
              
private:    // Utility functions.

    static GLenum _validType(GLenum type);
    static GLint  _validSize(GLint size);
    static GLint  _validLocation(GLint location);

private:    // Disabled.

    ShaderUniformBase();
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl

#endif  // NGL_SHADER_UNIFORM_BASE_H
