// -----------------------------------------------------------------------------
//
// NglTexture1D.cc
//
// Texture object class for 1 dimension.
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

#include "NglTexture1D.h"
#include "NglTextureTarget.h"

namespace Ngl
{
// -----------------------------------------------------------------------------

Texture1D::Texture1D(const GLvoid* data,
                     const GLsizei width,
                     const GLint   wrapS,
                     const GLint   internalFormat,
                     const GLenum  format,
                     const GLenum  type ,
                     const GLint   border,
                     const GLint   minFilter,
                     const GLint   magFilter)
    : _base(internalFormat, format, type, border, minFilter, magFilter),
      _width(TextureBase1D::validDim(width)),
      _wrapS(TextureBase1D::validWrap(wrapS))
{
    bind();

#if 0
    // Use proxy to test if settings are valid.

    glTexParameteri(_proxy1D, GL_TEXTURE_WRAP_S, _wrapS);
    glTexParameteri(_proxy1D, GL_TEXTURE_MIN_FILTER, _base.minFilter());
    glTexParameteri(_proxy1D, GL_TEXTURE_MAG_FILTER, _base.magFilter());

    glTexImage2D(_proxy1D,
                 _base.level(),
                 _base.internalFormat(),
                 _width,
                 _base.border(),
                 _base.format(),
                 _base.type(),
                 0);    // No data transferred.

    GLint w(0);
    glGetTexLevelParameteriv(_proxy1D, _base.level(), GL_TEXTURE_WIDTH, &w);

    if (0 == w) {
        NG_THROW("Invalid texture proxy");
    }
#endif

    glTexParameteri(_target1D, GL_TEXTURE_WRAP_S, _wrapS);
    glTexParameteri(_target1D, GL_TEXTURE_MIN_FILTER, _base.minFilter());
    glTexParameteri(_target1D, GL_TEXTURE_MAG_FILTER, _base.magFilter());

    glTexImage1D(_target1D,
                 _base.level(),
                 _base.internalFormat(),
                 _width,
                 _base.border(),
                 _base.format(),
                 _base.type(),
                 data);

    unbind();
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
