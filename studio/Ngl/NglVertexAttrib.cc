// -----------------------------------------------------------------------------
//
// NglVertexAttrib.cc
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

#include "NglVertexAttrib.h"


namespace Ngl
{
// -----------------------------------------------------------------------------

// Static type map is uninitialized.

std::map<GLenum, VertexAttrib::_Type>* VertexAttrib::_typeMap = 0;


// createTypeMap
// -------------
//! Create static type map.

void
VertexAttrib::createTypeMap()
{
    if (0 == _typeMap) {
        _typeMap = new _TypeMap();

        typedef _TypeMap::value_type _TypeMapValue;
        _typeMap->insert(_TypeMapValue(GL_FLOAT,      _Type(GL_FLOAT, 1)));
        _typeMap->insert(_TypeMapValue(GL_FLOAT_VEC2, _Type(GL_FLOAT, 2)));
        _typeMap->insert(_TypeMapValue(GL_FLOAT_VEC3, _Type(GL_FLOAT, 3)));
        _typeMap->insert(_TypeMapValue(GL_FLOAT_VEC4, _Type(GL_FLOAT, 4)));
    }
}


// createTypeMap
// -------------
//! Create static type map.

void
VertexAttrib::destroyTypeMap()
{
    delete _typeMap;
    _typeMap = 0;
}


// connect
// -------
//! Connect a shader attribute to a vertex buffer.

GLsizei
VertexAttrib::connect(const ShaderAttrib& shaderAttrib,
                      const VertexBuffer& vertexBuffer,
                      const GLboolean     normalized,
                      const GLsizei       stride)
{
    EM_ASSERT(Error::check());

    if (0 == _typeMap) {
        NB_THROW("Uninitialized vertex attribute type map");
    }

    const _TypeMap::const_iterator typeIter(
        _typeMap->find(shaderAttrib.type()));

    if (typeIter == _typeMap->end()) {
        NB_THROW("Unknown shader attribute type");
    }

    const GLuint index(_validIndex(shaderAttrib.location()));

    glEnableVertexAttribArray(index);

    vertexBuffer.bind();              // Bind VBO
    glVertexAttribPointer(index,
                          typeIter->second.size(),
                          typeIter->second.type(),
                          normalized,
                          _validStride(stride),
                          0); // Null, data is read from currently bound VBO

#if 1
    // TODO: Investigate if this is necessary.
    vertexBuffer.unbind();
#endif

    EM_ASSERT(Error::check());

    // Return number of elements in vertex buffer.

    return vertexBuffer.size()
           /
           (GLType::size(typeIter->second.type())*typeIter->second.size());
}


// disconnect
// ----------
//! Disconnect a shader attribute.

void
VertexAttrib::disconnect(const ShaderAttrib& shaderAttrib)
{
    EM_ASSERT(Error::check());

    const GLuint index(_validIndex(shaderAttrib.location()));
    glDisableVertexAttribArray(index);

    EM_ASSERT(Error::check());
}


// VertexAttrib::_Type

// _Type
// -----
//! Construct a valid vertex attribute type.

VertexAttrib::_Type::_Type(const GLenum type, const GLint size)
    : _type(_validType(type)),   // May throw
      _size(_validSize(size))    // May throw
{}


// type
// ----
//! Return type

GLenum
VertexAttrib::_Type::type() const
{
    return _type;
}


// size
// ----
//! Return size

GLint
VertexAttrib::_Type::size() const
{
    return _size;
}


// _validType
// ----------
//! Throws an exception if type is invalid. Returns a valid type.

GLenum
VertexAttrib::_Type::_validType(const GLenum type)
{
#if 1
    if (GL_BYTE   != type && GL_UNSIGNED_BYTE  != type &&
        GL_SHORT  != type && GL_UNSIGNED_SHORT != type &&
        GL_INT    != type && GL_UNSIGNED_INT   != type &&
        GL_FLOAT  != type &&
#ifndef __APPLE__
#ifndef WIN32
        GL_HALF_FLOAT     != type &&
#endif
#endif
        GL_DOUBLE != type) {
        NB_THROW("Invalid vertex attribute type: " << type);
    }
#endif

    return type;
}


// _validSize
// ----------
//! Throws an exception if size is invalid. Returns a valid size.

GLint
VertexAttrib::_Type::_validSize(const GLint size)
{
#if 1
    if (1 > size || size > 4) {
        NB_THROW("Invalid vertex attribute size: " << size);
    }
#endif

    return size;
}


// _validIndex
// -----------
//! Convert a shader attribute location to a vertex attribute index.

GLuint
VertexAttrib::_validIndex(const GLint location)
{
#if 1
    // 16 is the minimum number of generic 4-component vertex attributes
    // accessible to a vertex shader, according to OpenGL 3.2 specs.
    //
    // TODO: Might be slow to fetch this value every time this function
    //       is called.

    GLint maxVtxAttr(0);
    glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &maxVtxAttr);

    if (0 > location || location >= maxVtxAttr) {
        NB_THROW("Invalid vertex attribute index: " << location
            << " (GL_MAX_VERTEX_ATTRIBS: " << maxVtxAttr << ")");
    }
#endif

    return static_cast<GLuint>(location);
}


// _validStride
// ----------
//! Throws an exception if stride is invalid. Returns a valid stride.

GLsizei
VertexAttrib::_validStride(const GLsizei stride)
{
#if 1
    if (stride < 0) {
        NB_THROW("Invalid vertex attribute stride: " << stride);
    }
#endif

    return stride;
}

// -----------------------------------------------------------------------------
}
