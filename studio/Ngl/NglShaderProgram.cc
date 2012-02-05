// -----------------------------------------------------------------------------
//
// NglShaderProgram.cc
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

#include "NglShaderProgram.h"


namespace Ngl
{
// -----------------------------------------------------------------------------

// ShaderProgram
// -------------
//! Constructor.

ShaderProgram::ShaderProgram(const NtString& vtxShaderPath,
                             const NtString& fragShaderPath)
    : _handle(glCreateProgram()),
      _vtxShader(vtxShaderPath),    // May throw
      _fragShader(fragShaderPath)   // May throw
{
    if (0 == _handle) {
        NB_THROW("Invalid Shader Program handle: " << _handle);
    }

    glAttachShader(_handle, _vtxShader.handle());
    glAttachShader(_handle, _fragShader.handle());
    _link();
}


// ~ShaderProgram
// --------------
//! Destructor.

ShaderProgram::~ShaderProgram()
{
    UniformMap::const_iterator iter(_uniformMap.begin());
    for(; iter!=_uniformMap.end(); iter++)
        delete iter->second;
    
    glDetachShader(_handle, _vtxShader.handle());
    glDetachShader(_handle, _fragShader.handle());

    // Free handle resource.

    glDeleteProgram(_handle);
}


// vtxShader
// ---------
//! Return vertex shader.

const VtxShader&
ShaderProgram::vtxShader() const
{
    return _vtxShader;
}


// fragShader
// ----------
//! Return fragment shader.

const FragShader&
ShaderProgram::fragShader() const
{
    return _fragShader;
}


// attribMap
// ---------
//! Return attribute map.

const ShaderProgram::AttribMap&
ShaderProgram::attribMap() const
{
    return _attribMap;
}


// uniformMap
// ----------
//! Return uniform map.

const ShaderProgram::UniformMap&
ShaderProgram::uniformMap() const
{
    return _uniformMap;
}


// linked
// ------
//! Returns true if shader program has been successfully linked.

bool
ShaderProgram::linked() const
{
    GLint status(GL_FALSE);
    glGetProgramiv(_handle, GL_LINK_STATUS, &status);
    return (GL_TRUE == status);
}


// valid
// -----
//! Returns true if shader program is successfully validated.

bool
ShaderProgram::valid() const
{
    GLint status(GL_FALSE);
    glValidateProgram(_handle);
    glGetProgramiv(_handle, GL_VALIDATE_STATUS, &status);
    return (GL_TRUE == status);
}


// infoLog
// -------
//! Retrieve information log for shader program. Returns the number of
//! characters in this log.

GLsizei
ShaderProgram::infoLog(NtString* info) const
{
    GLsizei logLength(0);

    GLint   maxLength(0);
    glGetProgramiv(_handle, GL_INFO_LOG_LENGTH, &maxLength);

    if (0 < maxLength && 0 != info) {
        // Info exists. The returned string is null-terminated.

        info->str().resize(maxLength);
        glGetProgramInfoLog(_handle,
                            static_cast<GLsizei>(maxLength),
                            &logLength,
                            &(*info)[0]);
    }

    // Return number of characters (exluding null-termination)
    // written to log-string. Returns zero if info-string is null.

    return logLength;
}


// use
// ---
//! Activate the shader program.

void
ShaderProgram::use() const
{
    glUseProgram(_handle);    
}


// uploadUniforms
// --------------
//! Upload all uniform variables to the shader

void
ShaderProgram::uploadUniforms(const Nb::TimeBundle& tb) const
{
    UniformMap::const_iterator iter(_uniformMap.begin());
    for(; iter != _uniformMap.end(); ++iter)
        iter->second->upload(tb);
}


// unuse
// -----
//! Deactivate the shader program.

void
ShaderProgram::unuse() const
{
    glUseProgram(0);
}


// queryConstAttrib
// ----------------
//! Returns a pointer to a shader attribute.

const ShaderAttrib*
ShaderProgram::queryConstAttrib(const NtString& name) const
{
    const AttribMap::const_iterator iter(_attribMap.find(name));

    if (iter != _attribMap.end()) {
        return &iter->second;
    }

    return 0;   // Not found, return null.
}


// constAttrib
// ----------------
//! Returns a reference to a shader attribute. If no existing attribute
//! matches name we throw.

const ShaderAttrib&
ShaderProgram::constAttrib(const NtString& name) const
{
    const ShaderAttrib* attrib(queryConstAttrib(name));

    if (0 == attrib) {
        NB_THROW("Invalid shader attribute: '" << name << "'");
    }

    return *attrib;
}


// queryMutableUniform
// -------------------
//! Returns a pointer to a shader uniform.

ShaderUniformBase*
ShaderProgram::queryMutableUniformBase(const NtString& name)
{
    UniformMap::iterator iter(_uniformMap.find(name));

    if (iter != _uniformMap.end()) {
        return iter->second;
    }

    return 0;   // Not found, return null.
}


// _link
// ----
//! Link the shader program.

void
ShaderProgram::_link()
{
    glLinkProgram(_handle);

    NtString info;
    if (0 < infoLog(&info) ) {
        // Display log contents, if any. A non-empty log is not
        // a critical error and should be a warning.

        NB_WARNING(
            "Shader Program Info Log: [" << _handle << "]: " << info
            );
    }

    if (!linked()) {
        // Linking failed. Ideally we would throw when this
        // happens, but it is left as a warning for now.

        NB_WARNING("Error linking shader program: [" << _handle << "]");
    }

    if (!valid()) {
        // Validation failed. Ideally we would throw when this
        // happens, but it is left as a warning for now.
        
        NB_WARNING(
            "Error validating shader program on this hardware: ["
                << _handle << "]"
                  );
    }

    // Retrieve information about shader parameters

    GLint activeAttribs(0);
    glGetProgramiv(_handle, GL_ACTIVE_ATTRIBUTES, &activeAttribs);

    GLint activeAttribMaxLength(0);
    glGetProgramiv(_handle,
                   GL_ACTIVE_ATTRIBUTE_MAX_LENGTH,
                   &activeAttribMaxLength);

    GLsizei    len(0);
    GLint      size(0);
    GLenum     type(GL_FLOAT);
    NtString name;

    _attribMap.clear();

    for(GLint a = 0; a < activeAttribs; ++a) {
        // Get attribute information.

        name.resize(activeAttribMaxLength);
        glGetActiveAttrib(
            _handle,
            static_cast<GLuint>(a),
            activeAttribMaxLength,
            &len,
            &size,
            &type,
            &name[0]);
        name.resize(len);

        const GLint loc(glGetAttribLocation(_handle, name.c_str()));

        // Add attribute to map

        _attribMap.insert(
            AttribMap::value_type(name, ShaderAttrib(type, size, loc)));
    }

    GLint activeUniforms(0);
    glGetProgramiv(_handle, GL_ACTIVE_UNIFORMS, &activeUniforms);

    GLint activeUniformMaxLength(0);
    glGetProgramiv(_handle,
                   GL_ACTIVE_UNIFORM_MAX_LENGTH,
                   &activeUniformMaxLength);

    _uniformMap.clear();

    for(GLint u = 0; u < activeUniforms; ++u) {
        // Get uniform information.

        name.resize(activeUniformMaxLength);
        glGetActiveUniform(
            _handle,
            static_cast<GLuint>(u),
            activeUniformMaxLength,
            &len,
            &size,
            &type,
            &name[0]);
        name.resize(len);

        const GLint loc(glGetUniformLocation(_handle, name.c_str()));

        // Add uniform to map

        ShaderUniformBase* uniform(0);

        switch(type) {
            case GL_FLOAT: 
                uniform = new ShaderUniform1f(name,type,size,loc); 
                break;
            case GL_FLOAT_VEC2: 
                NB_THROW("GL_FLOAT_VEC2 uniforms not supported!");
                break;
            case GL_FLOAT_VEC3:
                uniform = new ShaderUniform3f(name,type,size,loc);
                break;
            case GL_FLOAT_VEC4:
                uniform = new ShaderUniform4f(name,type,size,loc);
                break;               
            case GL_INT:
            case GL_SAMPLER_1D:
            case GL_SAMPLER_2D:
            case GL_SAMPLER_3D:
            case GL_SAMPLER_CUBE:
            case GL_SAMPLER_1D_SHADOW:
            case GL_SAMPLER_2D_SHADOW:
                uniform = new ShaderUniform1i(name,type,size,loc);
                break;
            case GL_INT_VEC2:
                NB_THROW("GL_INT_VEC2 uniforms not supported!");
                break;
            case GL_INT_VEC3:
                uniform = new ShaderUniform3i(name,type,size,loc);
                break;
            case GL_INT_VEC4:
                uniform = new ShaderUniform4i(name,type,size,loc);
                break;
            case GL_FLOAT_MAT3:
                uniform = new ShaderUniform3m(name,type,size,loc);
                break;
            case GL_FLOAT_MAT4:
                uniform = new ShaderUniform4m(name,type,size,loc);
                break;
            default:
                NB_THROW("Unknown/Unsupported type for uniform " << name); 
        }
        
        _uniformMap.insert(UniformMap::value_type(name, uniform));
    }
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
