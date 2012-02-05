// -----------------------------------------------------------------------------
//
// NglTexture2D.h
//
// Texture object class for 2 dimensions.
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

#ifndef NGL_TEXTURE_2D_H
#define NGL_TEXTURE_2D_H

#include "NglExtensions.h"
#include "NglTextureBase.h"
#include "NglTextureTarget.h"


namespace Ngl
{
// -----------------------------------------------------------------------------

class Texture2D
{
public:     // Interface.

    explicit
    Texture2D(const GLvoid* data,
              GLsizei width,
              GLsizei height,
              GLint   wrapS          = GL_CLAMP_TO_EDGE,
              GLint   wrapT          = GL_CLAMP_TO_EDGE,
              GLint   internalFormat = GL_RGBA32F_ARB,  // TODO: Remove ARB
              GLenum  format         = GL_RGBA,
              GLenum  type           = GL_FLOAT,
              GLint   border         = 0,
              GLint   minFilter      = GL_LINEAR,
              GLint   magFilter      = GL_LINEAR);

    // Default copy, assign & destruct.
    // Note that copy & assign will give linker errors because
    // TextureBase is noncopyable.

    GLsizei width()  const { return _width;  }
    GLsizei height() const { return _height; }
    GLint   wrapS()  const { return _wrapS;  }
    GLint   wrapT()  const { return _wrapT;  }

    const TextureBase2D& base()                const { return _base;        }
    void                 bind()                const { _base.bind();        }
    void                 unbind()              const { _base.unbind();      }
    void                 data(GLvoid* texData) const { _base.data(texData); }

private:    // Member variables

    TextureBase2D _base;
    GLsizei       _width;
    GLsizei       _height;
    GLint         _wrapS;
    GLint         _wrapT;

private:    // Constants.

    static const GLenum _proxy2D  = TextureTarget2D::proxy;
    static const GLenum _target2D = TextureTarget2D::target;

private:    // Disabled

    Texture2D();
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_TEXTURE_2D_H
