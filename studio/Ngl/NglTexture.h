// -----------------------------------------------------------------------------
//
// NglTexture.h
//
// Texture object classes for {1,2,3} dimensions.
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

#ifndef NGL_TEXTURE_H
#define NGL_TEXTURE_H

#include "NglNonCopyable.h"
#include "NglExtensions.h"

#include <Ngx.h>


namespace Ngl
{
// -----------------------------------------------------------------------------


class Texture1D
{
public:

    explicit
    Texture1D(const GLvoid* data,
              const GLsizei width,
              const GLint   wrapS          = GL_CLAMP_TO_EDGE,
              const GLint   internalFormat = GL_RGBA32F_ARB,
              const GLenum  format         = GL_RGBA,
              const GLenum  type           = GL_FLOAT,
              const GLint   border         = 0,
              const GLint   minFilter      = GL_LINEAR,
              const GLint   magFilter      = GL_LINEAR)
        : _base(internalFormat, format, type, border, minFilter, magFilter),
          _width(TextureBase<1>::validDim(width)),
          _wrapS(TextureBase<1>::validWrap(wrapS))
    {
        bind();
        glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_WRAP_S,_wrapS);
        glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MIN_FILTER,_base.minFilter());
        glTexParameteri(GL_TEXTURE_1D,GL_TEXTURE_MAG_FILTER,_base.magFilter());
        glTexImage1D(GL_TEXTURE_1D,
                     _base.level(),
                     _base.internalFormat(),
                     _width,
                     _base.border(),
                     _base.format(),
                     _base.type(),
                     data);
        unbind();
    }


    GLsizei width() const { return _width; }
    GLint   wrapS() const { return _wrapS; }

    const TextureBase<1>& base()                const { return _base;        }
    void                  bind()                const { _base.bind();        }
    void                  unbind()              const { _base.unbind();      }
    void                  data(GLvoid* texData) const { _base.data(texData); }

private:    // Member variables

    TextureBase<1> _base;
    GLsizei        _width;
    GLint          _wrapS;

private:    // Disabled

    Texture1D();
};

// -----------------------------------------------------------------------------

class Texture2D
{
public:

    explicit
    Texture2D(const GLvoid* data,
              const GLsizei width,
              const GLsizei height,
              const GLint   wrapS          = GL_CLAMP_TO_EDGE,
              const GLint   wrapT          = GL_CLAMP_TO_EDGE,
              const GLint   internalFormat = GL_RGBA32F_ARB,
              const GLenum  format         = GL_RGBA,
              const GLenum  type           = GL_FLOAT,
              const GLint   border         = 0,
              const GLint   minFilter      = GL_LINEAR,
              const GLint   magFilter      = GL_LINEAR)
        : _base(internalFormat, format, type, border, minFilter, magFilter),
          _width(TextureBase<2>::validDim(width)),
          _height(TextureBase<2>::validDim(height)),
          _wrapS(TextureBase<2>::validWrap(wrapS)),
          _wrapT(TextureBase<2>::validWrap(wrapT))
    {
        bind();
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,_wrapS);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,_wrapT);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,_base.minFilter());
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,_base.magFilter());
        glTexImage2D(GL_TEXTURE_2D,
                     _base.level(),
                     _base.internalFormat(),
                     _width, _height,
                     _base.border(),
                     _base.format(),
                     _base.type(),
                     data);
        unbind();
    }

    GLsizei width()  const { return _width;  }
    GLsizei height() const { return _height; }
    GLint   wrapS()  const { return _wrapS;  }
    GLint   wrapT()  const { return _wrapT;  }

    const TextureBase<2>& base()                const { return _base;        }
    void                  bind()                const { _base.bind();        }
    void                  unbind()              const { _base.unbind();      }
    void                  data(GLvoid* texData) const { _base.data(texData); }

private:    // Member variables

    TextureBase<2> _base;
    GLsizei        _width;
    GLsizei        _height;
    GLint          _wrapS;
    GLint          _wrapT;

private:    // Disabled

    Texture2D();
};

// -----------------------------------------------------------------------------

class Texture3D
{
public:

    explicit
    Texture3D(const GLvoid* data,
              const GLsizei width,
              const GLsizei height,
              const GLsizei depth,
              const GLint   wrapS          = GL_CLAMP_TO_EDGE,
              const GLint   wrapT          = GL_CLAMP_TO_EDGE,
              const GLint   wrapR          = GL_CLAMP_TO_EDGE,
              const GLint   internalFormat = GL_RGBA32F_ARB,
              const GLenum  format         = GL_RGBA,
              const GLenum  type           = GL_FLOAT,
              const GLint   border         = 0,
              const GLint   minFilter      = GL_LINEAR,
              const GLint   magFilter      = GL_LINEAR)
        : _base(internalFormat, format, type, border, minFilter, magFilter),
          _width(TextureBase<3>::validDim(width)),
          _height(TextureBase<3>::validDim(height)),
          _depth(TextureBase<3>::validDim(depth)),
          _wrapS(TextureBase<3>::validWrap(wrapS)),
          _wrapT(TextureBase<3>::validWrap(wrapT)),
          _wrapR(TextureBase<3>::validWrap(wrapR))
    {
        bind();
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_S,_wrapS);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_T,_wrapT);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_WRAP_R,_wrapR);
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MIN_FILTER,_base.minFilter());
        glTexParameteri(GL_TEXTURE_3D,GL_TEXTURE_MAG_FILTER,_base.magFilter());
        glTexImage3D(GL_TEXTURE_3D,
                     _base.level(),
                     _base.internalFormat(),
                     _width, _height, _depth,
                     _base.border(),
                     _base.format(),
                     _base.type(),
                     data);
        unbind();
    }

    GLsizei width()  const { return _width;  }
    GLsizei height() const { return _height; }
    GLsizei depth()  const { return _depth;  }
    GLint   wrapS()  const { return _wrapS;  }
    GLint   wrapT()  const { return _wrapT;  }
    GLint   wrapR()  const { return _wrapR;  }

    const TextureBase<3>& base()                const { return _base;        }
    void                  bind()                const { _base.bind();        }
    void                  unbind()              const { _base.unbind();      }
    void                  data(GLvoid* texData) const { _base.data(texData); }

private:    // Member variables

    TextureBase<3> _base;
    GLsizei        _width;
    GLsizei        _height;
    GLsizei        _depth;
    GLint          _wrapS;
    GLint          _wrapT;
    GLint          _wrapR;

private:    // Disabled

    Texture3D();
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_TEXTURE_H
