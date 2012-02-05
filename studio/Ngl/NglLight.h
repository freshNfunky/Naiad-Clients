// -----------------------------------------------------------------------------
//
// NglLight.h
//
// Fixed pipeline OpenGL light source parameters.
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

#ifndef NGL_LIGHT_H
#define NGL_LIGHT_H

#include "NglTypes.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#endif

#ifdef LINUX
#include <GL/gl.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <gl/GL.h>
#include <gl/glext.h>
#include <gl/wglext.h>
#endif

namespace Ngl
{
// -----------------------------------------------------------------------------

class Light
{
public:

    explicit
    Light(const vec4f   &amb         = vec4f(0.f, 0.f, 0.f, 1.f),
          const vec4f   &diff        = vec4f(1.f, 1.f, 1.f, 1.f),
          const vec4f   &spec        = vec4f(1.f, 1.f, 1.f, 1.f),
          const vec4f   &pos         = vec4f(0.f, 0.f, 1.f, 0.f),
          const vec3f   &spotDir     = vec3f(0.f, 0.f, -1.f),
          const GLfloat  spotExp     = 0.f,
          const GLfloat  spotCut     = 180.f,
          const vec3f   &constAtt    = vec3f(1.f, 0.f, 0.f),
          const vec3f   &linearAtt   = vec3f(1.f, 0.f, 0.f),
          const vec3f   &quadAtt     = vec3f(1.f, 0.f, 0.f),
          const GLenum   i           = GL_LIGHT0)
        : ambient(amb),
          diffuse(diff),
          specular(spec),
          position(pos),
          spotDirection(spotDir),
          spotExponent(spotExp),
          spotCutoff(spotCut),
          constAttenuation(constAtt),
          linearAttenuation(linearAtt),
          quadAttenuation(quadAtt),
          index(i)
    {}

    void
    set() const
    {
        glLightfv(index, GL_AMBIENT,                &ambient[0]          );
        glLightfv(index, GL_DIFFUSE,                &diffuse[0]          );
        glLightfv(index, GL_SPECULAR,               &specular[0]         );
        glLightfv(index, GL_POSITION,               &position[0]         );
        glLightfv(index, GL_SPOT_DIRECTION,         &spotDirection[0]    );
        glLightf( index, GL_SPOT_EXPONENT,           spotExponent        );
        glLightf( index, GL_SPOT_CUTOFF,             spotCutoff          );
        glLightfv(index, GL_CONSTANT_ATTENUATION,   &constAttenuation[0] );
        glLightfv(index, GL_LINEAR_ATTENUATION,     &linearAttenuation[0]);
        glLightfv(index, GL_QUADRATIC_ATTENUATION,  &quadAttenuation[0]  );
    }

    // The data is the interface

    vec4f ambient;
    vec4f diffuse;
    vec4f specular;
    vec4f position;
    vec3f spotDirection;
    GLfloat spotExponent;
    GLfloat spotCutoff;
    vec3f constAttenuation;
    vec3f linearAttenuation;
    vec3f quadAttenuation;
    GLenum index;
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_LIGHT_H
