// -----------------------------------------------------------------------------
//
// NglVertexBuffer.h
//
// Vertex buffer object class.
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

#ifndef NGL_VERTEX_BUFFER_H
#define NGL_VERTEX_BUFFER_H

#include "NglNonCopyable.h"
#include "NglExtensions.h"

#include <em_array1.h>
#include <em_block3_array.h>

#include <Ni.h>

namespace Ngl
{
// -----------------------------------------------------------------------------
//
// TODO: It would be nice if instances of this class were able to report
//       an error if the requested memory was not available. The only known
//       way to retrieve such information at present is to call glGetError()
//       and compare the returned value with GL_OUT_OF_MEMORY. However,
//       glGetError may report errors that were made in other parts of the
//       code. Thus, if glGetError is not used consistently throughout it is
//       is not a good option to use here, since we cannot guarantee where
//       the errors are actually coming from.
//
//       Such memory error checking should be done directly after calls
//       to glBufferData().
//
// Note that the OpenGL handle is not exposed. This is intentional, and the
// purpose is to prevent outside calls from modifying the buffer. There is no
// way of preventing this from happening anyway, but it becomes less likely if
// outside calls have to guess the OpenGL handle. Only severely improper
// coding would cause such bad behaviour.
//

class VertexBuffer : private NonCopyable
{
public:

    VertexBuffer(GLenum target = GL_ARRAY_BUFFER);

    explicit
    VertexBuffer(GLsizeiptr    size,         // [bytes]
                 const GLvoid* data,
                 GLenum        target = GL_ARRAY_BUFFER,
                 GLenum        usage  = GL_STATIC_DRAW);

    explicit
    VertexBuffer(const em::array1<GLsizeiptr>&    dataSize,
                 const em::array1<const GLvoid*>& data,
                 GLenum                           target = GL_ARRAY_BUFFER,
                 GLenum                           usage  = GL_STATIC_DRAW);

    explicit
    VertexBuffer(const em::block3_array3f& blocks,
                 const int                 block0,
                 const int                 block1,
                 GLenum                    target = GL_ARRAY_BUFFER,
                 GLenum                    usage  = GL_STATIC_DRAW);

    explicit
    VertexBuffer(const em::block3_array1f& blocks,
                 const int                 block0,
                 const int                 block1,
                 GLenum                    target = GL_ARRAY_BUFFER,
                 GLenum                    usage  = GL_STATIC_DRAW);

    ~VertexBuffer();

    GLenum     target() const;
    GLsizeiptr size()   const;

    void bind()   const;
    void unbind() const;

    void setData(GLsizeiptr    size,         // [bytes]
                 const GLvoid* data  = 0,
                 GLenum        usage = GL_STATIC_DRAW);

    void setSubData(GLintptr      offset,
                    GLsizeiptr    size,      // [bytes]
                    const GLvoid* data);


    void subData(GLintptr   offset,
                 GLsizeiptr size,
                 GLvoid*    data) const;

    GLint access() const;
    bool  mapped() const;
    GLint usage() const;

    bool attachMetaData1i(const NtString& name, const int data);
    int  metaData1i(const NtString& name) const;
    bool hasMetaData1i(const NtString& name) const;

    bool  attachMetaData1f(const NtString& name, const float data);
    float metaData1f(const NtString& name) const;
    bool hasMetaData1f(const NtString& name) const;

    //void*     map(GLenum access);
    //GLboolean unmap();

private:        // Member variables.

    GLuint     _handle;     // OpenGL Id.
    GLenum     _target;     // See validTarget().
    GLsizeiptr _size;       // Buffer size in bytes.

    std::map<NtString, int> _intMeta;
    std::map<NtString, float> _floatMeta;


private:        // Utility functions.

    void _alloc(GLsizeiptr    size,         // [bytes]
                const GLvoid* data  = 0,
                GLenum        usage = GL_STATIC_DRAW);

    void _setData(GLsizeiptr           totalDataSize,
                  const GLsizeiptr*    dataSize,
                  const GLvoid* const* data,
                  std::size_t          n,
                  GLenum               usage = GL_STATIC_DRAW);

    static void _genBuffers(GLuint* handle, GLsizei n = 1);

    static GLenum     _validTarget(GLenum target);
    static GLenum     _validUsage(GLenum usage);
    static GLsizeiptr _validSize(GLsizeiptr size);
    static GLintptr   _validOffset(GLintptr offset);
    static GLenum     _validAccess(GLenum access);


    class _Range
    {
    public:         // Interface

        explicit
        _Range(GLintptr offset, GLsizeiptr size, GLsizeiptr totalSize);

        GLintptr   offset() const;
        GLsizeiptr size()   const;

    private:        // Member variables.

        GLintptr   _offset;
        GLsizeiptr _size;

    private:        // Disabled.

        _Range();
    };
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_VERTEX_BUFFER_H
