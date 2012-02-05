// -----------------------------------------------------------------------------
//
// NglMaterial.h
//
// Fixed pipeline OpenGL material parameters.
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

#ifndef NGL_MATERIAL_H
#define NGL_MATERIAL_H

#include "NglTypes.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif


namespace Ngl
{
// -----------------------------------------------------------------------------

class Material
{
public:

    explicit
    Material(const vec4f   &amb   = vec4f(0.2f, 0.2f, 0.2f, 1.f),
             const vec4f   &diff  = vec4f(0.8f, 0.8f, 0.8f, 1.f),
             const vec4f   &spec  = vec4f(0.f , 0.f , 0.f , 1.f),
             const GLfloat  shine = 0.f,
             const GLenum   f     = GL_FRONT)
        : ambient(amb),
          diffuse(diff),
          specular(spec),
          shininess(shine),
          face(f)
    {}

    void
    set() const
    {
        glMaterialfv(face, GL_AMBIENT,  &ambient[0]);
        glMaterialfv(face, GL_DIFFUSE,  &diffuse[0]);
        glMaterialfv(face, GL_SPECULAR, &specular[0]);
        glMaterialf(face, GL_SHININESS, shininess);
    }

    // The data is the interface

    vec4f ambient;
    vec4f diffuse;
    vec4f specular;
    GLfloat shininess;
    GLenum face;
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_MATERIAL_H
