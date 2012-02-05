// -----------------------------------------------------------------------------
//
// NglShader.h
//
// Class for OpenGL shaders.
//
// Copyright (c) 2011 Exotic Matter AB.  All rights reserved. 
//
// This file is part of Open Naiad Studio.
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
//
// -----------------------------------------------------------------------------

#ifndef NGL_SHADER_H
#define NGL_SHADER_H

#include "NglNonCopyable.h"
#include "NglExtensions.h"

#include <Ni.h>


namespace Ngl
{
// -----------------------------------------------------------------------------
//
// T should be GL_VERTEX_SHADER or GL_FRAGMENT_SHADER. If not, _handle
// will be zero and the CTOR will throw.
//
// TODO: NG_WARNING should be changed to NG_THROW in some cases.
//

template<GLenum T>
class Shader : private NonCopyable
{
public:     // Static interface

    static const GLenum type = T;

public:     // Interface

    explicit
    Shader(const NtString& fname)
        : _handle(glCreateShader(T))
    {                
        if (0 == _handle) {
            NB_THROW("Invalid Shader handle: '" << fname << "':" << _handle);
        }

        // Valid handle, read shader source from file

        NtString src;
        read(fname, &src);

        if (src.empty()) {
            NB_THROW(
                "Empty Shader Source: '" << fname << "' [" << _handle << "]"
                );
        }

        // Non-empty shader source

        const GLchar* src0 = &src[0];
        glShaderSource(_handle, 1, &src0, 0);

        // Compile shader

        glCompileShader(_handle);

        NtString info;
        if (0 < infoLog(&info) ) {
            // Display log contents, if any. A non-empty log is not
            // a critical error and should be a warning.

            NB_WARNING(
                "Shader Info Log: '" << fname
                    << "' [" << _handle << "]: " << info
                    );
        }

        if (!compiled()) {
            // Compilation failed. Ideally we would throw when this
            // happens, but it is left as a warning for now.

            NB_WARNING(
                "Error Compiling Shader: '" << fname << "' [" << _handle << "]"
                );
        }
    };

    ~Shader()
    {
        // Free handle resource.

        glDeleteShader(_handle);
    }

    GLuint handle() const { return _handle; }

    bool
    compiled() const
    {
        GLint status;
        glGetShaderiv(_handle, GL_COMPILE_STATUS, &status);
        return (GL_TRUE == status);
    }

    GLsizei
    infoLog(NtString* info) const
    {
        GLint   maxLength(0);
        GLsizei logLength(0);

        glGetShaderiv(_handle, GL_INFO_LOG_LENGTH, &maxLength);

        if (0 < maxLength && 0 != info) {
            // Info exists. The returned string is null-terminated.

            info->resize(maxLength);
            glGetShaderInfoLog(_handle,
                               static_cast<GLsizei>(maxLength),
                               &logLength,
                               &(*info)[0]);
        }

        // Return number of characters (exluding null-termination)
        // written to log-string. Returns zero if log-string is null.

        return logLength;
    }


private:

    static
    void
    read(const NtString& fname, NtString* src)
    {
        if (0 == src) {
            return;
        }

        FILE *fptr(std::fopen(fname.c_str(), "r"));
        if(!fptr) {
            NB_THROW("Shader file '" << fname << "' does not exist");
        }

        std::fseek(fptr, 0, SEEK_END);
        const int length(std::ftell(fptr));
        src->resize(length + 1);
        std::fseek(fptr, 0, SEEK_SET);
        std::fread(&(*src)[0], length, 1, fptr);
        std::fclose(fptr);
        (*src)[length] = 0;     // Null-termination
    }

private:    // Member variables.

    GLuint _handle;

private:    // Disabled.

    Shader();
};


typedef Shader<GL_VERTEX_SHADER>   VtxShader;
typedef Shader<GL_FRAGMENT_SHADER> FragShader;

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_SHADER_H
