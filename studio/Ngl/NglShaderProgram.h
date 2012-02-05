// -----------------------------------------------------------------------------
//
// NglShaderProgram.h
//
// Class for OpenGL shader programs.
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

#ifndef NGL_SHADER_PROGRAM_H
#define NGL_SHADER_PROGRAM_H

#include "NglNonCopyable.h"
#include "NglShader.h"
#include "NglShaderAttrib.h"
#include "NglShaderUniform.h"

#include <Ni.h>
#include <NbValueType.h>

#include <em_array1.h>

#include <map>


namespace Ngl
{
// -----------------------------------------------------------------------------
//
// TODO: NG_WARNING could be changed to NG_THROW in some cases.

class ShaderProgram : private NonCopyable
{
public:     // Interface

    typedef std::map<NtString, ShaderAttrib>       AttribMap;
    typedef std::map<NtString, ShaderUniformBase*> UniformMap;

    explicit
    ShaderProgram(const NtString& vtxShaderPath,
                  const NtString& fragShaderPath);

    ~ShaderProgram();


    const VtxShader&  vtxShader()  const;
    const FragShader& fragShader() const;

    const AttribMap&  attribMap()  const;
    const UniformMap& uniformMap() const;


    bool linked() const;
    bool valid()  const;

    GLsizei infoLog(NtString* info) const;

    void use()   const;
    void unuse() const;

    void uploadUniforms(const Nb::TimeBundle& tb) const;
    
    // Attributes

    const ShaderAttrib* queryConstAttrib(const NtString& name) const;
    const ShaderAttrib& constAttrib(const NtString& name) const;

    // Uniforms

    ShaderUniformBase* queryMutableUniformBase(const NtString& name);

    // Bind uniforms to external Nb::Values, where the evaluation is delayed
    // until the upload occurs
    
    void
    bindUniform1f(const NtString& name, Nb::Value1f* value)
    { _bindUniform<float,1,1>(name,value); }

    void
    bindUniform3f(const NtString& name, Nb::Value3f* value)
    { _bindUniform<float,3,3>(name,value); }

    void
    bindUniform4f(const NtString& name, Nb::Value3f* value)
    { _bindUniform<float,4,3>(name,value); }

    void
    bindUniform4f(const NtString& name, Nb::Value4f* value)
    { _bindUniform<float,4,4>(name,value); }

    void
    bindUniform1i(const NtString& name, Nb::Value1i* value)
    { _bindUniform<int,1,1>(name,value); }

    void
    bindUniform3i(const NtString& name, Nb::Value3i* value)
    { _bindUniform<int,3,3>(name,value); }

    void
    bindUniform4i(const NtString& name, Nb::Value3i* value)
    { _bindUniform<int,4,3>(name,value); }

    void
    bindUniform4i(const NtString& name, Nb::Value4i* value)
    { _bindUniform<int,4,4>(name,value); }

    // Immediately copy values to the store in the uniforms; used during
    // later upload.
    
    void
    storeUniform1f(const NtString& name, const float value)
    { _storeUniform<float,1>(name,value); }

    void
    storeUniform3f(const NtString& name, const NtVec3f& value)
    { _storeUniform<float,3>(name,&value[0]); }

    void
    storeUniform3f(const NtString& name, const float* values)
    { _storeUniform<float,3>(name,values); }

    void
    storeUniform4f(const NtString& name, const Nb::Vec4f& value)
    { _storeUniform<float,4>(name,&value[0]); }

    void
    storeUniform4f(const NtString& name, const float* values)
    { _storeUniform<float,4>(name,values); }

    void
    storeUniform1i(const NtString& name, const int value)
    { _storeUniform<int,1>(name,value); }

    void
    storeUniform3i(const NtString& name, const NtVec3i& value)
    { _storeUniform<int,3>(name,&value[0]); }

    void
    storeUniform3i(const NtString& name, const int* values)
    { _storeUniform<int,3>(name,values); }

    void
    storeUniform4i(const NtString& name, const Nb::Vec4i& value)
    { _storeUniform<int,4>(name,&value[0]); }

    void
    storeUniform4i(const NtString& name, const int* values)
    { _storeUniform<int,4>(name,values); }

    void
    storeUniform3m(const NtString& name, const float* values)
    { _storeUniform<float,9>(name,values); }

    void
    storeUniform4m(const NtString& name, const float* values)
    { _storeUniform<float,16>(name,values); }

private:        // Member variables.

    GLuint     _handle;
    VtxShader  _vtxShader;
    FragShader _fragShader;
    AttribMap  _attribMap;
    UniformMap _uniformMap;    

private:        // Utility functions.

    template<typename T, unsigned int N, unsigned int VN> ShaderUniform<T,N,VN>*
    _typed_uniform_ptr(const NtString& name)
    {
        ShaderUniformBase* sub=queryMutableUniformBase(name);
        if(!sub) return 0;
        ShaderUniform<T,N,VN>* ptr=dynamic_cast<ShaderUniform<T,N,VN>*>(sub);
        // auto-create ShaderUniform in case we have fed it a value of another
        // dimension (e.g a Value3f to a ShaderUniform4f).
        if(!ptr && N!=VN) {
            ShaderUniform<T,N,N>* ptr2=dynamic_cast<ShaderUniform<T,N,N>*>(sub);
            if(ptr2) {
                ptr=new ShaderUniform<T,N,VN>(
                    ptr2->name(),
                    ptr2->type(),
                    ptr2->size(),
                    ptr2->location()
                    );
                _uniformMap.erase(ptr2->name());
                delete ptr2;
                _uniformMap.insert(UniformMap::value_type(ptr->name(),ptr));
            }
        }
        if(!ptr) {
            NB_THROW("Type mismatch for uniform shader param '" << name << "'");
        }
        return ptr;
    }

    template<typename T, unsigned int N, unsigned int VN> void
    _bindUniform(const NtString& name, Nb::Value<T,VN>* value)
    { 
        ShaderUniform<T,N,VN>* su=_typed_uniform_ptr<T,N,VN>(name);
        if(su) su->bind(value);
    }

    template<typename T, unsigned int N> void
    _storeUniform(const NtString& name, const T* value)
    { 
        ShaderUniform<T,N,N>* su=_typed_uniform_ptr<T,N,N>(name);
        if(su) su->store(value);
    }

    template<typename T, unsigned int N> void
    _storeUniform(const NtString& name, const T value)
    { 
        ShaderUniform<T,N,N>* su=_typed_uniform_ptr<T,N,N>(name);
        if(su) su->store(value);
    }
    
    void _link();

private:        // Disabled.

    ShaderProgram();
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_SHADER_PROGRAM_H
