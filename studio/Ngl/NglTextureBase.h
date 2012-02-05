// -----------------------------------------------------------------------------
//
// NglTextureBase.h
//
// Dimensionless information for textures.
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

#ifndef NGL_TEXTURE_BASE_H
#define NGL_TEXTURE_BASE_H

#include "NglNonCopyable.h"
#include "NglExtensions.h"
#include "NglTextureTarget.h"

#include <Nbx.h>

namespace Ngl
{
// -----------------------------------------------------------------------------

// Declaration

template<GLint D>
class TextureBase : private NonCopyable
{
public:     // Interface

    TextureBase(GLint  internalFormat = GL_RGBA32F_ARB,
                GLenum format         = GL_RGBA,
                GLenum type           = GL_FLOAT,
                GLint  border         = 0,
                GLint  minFilter      = GL_LINEAR,
                GLint  magFilter      = GL_LINEAR);

    ~TextureBase();

    GLint  internalFormat() const { return _internalFormat; }
    GLenum format()         const { return _format;         }
    GLenum type()           const { return _type;           }
    GLint  border()         const { return _border;         }
    GLint  level()          const { return _level;          }
    GLint  minFilter()      const { return _minFilter;      }
    GLint  magFilter()      const { return _magFilter;      }

    void bind() const;
    void unbind() const;
    void data(GLvoid* texData) const;

public:     // Public utility functions

    static GLsizei validDim(const GLsizei dim);
    static GLint   validWrap(const GLint wrap);

private:    // Member variables

    GLuint _handle;
    GLint  _internalFormat;
    GLenum _format;
    GLenum _type;
    GLint  _border;
    GLint  _level;   // For mipmaps, currently not used.
    GLint  _minFilter;
    GLint  _magFilter;

private:    // Private utility functions

    static GLint  _validInternalFormat(GLint internalFormat);
    static GLenum _validFormat(GLenum format);
    static GLenum _validType(GLenum type);
    static GLint  _validBorder(GLint border);
    static GLint  _validFilter(GLint filter);
};


typedef TextureBase<1> TextureBase1D;
typedef TextureBase<2> TextureBase2D;
typedef TextureBase<3> TextureBase3D;

// -----------------------------------------------------------------------------

// Definitions.


// TextureBase
// ------------
//! Constructor

template<GLint D>
TextureBase<D>::TextureBase(const GLint  internalFormat,
                            const GLenum format,
                            const GLenum type,
                            const GLint  border,
                            const GLint  minFilter,
                            const GLint  magFilter)
    : _handle(0),
      _internalFormat(_validInternalFormat(internalFormat)),
      _format(_validFormat(format)),
      _type(_validType(type)),
      _border(_validBorder(border)),
      _level(0),
      _minFilter(_validFilter(minFilter)),
      _magFilter(_validFilter(magFilter))
{
    glGenTextures(1, &_handle);

    if (0 == _handle) {
        NB_THROW("Invalid texture handle: " << _handle);
    }
}


// ~TextureBase
// ------------
//! Destructor

template<GLint D>
TextureBase<D>::~TextureBase()
{
    glDeleteTextures(1, &_handle);
}


// bind
// ------
//! Bind texture.

template<GLint D>
void
TextureBase<D>::bind() const
{
    glBindTexture(TextureTarget<D>::target, _handle);
}


// unbind
// ------
//! Unbind texture.

template<GLint D>
void
TextureBase<D>::unbind() const
{
    glBindTexture(TextureTarget<D>::target, 0);
}


// data
// ----
//! Retrieve texture data.

template<GLint D>
void
TextureBase<D>::data(GLvoid* texData) const
{
    bind();
    glGetTexImage(TextureTarget<D>::target,
                  _level,
                  _format,
                  _type,
                  texData);
    unbind();
}


// validDim
// --------
//! Throws an exception if input is invalid, otherwise return input.

template<GLint D>
GLsizei
TextureBase<D>::validDim(const GLsizei dim)
{
#if 1
    //GLint maxSize;
    //glGetIntegerv(TextureTarget<D>::maxSize, &maxSize);

    //if (1 > dim || dim > maxSize) {
    if (1 > dim) {
        NB_THROW("Invalid texture dimension: " << dim);
    }
#endif

    return dim;
}


// validWrap
// ---------
//! Throws an exception if input is invalid, otherwise return input.

template<GLint D>
GLint
TextureBase<D>::validWrap(const GLint wrap)
{
#if 1
    if (GL_CLAMP           != wrap &&
        GL_CLAMP_TO_EDGE   != wrap &&
        GL_CLAMP_TO_BORDER != wrap &&
        GL_REPEAT          != wrap) {
        NB_THROW("Invalid texture wrap: " << wrap);
    }
#endif

    return wrap;
}


// _validInternalFormat
// --------------------
//! Throws an exception if input is invalid, otherwise return input.

template<GLint D>
GLint
TextureBase<D>::_validInternalFormat(const GLint internalFormat)
{
#if 1
    // NB: Add more formats if necessary.

    if (internalFormat != GL_DEPTH_COMPONENT &&
        internalFormat != GL_INTENSITY       &&
        internalFormat != GL_RED             &&
        internalFormat != GL_GREEN           &&
        internalFormat != GL_BLUE            &&
        internalFormat != GL_RGB             &&
        internalFormat != GL_RGBA            &&
#ifndef __APPLE__
#ifndef WIN32
        internalFormat != GL_RGBA32F         &&
#endif
#endif
        internalFormat != GL_RGBA32F_ARB) {
        NB_THROW("Invalid texture internal format: " << internalFormat);
    }
#endif

    return internalFormat;
}


// _validFormat
// ------------
//! Throws an exception if input is invalid, otherwise return input.

template<GLint D>
GLenum
TextureBase<D>::_validFormat(const GLenum format)
{
#if 1
    // NB: Add more internal formats if necessary.

    if (format != GL_DEPTH_COMPONENT &&
        format != GL_RED &&
        format != GL_GREEN &&
        format != GL_BLUE &&
        format != GL_RGB &&
        format != GL_RGBA) {
        NB_THROW("Invalid texture format: " << format);
    }
#endif

    return format;
}


// _validType
// ----------
//! Throws an exception if input is invalid, otherwise return input.

template<GLint D>
GLenum
TextureBase<D>::_validType(const GLenum type)
{
#if 1
    if (type != GL_UNSIGNED_BYTE  && type != GL_BYTE  &&
        type != GL_UNSIGNED_SHORT && type != GL_SHORT &&
        type != GL_UNSIGNED_INT   && type != GL_INT   &&
#ifndef __APPLE__
#ifndef WIN32
        type != GL_HALF_FLOAT     &&
#endif
#endif
        type != GL_FLOAT) {
        NB_THROW("Invalid texture type: " << type);
    }
#endif

    return type;
}


// _validBorder
// ------------
//! Throws an exception if input is invalid, otherwise return input.

template<GLint D>
GLint
TextureBase<D>::_validBorder(const GLint border)
{
#if 1
    if (border < 0) {
        NB_THROW("Invalid texture border: " << border);
    }
#endif

    return border;
}


// _validFilter
// ------------
//! Throws an exception if input is invalid, otherwise return input.

template<GLint D>
GLint
TextureBase<D>::_validFilter(const GLint filter)
{
#if 1
    // Mipmapping not supported yet...

    if (GL_NEAREST != filter &&
        GL_LINEAR  != filter) {
        NB_THROW("Invalid texture filter: " << filter);
    }
#endif

    return filter;
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_TEXTURE_BASE_H
