// -----------------------------------------------------------------------------
//
// NglShaderUniform.h
//
// Class for uniform variables in shader programs.
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

#ifndef NGL_SHADER_UNIFORM_H
#define NGL_SHADER_UNIFORM_H

#include "NglShaderUniformBase.h"
#include "NglExtensions.h"
#include "NglError.h"

#include <NbValue.h>

namespace Ngl
{
// -----------------------------------------------------------------------------



// GL_INVALID_OPERATION is generated if there is no current program object.
//
// GL_INVALID_OPERATION is generated if the size of the uniform variable
//                      declared in the shader does not match the size
//                      indicated by the glUniform command.
//
// GL_INVALID_OPERATION is generated if one of
//            the integer variants of this function is used to load a uniform
//            variable of type float, vec2, vec3, vec4, or an array of these,
//            or if one of the floating-point variants of this function is
//            used to load a uniform variable of type int, ivec2, ivec3, or
//            ivec4, or an array of these.
//
//    GL_INVALID_OPERATION is generated if
//            location is an invalid uniform location
//            for the current program object and
//            location is not equal to -1.
//
//    GL_INVALID_VALUE is generated if count is less than 0.
//
//    GL_INVALID_OPERATION is generated if
//            count is greater than 1 and the indicated
//            uniform variable is not an array variable.
//
//    GL_INVALID_OPERATION is generated if a
//            sampler is loaded using a command other than
//            glUniform1i and
//            glUniform1iv.

template<typename T, unsigned int N, unsigned int VN>
class ShaderUniform : public ShaderUniformBase
{
public:     // Interface

    ShaderUniform(const NtString& name, 
                  GLenum            type, 
                  GLint             size, 
                  GLint             location)
        : ShaderUniformBase(name, type, size, location),
          _value(0)
    {
        for(unsigned int i(0); i < N; ++i)
            _constant[i] = 0;
    }

    // Store a copy of a constant value in this uniform.

    void
    store(const T* vptr)
    { 
        _value=0;
        for(unsigned int i(0); i < N; ++i)
            _constant[i] = vptr[i];
    }

    void
    store(const T constant, const int component=0)
    { 
        if (component > static_cast<int>(N))
            NB_THROW("Component out of bounds");

        _value = 0;
        _constant[component] = constant;
    }

    // Bind external Nb::Value to this uniform.

    void
    bind(Nb::Value<T,VN>* value)
    {
        _value=value;
    }

    // Upload constant value to shader, or evaluated value at given time
    // and upload that to shader.

    virtual void
    upload(const Nb::TimeBundle& tb);

private:    // Member variables.

    Nb::Value<T,VN>* _value;
    T                _constant[N];

private:    // Disabled.

    ShaderUniform();
};

// -----------------------------------------------------------------------------

template<> inline void
ShaderUniform<float,1,1>::upload(const Nb::TimeBundle& tb)
{
    EM_ASSERT(Error::check());
    if(_value) {
        glUniform1f(
            location(),
            _value->eval(tb)
            );
    } else {
        glUniform1f(location(), _constant[0]);
    }
    EM_ASSERT(Error::check());
}

// -----------------------------------------------------------------------------

template<> inline void
ShaderUniform<float,3,3>::upload(const Nb::TimeBundle& tb)
{
    EM_ASSERT(Error::check());
    if(_value) {
        glUniform3f(
            location(), 
            _value->eval(tb,0),
            _value->eval(tb,1),
            _value->eval(tb,2)
            );
    } else {
        glUniform3f(
            location(), 
            _constant[0],
            _constant[1],
            _constant[2]
            );            
    }
    EM_ASSERT(Error::check());
}

// -----------------------------------------------------------------------------

template<> inline void
ShaderUniform<float,4,4>::upload(const Nb::TimeBundle& tb)
{
    EM_ASSERT(Error::check());
    if(_value) {
        glUniform4f(
            location(), 
            _value->eval(tb,0),
            _value->eval(tb,1),
            _value->eval(tb,2),
            _value->eval(tb,3)
            );
    } else {
        glUniform4fv(location(), 1, _constant);
    }
    EM_ASSERT(Error::check());
}

// -----------------------------------------------------------------------------

template<> inline void
ShaderUniform<float,4,3>::upload(const Nb::TimeBundle& tb)
{
    EM_ASSERT(Error::check());
    if(_value) {
        glUniform4f(
            location(), 
            _value->eval(tb,0),
            _value->eval(tb,1),
            _value->eval(tb,2),
            1
            );
    } else {
        glUniform4fv(location(), 1, _constant);
    }
    EM_ASSERT(Error::check());
}

// -----------------------------------------------------------------------------

template<> inline void
ShaderUniform<int,1,1>::upload(const Nb::TimeBundle& tb)
{
    EM_ASSERT(Error::check());
    if(_value) {
        glUniform1i(
            location(),
            _value->eval(tb)
            );
    } else {
        glUniform1i(location(), _constant[0]);        
    }
    EM_ASSERT(Error::check());
}

// -----------------------------------------------------------------------------

template<> inline void
ShaderUniform<int,3,3>::upload(const Nb::TimeBundle& tb)
{
    EM_ASSERT(Error::check());
    if(_value)
        glUniform3i(
            location(), 
            _value->eval(tb,0),
            _value->eval(tb,1),
            _value->eval(tb,2)
            );
    else
        glUniform3i(
            location(), 
            _constant[0],
            _constant[1],
            _constant[2]
            );
    EM_ASSERT(Error::check());
}

// -----------------------------------------------------------------------------

template<> inline void
ShaderUniform<int,4,4>::upload(const Nb::TimeBundle& tb)
{
    EM_ASSERT(Error::check());
    if(_value)
        glUniform4i(
            location(), 
            _value->eval(tb,0),
            _value->eval(tb,1),
            _value->eval(tb,2),
            _value->eval(tb,3)
            );
    else
        glUniform4iv(location(), 1, _constant);
    EM_ASSERT(Error::check());
}

// -----------------------------------------------------------------------------

template<> inline void
ShaderUniform<float,9,9>::upload(const Nb::TimeBundle& tb)
{
    EM_ASSERT(Error::check());
    if(_value)
        NB_THROW("Value9f not supported!");
    glUniformMatrix3fv(location(), 1, transpose(), _constant);
    EM_ASSERT(Error::check());
}

// -----------------------------------------------------------------------------

template<> inline void
ShaderUniform<float,16,16>::upload(const Nb::TimeBundle& tb)
{
    EM_ASSERT(Error::check());
    if(_value)
        NB_THROW("Value16f not supported!");
    glUniformMatrix4fv(location(), 1, transpose(), _constant);
    EM_ASSERT(Error::check());
}

// -----------------------------------------------------------------------------

typedef ShaderUniform<float,1,1>   ShaderUniform1f;
typedef ShaderUniform<float,3,3>   ShaderUniform3f;
typedef ShaderUniform<float,4,3>   ShaderUniform43f;
typedef ShaderUniform<float,4,4>   ShaderUniform4f;

typedef ShaderUniform<int,1,1>     ShaderUniform1i;
typedef ShaderUniform<int,3,3>     ShaderUniform3i;
typedef ShaderUniform<int,4,4>     ShaderUniform4i;

typedef ShaderUniform<float,9,9>   ShaderUniform3m;
typedef ShaderUniform<float,16,16> ShaderUniform4m;

// -----------------------------------------------------------------------------
}   // Namespace: Ngl

#endif  // NGL_SHADER_UNIFORM_H
