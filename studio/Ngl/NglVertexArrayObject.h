// -----------------------------------------------------------------------------
//
// NglVertexArrayObject.h
//
// Encapsulation of OpenGL Vertex Array Object functions.
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

// NB: Currently disabled because vertex array objects appear to be unsupported
//     on Mac. Also, they do not exactly fit the patterns that we use with
//     vertex attributes.
#if 0

#ifndef NGL_VERTEX_ARRAY_OBJECT_H
#define NGL_VERTEX_ARRAY_OBJECT_H

#include "NglNonCopyable.h"

#include <Ngx.h>


namespace Ngl
{
// -----------------------------------------------------------------------------

class VertexArrayObject : private NonCopyable
{
public:

    VertexArrayObject()
        : _handle(0)
    {
        glGenVertexArrays(1, &_handle);

        if (0 == _handle) {
            NG_THROW("Invalid VertexArrayObject handle: " << _handle);
        }
    }


    ~VertexArrayObject()
    {
        unbind();
        glDeleteVertexArrays(1, &_handle);
    }


    void bind()   const { glBindVertexArray(_handle); }
    void unbind() const { glBindVertexArray(0);       }

private:        // Member variables

    GLuint _handle;
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_VERTEX_ARRAY_OBJECT_H

#endif // DISABLED
