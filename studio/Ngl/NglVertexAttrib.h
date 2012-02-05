// -----------------------------------------------------------------------------
//
// NglVertexAttrib.h
//
// Interface for managing vertex attributes.
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

#ifndef NGL_VERTEX_ATTRIB_H
#define NGL_VERTEX_ATTRIB_H

#include "NglNonConstructible.h"
#include "NglShaderAttrib.h"
#include "NglVertexBuffer.h"
#include "NglTypes.h"
#include "NglError.h"
#include "NglExtensions.h"

#include <em_array1.h>
#include <em_block3_array.h>

#include <map>

namespace Ngl
{
// -----------------------------------------------------------------------------
//
// Possible queries through OpenGL. It might be useful to add these
// to the interface at some stage.
//
// void glGetVertexAttribiv(const GLuint index,
//                          const GLenum pname,
//                          GLint * 	 params);
// pnames:
//     GL_VERTEX_ATTRIB_ARRAY_BUFFER_BINDING
//     GL_VERTEX_ATTRIB_ARRAY_ENABLED
//     GL_VERTEX_ATTRIB_ARRAY_SIZE
//     GL_VERTEX_ATTRIB_ARRAY_STRIDE
//     GL_VERTEX_ATTRIB_ARRAY_TYPE
//     GL_VERTEX_ATTRIB_ARRAY_NORMALIZED
//     GL_CURRENT_VERTEX_ATTRIB
//

class VertexAttrib : private NonConstructible
{
public:     // Interface

    static void createTypeMap();
    static void destroyTypeMap();


    static GLsizei connect(const ShaderAttrib& shaderAttrib,
                           const VertexBuffer& vertexBuffer,
                           GLboolean           normalized = GL_FALSE,
                           GLsizei             stride     = 0);

    static void disconnect(const ShaderAttrib& shaderAttrib);

public:    // Conversion between shader attribute and vertex attribute types

    class _Type
    {
    public:     // Interface.

        explicit
        _Type(GLenum type, GLint size);

        GLenum type() const;
        GLint  size() const;

    private:    // Member variables.

        GLenum _type;   // Number of components, {1,2,3,4}.
        GLint  _size;   // GL_FLOAT, etc. See _validType().

    private:    // Utility functions.

        static GLenum _validType(GLenum type);
        static GLint  _validSize(GLint size);

    private:    // Disabled.

        _Type();
    };

private:

    // Key:   Shader attribute type.
    // Value: <Vertex attribute type, number of channels>

    typedef std::map<GLenum, _Type> _TypeMap;
    static _TypeMap* _typeMap;

private:    // Utility functions.

    static GLuint  _validIndex(GLint location);
    static GLsizei _validStride(GLsizei stride);
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_VERTEX_ATTRIB_H
