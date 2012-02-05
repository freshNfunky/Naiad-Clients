// -----------------------------------------------------------------------------
//
// NglTypes.h
//
// Convenient types
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

#ifndef NGL_TYPES_H
#define NGL_TYPES_H

#include "NglInclude.h"
#include <Ni.h>
#include <em_vec.h>
#include <em_glmat44.h>

// -----------------------------------------------------------------------------

namespace Ngl
{

typedef em::vec<2, GLint>    vec2i;
typedef em::vec<2, GLfloat>  vec2f;
typedef em::vec<3, GLint>    vec3i;
typedef em::vec<3, GLfloat>  vec3f;
typedef em::vec<3, GLdouble> vec3d;
typedef em::vec<4, GLint>    vec4i;
typedef em::vec<4, GLfloat>  vec4f;
typedef em::vec<4, GLubyte>  vec4uc;

typedef em::glmat44<GLfloat> mat44f;


class GLType
{
public:

    static
    NtString
    str(const GLenum type)
    {
        switch (type)
        {
        case GL_BYTE:
            return NtString("GL_BYTE");
        case GL_UNSIGNED_BYTE:
            return NtString("GL_UNSIGNED_BYTE");
        case GL_SHORT:
            return NtString("GL_SHORT");
        case GL_UNSIGNED_SHORT:
            return NtString("GL_UNSIGNED_SHORT");
        case GL_INT:
            return NtString("GL_INT");
        case GL_UNSIGNED_INT:
            return NtString("GL_UNSIGNED_INT");
        case GL_FLOAT:
            return NtString("GL_FLOAT");
        case GL_FLOAT_VEC2:
            return NtString("GL_FLOAT_VEC2");
        case GL_FLOAT_VEC3:
            return NtString("GL_FLOAT_VEC3");
        case GL_FLOAT_VEC4:
            return NtString("GL_FLOAT_VEC4");
        case GL_FLOAT_MAT2:
            return NtString("GL_FLOAT_MAT2");
        case GL_FLOAT_MAT3:
            return NtString("GL_FLOAT_MAT3");
        case GL_FLOAT_MAT4:
            return NtString("GL_FLOAT_MAT4");
//        case GL_FLOAT_MAT2x3:
//            return NtString("GL_FLOAT_MAT2x3");
//        case GL_FLOAT_MAT2x4:
//            return NtString("GL_FLOAT_MAT2x4");
//        case GL_FLOAT_MAT3x2:
//            return NtString("GL_FLOAT_MAT3x2");
//        case GL_FLOAT_MAT3x4:
//            return NtString("GL_FLOAT_MAT3x4");
//        case GL_FLOAT_MAT4x2:
//            return NtString("GL_FLOAT_MAT4x2");
//        case GL_FLOAT_MAT4x3:
//            return NtString("GL_FLOAT_MAT4x3");
        case GL_DOUBLE:
            return NtString("GL_DOUBLE");
//        case GL_SAMPLER_1D:
//            return NtString("GL_SAMPLER_1D");
        case GL_SAMPLER_2D:
            return NtString("GL_SAMPLER_2D");
//        case GL_SAMPLER_3D:
//            return NtString("GL_SAMPLER_3D");
        default:
            return NtString("GL_UNKNOWN");
        }
    }

    static
    GLuint
    size(const GLenum type)
    {
        switch (type)
        {
        case GL_BYTE:
            return sizeof(GLbyte);
        case GL_UNSIGNED_BYTE:
            return sizeof(GLubyte);
        case GL_SHORT:
            return sizeof(GLshort);
        case GL_UNSIGNED_SHORT:
            return sizeof(GLushort);
        case GL_INT:
            return sizeof(GLint);
        case GL_UNSIGNED_INT:
            return sizeof(GLuint);
        case GL_FLOAT:
            return sizeof(GLfloat);
        case GL_FLOAT_VEC2:
            return 2*sizeof(GLfloat);
        case GL_FLOAT_VEC3:
            return 3*sizeof(GLfloat);
        case GL_FLOAT_VEC4:
            return 4*sizeof(GLfloat);
        case GL_FLOAT_MAT2:
            return 2*2*sizeof(GLfloat);
        case GL_FLOAT_MAT3:
            return 3*3*sizeof(GLfloat);
        case GL_FLOAT_MAT4:
            return 4*4*sizeof(GLfloat);
        case GL_FLOAT_MAT2x3:
            return 2*3*sizeof(GLfloat);
        case GL_FLOAT_MAT2x4:
            return 2*4*sizeof(GLfloat);
        case GL_FLOAT_MAT3x2:
            return 3*2*sizeof(GLfloat);
        case GL_FLOAT_MAT3x4:
            return 3*4*sizeof(GLfloat);
        case GL_FLOAT_MAT4x2:
            return 4*2*sizeof(GLfloat);
        case GL_FLOAT_MAT4x3:
            return 4*3*sizeof(GLfloat);
        case GL_DOUBLE:
            return sizeof(GLdouble);
        default:
            NB_WARNING("Unknown GL type");
            return 1;
            //NB_THROW("Unknown GL type: " << type);
        }
    }
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_TYPES_H
