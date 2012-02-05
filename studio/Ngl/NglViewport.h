// -----------------------------------------------------------------------------
//
// NglViewport.h
//
// OpenGL compatible viewport.
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

#ifndef NGL_VIEWPORT_H
#define NGL_VIEWPORT_H

#include <Nbx.h>
#include <em_vec.h>

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>      // OGL types
#endif


namespace Ngl
{
// -----------------------------------------------------------------------------

class Viewport
{
public:     // Interface.

    Viewport(const GLint   x      = 0,
             const GLint   y      = 0,
             const GLsizei width  = 64,
             const GLsizei height = 64)
        : _x(x),
          _y(y),
          _width(width),
          _height(height)
    { apply(); }

    // Default copy, assign & destruct

    // Access

    GLint   x()      const { return _x;      }
    GLint   y()      const { return _y;      }
    GLsizei width()  const { return _width;  }
    GLsizei height() const { return _height; }

    em::vec4i toArray() const { return em::vec4i(_x, _y, _width, _height); }


    // Mutate

    void setX(const GLint x)        { _x      = x;               }
    void setY(const GLint y)        { _y      = y;               }
    void setWidth(const GLsizei w)  { _width  = _validWidth(w);  }
    void setHeight(const GLsizei h) { _height = _validHeight(h); }

    // Apply to GL state

    void apply() { glViewport(_x,_y,_width,_height); }

private:    // Member variables

    GLint   _x;
    GLint   _y;
    GLsizei _width;
    GLsizei _height;

private:    // Utility functions

    static
    GLsizei
    _validWidth(const GLsizei width)
    {
#if 1
        GLint dims[2];
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, dims);

        if (width < 0 || width > dims[0]) {
            NB_THROW(
                "Invalid viewport width: " << width <<
                " (max: " << dims[0] << ")"
                    );
        }
#endif

        return width;
    }


    static
    GLsizei
    _validHeight(const GLsizei height)
    {
#if 1
        GLint dims[2];
        glGetIntegerv(GL_MAX_VIEWPORT_DIMS, dims);

        if (height < 0 || height > dims[1]) {
            NB_THROW(
                "Invalid viewport height: " << height <<
                " (max: " << dims[1] << ")"
                    );
        }
#endif

        return height;
    }
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_VIEWPORT_H
