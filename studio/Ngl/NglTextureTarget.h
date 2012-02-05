// -----------------------------------------------------------------------------
//
// NglTextureTarget.h
//
// Map dimension number to texture target.
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

#ifndef NGL_TEXTURE_TARGET_H
#define NGL_TEXTURE_TARGET_H

#include "NglExtensions.h"


namespace Ngl
{
// -----------------------------------------------------------------------------

// Map dimension number to texture target.
// Only dimensions {1,2,3} supported, other numbers give linker error.

template<GLint D>
class TextureTarget;    // Generic template, not implemented.


template<>
class TextureTarget<1>
{
public:

    static const GLenum target  = GL_TEXTURE_1D;
    static const GLenum proxy   = GL_PROXY_TEXTURE_1D;
    static const GLenum maxSize = GL_MAX_TEXTURE_SIZE;
};


template<>
class TextureTarget<2>
{
public:

    static const GLenum target  = GL_TEXTURE_2D;
    static const GLenum proxy   = GL_PROXY_TEXTURE_2D;
    static const GLenum maxSize = GL_MAX_TEXTURE_SIZE;
};


template<>
class TextureTarget<3>
{
public:

    static const GLenum target  = GL_TEXTURE_3D;
    static const GLenum proxy   = GL_PROXY_TEXTURE_3D;
    static const GLenum maxSize = GL_MAX_3D_TEXTURE_SIZE;
};


typedef TextureTarget<1> TextureTarget1D;
typedef TextureTarget<2> TextureTarget2D;
typedef TextureTarget<3> TextureTarget3D;

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_TEXTURE_TARGET_H
