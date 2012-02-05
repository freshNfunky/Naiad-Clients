// -----------------------------------------------------------------------------
//
// NglTexture3D.h
//
// Texture object class for 3 dimensions.
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

#ifndef NGL_TEXTURE_3D_H
#define NGL_TEXTURE_3D_H

#include "NglExtensions.h"
#include "NglTextureBase.h"

#include <Ni.h>
#include <iostream>


namespace Ngl
{
// -----------------------------------------------------------------------------

class Texture3D
{
public:


    explicit
    Texture3D(const GLvoid* data,
              GLsizei width,
              GLsizei height,
              GLsizei depth,
              GLint   wrapS          = GL_CLAMP_TO_EDGE,
              GLint   wrapT          = GL_CLAMP_TO_EDGE,
              GLint   wrapR          = GL_CLAMP_TO_EDGE,
              GLint   internalFormat = GL_RGBA32F_ARB,   // TODO: Remove ARB
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
    GLsizei depth()  const { return _depth;  }
    GLint   wrapS()  const { return _wrapS;  }
    GLint   wrapT()  const { return _wrapT;  }
    GLint   wrapR()  const { return _wrapR;  }

    const TextureBase3D& base()                const { return _base;        }
    void                 bind()                const { _base.bind();        }
    void                 unbind()              const { _base.unbind();      }
    void                 data(GLvoid* texData) const { _base.data(texData); }

    void
    computeMinMax(GLfloat &min, GLfloat &max) const
    {
        // HACK: Assumes GL_RGBA32F_ARB!

        min =  (std::numeric_limits<GLfloat>::max)();
        max = -(std::numeric_limits<GLfloat>::max)();
        const GLint size = _width*_height*_depth*4;
        GLfloat *data = new GLfloat[size];
        bind();
        glGetTexImage(GL_TEXTURE_3D, 0, GL_RGBA/*GL_RGBA32F_ARB*/, GL_FLOAT, data);
        unbind();
        for (GLint i = 0; i < size; i += 4) {
            min = std::min<GLfloat>(min, data[i+3]); // Read alpha.
            max = std::max<GLfloat>(max, data[i+3]);
        }
        delete[] data;
#if 1
        std::cerr << "Tex min: " << min << " | Tex max: " << max << "\n";
#endif
    }

private:    // Member variables

    TextureBase3D _base;
    GLsizei       _width;
    GLsizei       _height;
    GLsizei       _depth;
    GLint         _wrapS;
    GLint         _wrapT;
    GLint         _wrapR;

private:    // Constants.

    static const GLenum _proxy3D  = TextureTarget3D::proxy;
    static const GLenum _target3D = TextureTarget3D::target;
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_TEXTURE_3D_H
