// -----------------------------------------------------------------------------
//
// NglVertexBuffer.cc
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

#include "NglVertexBuffer.h"
#include "NglError.h"

#include <Nbx.h>

#include <em_array1.h>
#include <em_block3_array.h>

#include <vector>

namespace Ngl
{
// -----------------------------------------------------------------------------

// VertexBuffer
// ------------
//! Construct an empty VBO, no resources are allocated (except the handle).

VertexBuffer::VertexBuffer(const GLenum target)
    : _handle(0),
      _target(_validTarget(target)), // May throw
      _size(0)
{
    _genBuffers(&_handle);           // May throw
}


// VertexBuffer
// ------------
//! Construct a VBO from a single chunk of memory. Note that
//! if data is null the buffer will be allocated but not filled.

VertexBuffer::VertexBuffer(const GLsizeiptr size,         // [bytes]
                           const GLvoid*    data,
                           const GLenum     target,
                           const GLenum     usage)
    : _handle(0),
      _target(_validTarget(target)), // May throw
      _size(0)
{
    _genBuffers(&_handle);           // May throw
    setData(size, data, usage);
}


// VertexBuffer
// ------------
//! Construct VBO from multiple chunks of memory.

VertexBuffer::VertexBuffer(const em::array1<GLsizeiptr>&    dataSize,
                           const em::array1<const GLvoid*>& data,
                           const GLenum                     target,
                           const GLenum                     usage)
    : _handle(0),
      _target(_validTarget(target)), // May throw
      _size(0)
{
    _genBuffers(&_handle);           // May throw

    GLsizeiptr totalDataSize(0);
    for(int64_t i(0); i < data.size(); ++i) {
        totalDataSize += dataSize(i);
    }

    _setData(totalDataSize,
             &dataSize[0],
             &data[0],
             data.size(),
             usage);
}


// VertexBuffer
// ------------
//! Construct VBO from blocks of 3D data.

VertexBuffer::VertexBuffer(const em::block3_array3f& blocks,
                           const int                 block0,
                           const int                 block1,
                           const GLenum              target,
                           const GLenum              usage)
    : _handle(0),
      _target(_validTarget(target)), // May throw
      _size(0)
{
    _genBuffers(&_handle);           // May throw

    const int b0(std::max<int>(0,block0));
    const int b1(std::min<int>(block1+1,blocks.block_count()));
    const int blockCount(b1-b0);
    if(blockCount<0)
        NB_THROW("Invalid block range");

    if (blockCount > 0) {
        std::vector<const GLvoid*> data(blockCount,
                                        reinterpret_cast<const GLvoid*>(0));
        std::vector<GLsizeiptr>    dataSize(blockCount, 0);
        GLsizeiptr                 totalDataSize(0);

        for(int64_t b(b0); b < b1; ++b) {
            if(blocks.const_block_base(b)==0) 
                continue;
            data[b-b0]     = reinterpret_cast<const GLvoid*>(blocks(b).data());
            dataSize[b-b0] = (3*sizeof(GLfloat))*blocks(b).size();
            totalDataSize += dataSize[b-b0];
        }

        _setData(totalDataSize,
                 &dataSize[0],
                 &data[0],
                 blockCount,
                 usage);
    }
}


// VertexBuffer
// ------------
//! Construct VBO from blocks of 1D data.

VertexBuffer::VertexBuffer(const em::block3_array1f& blocks,
                           const int                 block0,
                           const int                 block1,
                           const GLenum              target,
                           const GLenum              usage)
    : _handle(0),
      _target(_validTarget(target)), // May throw
      _size(0)
{
    _genBuffers(&_handle);           // May throw

    const int b0(std::max<int>(0,block0));
    const int b1(std::min<int>(block1+1,blocks.block_count()));
    const int blockCount(b1-b0);
    if(blockCount<0)
        NB_THROW("Invalid block range");

    if (blockCount > 0) {
        std::vector<const GLvoid*> data(blockCount,
                                        reinterpret_cast<const GLvoid*>(0));
        std::vector<GLsizeiptr>    dataSize(blockCount, 0);
        GLsizeiptr                 totalDataSize(0);

        for(int64_t b(b0); b < b1; ++b) {
            if(blocks.const_block_base(b)==0) 
                continue;
            data[b-b0]     = reinterpret_cast<const GLvoid*>(blocks(b).data());
            dataSize[b-b0] = sizeof(GLfloat)*blocks(b).size();
            totalDataSize += dataSize[b-b0];
        }

        _setData(totalDataSize,
                 &dataSize[0],
                 &data[0],
                 blockCount,
                 usage);
    }
}


// ~VertexBuffer
// -------------
//! Destructor.

VertexBuffer::~VertexBuffer()
{
#if 0
    std::cerr << "Destroy VBO: " << _handle << "\n";
#endif
    unbind();

    // Free handle resource

    glDeleteBuffers(1, &_handle);
}


// target
// ------
//! Return target. See validTarget().

GLenum
VertexBuffer::target() const
{
    return _target;
}


// size
// ----
//! Return size of buffer in bytes.

GLsizeiptr
VertexBuffer::size() const
{
    return _size;
}


// bind
// ----
//! Bind buffer.

void
VertexBuffer::bind() const
{
    glBindBuffer(_target, _handle);
}


// unbind
// ------
//! Unbind buffer.

void
VertexBuffer::unbind() const
{
    glBindBuffer(_target, 0);
}


// setData
// -------
//! Upload data to GPU memory. If data is null, memory gets allocated but
//! nothing gets transferred. This is useful for populating a VBO from
//! several sub-buffers, e.g. blocks.

void
VertexBuffer::setData(const GLsizeiptr size,         // [bytes]
                      const GLvoid*    data,
                      const GLenum     usage)
{
    bind();     // Make current.
    _alloc(size, data, usage);
    unbind();
}


// setSubData
// ----------
//! Upload data to GPU memory. Size of buffer remains constant.

void
VertexBuffer::setSubData(const GLintptr   offset,
                         const GLsizeiptr size,      // [bytes]
                         const GLvoid*    data)
{
    _Range range(offset, size, _size);  // May throw

    // Range is within current size.

    bind();     // Make current.
    glBufferSubData(_target, range.offset(), range.size(), data);
    unbind();
}


// subData
// -------
//! Read back a portion of data from GPU memory.

void
VertexBuffer::subData(const GLintptr   offset,
                      const GLsizeiptr size,
                      GLvoid*          data) const
{
    _Range range(offset, size, _size);  // May throw

    // Range is within current size.

    bind();     // Make current.
    glGetBufferSubData(_target, range.offset(), range.size(), data);
    unbind();
}


// The following functions retrieve buffer parameters from the driver.
// There may be a performance penalty involved, so use with caution.
//
// Note that these functions operate on the currently "bound" buffer,
// which is why we bind/unbind the current buffer before/after the
// queries.

// access
// ------
//! Return access mode.

GLint
VertexBuffer::access() const
{
    bind(); // Make current.
    GLint access(GL_READ_WRITE);
    glGetBufferParameteriv(_target, GL_BUFFER_ACCESS, &access);
    unbind();
    return access;
}


// mapped
// ------
//! Return true if buffer is currently mapped.

bool
VertexBuffer::mapped() const
{
    bind(); // Make current.
    GLint mapped(GL_FALSE);
    glGetBufferParameteriv(_target, GL_BUFFER_MAPPED, &mapped);
    unbind();
    return (GL_TRUE == mapped);
}


//// size
//// ------
////! Return size of buffer in bytes.
//
//GLint
//VertexBuffer::size() const
//{
//    bind(); // Make current.
//    GLint size(0);
//    glGetBufferParameteriv(_target, GL_BUFFER_SIZE, &size);
//    unbind();
//    return size;    // [bytes]
//}


// usage
// -----
//! Return buffer usage mode.

GLint
VertexBuffer::usage() const
{
    bind(); // Make current.
    GLint usage(GL_STATIC_DRAW);
    glGetBufferParameteriv(_target, GL_BUFFER_USAGE, &usage);
    unbind();
    return usage;
}


// NB: Mapping functions are untested. Use with caution!
//
//// map
//// ---
////! Provide pointer to data.
//
//void*
//VertexBuffer::map(const GLenum access)
//{
//    bind();     // Make current.
//    void* mapPtr(glMapBuffer(_target, _validAccess(access)));
//    unbind();
//    return mapPtr;
//}
//
//
//// unmap
//// -----
////! Let OpenGL know that buffer is no longer mapped.
//
//GLboolean
//VertexBuffer::unmap()
//{
//    // Returns true unless the data contents have become corrupted while
//    // the buffer was mapped. If this function returns false the buffer
//    // should be reinitialized.
//
//    bind();     // Make current.
//    const GLboolean status(glUnmapBuffer(_target));
//    unbind();
//
//    return status;
//}


//! Returns true if meta value was inserted. Does nothing if name already
//! exists.

bool
VertexBuffer::attachMetaData1i(const NtString& name, const int data)
{
    typedef std::map<NtString, int>::value_type ValueType;
    return _intMeta.insert(ValueType(name, data)).second;
}

int
VertexBuffer::metaData1i(const NtString& name) const
{
    typedef std::map<NtString, int>::const_iterator IterType;

    const IterType iter(_intMeta.find(name));
    if (iter != _intMeta.end()) {
        return iter->second;
    }
    NB_THROW("No VBO integer meta data called '" << name << "'");
}


bool
VertexBuffer::hasMetaData1i(const NtString& name) const
{
    return (_intMeta.find(name) != _intMeta.end());
}


//! Returns true if meta value was inserted. Does nothing if name already
//! exists.

bool
VertexBuffer::attachMetaData1f(const NtString& name, const float data)
{
    typedef std::map<NtString, float>::value_type ValueType;
    return _floatMeta.insert(ValueType(name, data)).second;
}


float
VertexBuffer::metaData1f(const NtString& name) const
{
    typedef std::map<NtString, float>::const_iterator IterType;

    const IterType iter(_floatMeta.find(name));
    if (iter != _floatMeta.end()) {
        return iter->second;
    }
    NB_THROW("No VBO float meta data called '" << name << "'");
}


bool
VertexBuffer::hasMetaData1f(const NtString& name) const
{
    return (_floatMeta.find(name) != _floatMeta.end());
}


// _alloc
// ------
//! Allocate buffer on GPU. Assumes that the buffer is currently bound.

void
VertexBuffer::_alloc(const GLsizeiptr size,         // [bytes]
                     const GLvoid*    data,
                     const GLenum     usage)
{
    // Create buffer on GPU.

    glBufferData(_target,
                 _validSize(size),       // May throw
                 data,
                 _validUsage(usage));    // May throw

    // Get actual buffer size from OpenGL.

    GLint i(0);
    glGetBufferParameteriv(_target, GL_BUFFER_SIZE, &i);
    _size = static_cast<GLsizeiptr>(i);
}


// _setData
// --------
//! Set buffer data.

void
VertexBuffer::_setData(const GLsizeiptr     totalDataSize,
                       const GLsizeiptr*    dataSize,
                       const GLvoid* const* data,
                       const std::size_t    n,
                       const GLenum         usage)
{
    EM_ASSERT(Error::check());

    // Allocate VBO memory

    bind();                                 // Make current
    _alloc(_validSize(totalDataSize),       // May throw
           0,                               // No data transferred
           _validUsage(usage)               // May throw
           );

    // Copy the data into the VBO

    GLintptr offset(0);
    for(std::size_t i(0); i < n; ++i) {
        if (0 != data[i]) {
            glBufferSubData(_target,
                            _validOffset(offset),    // May throw
                            _validSize(dataSize[i]), // May throw
                            data[i]);
        }
        offset += dataSize[i];
    }
    unbind();

    EM_ASSERT(Error::check());
}


// _genBuffers
// -----------
//! Generate OpenGL id's.

void
VertexBuffer::_genBuffers(GLuint*       handle,
                          const GLsizei n)
{
    if (0 != handle) {
#if 1
        if (n < 1) {
            NB_THROW("Invalid vertex buffer n: " << n);
        }
#endif

        glGenBuffers(n, handle);

#if 1
        for (GLsizei i(0); i < n; ++i) {
            if (0 == handle[i]) {
                NB_THROW("Invalid vertex buffer handle: " << handle[i]);
            }
        }
#endif
    }
}


// _validTarget
// ------------
//! Throw an exception if provided target is not valid. Returns a valid target.

GLenum
VertexBuffer::_validTarget(const GLenum target)
{
#if 1
    if (GL_ARRAY_BUFFER         != target &&
        GL_ELEMENT_ARRAY_BUFFER != target &&
        GL_PIXEL_PACK_BUFFER    != target &&
        GL_PIXEL_UNPACK_BUFFER  != target) {
        NB_THROW("Invalid vertex buffer target: " << target);
    }
#endif

    return target;
}


// _validUsage
// ------------
//! Throw an exception if provided usage mode is not valid. Returns a valid
//! usage mode.

GLenum
VertexBuffer::_validUsage(const GLenum usage)
{
#if 1
    if (GL_STREAM_DRAW  != usage &&
        GL_STREAM_READ  != usage &&
        GL_STREAM_COPY  != usage &&
        GL_STATIC_DRAW  != usage &&
        GL_STATIC_READ  != usage &&
        GL_STATIC_COPY  != usage &&
        GL_DYNAMIC_DRAW != usage &&
        GL_DYNAMIC_READ != usage &&
        GL_DYNAMIC_COPY != usage) {
        NB_THROW("Invalid vertex buffer usage: " << usage);
    }
#endif

    return usage;
}


// _validSize
// ------------
//! Throw an exception if provided size is not valid. Returns a valid
//! size.

GLsizeiptr
VertexBuffer::_validSize(const GLsizeiptr size)
{
#if 1
    if (size < 0) {
        NB_THROW("Invalid vertex buffer size: " << size);
    }
#endif

    return size;
}


// _validOffset
// ------------
//! Throw an exception if provided offset is not valid. Returns a valid
//! offset.

GLintptr
VertexBuffer::_validOffset(const GLintptr offset)
{
#if 1
    if (offset < 0) {
        NB_THROW("Invalid vertex buffer offset: " << offset);
    }
#endif

    return offset;
}


// _validAccess
// ------------
//! Throw an exception if provided access mode is not valid. Returns a valid
//! access mode.

GLenum
VertexBuffer::_validAccess(const GLenum access)
{
#if 1
    if (GL_READ_ONLY  != access &&
        GL_WRITE_ONLY != access &&
        GL_READ_WRITE != access) {
        NB_THROW("Invalid vertex buffer access: " << access);
    }
#endif

    return access;
}


// _Range
// ------
//! Construct a valid range. Will throw if range is not valid.

VertexBuffer::_Range::_Range(const GLintptr   offset,
                             const GLsizeiptr size,
                             const GLsizeiptr totalSize)
    : _offset(VertexBuffer::_validOffset(offset)),    // May throw
      _size(VertexBuffer::_validSize(size))           // May throw
{
    if (offset + size > totalSize) {
        NB_THROW("Invalid vertex buffer range: ["
                 << offset << ", " << offset + size << "], size: "
                 << totalSize);
    }
}


// offset
// ------
//! Return offset.

GLintptr
VertexBuffer::_Range::offset() const
{
    return _offset;
}


// size
// ----
//! Return size.

GLsizeiptr
VertexBuffer::_Range::size() const
{
    return _size;
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
