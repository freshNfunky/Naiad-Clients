// -----------------------------------------------------------------------------
//
// NglError.h
//
// Useful manipulation of OpenGL errors.
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

#ifndef NGL_ERROR_H
#define NGL_ERROR_H

#include "NglExtensions.h"

//#include <Ni.h>
//#include <NgLog.h>      // NG_WARNING
//#include <Ngx.h>

#include <sstream>
#include <iostream>


namespace Ngl
{
// -----------------------------------------------------------------------------

class Error
{
public:     // Interface

    static
    bool
    check()
    {
        // Put all errors into a string and report.

        std::stringstream ss;
        GLenum err(glGetError());
        while (GL_NO_ERROR != err) {
            ss << _str(err) << " ";
            err = glGetError();
        }

        return ss.str().empty();    // No errors.

        //NG_WARNING("GL Error(s): " << NtString(ss.str()));
        //std::cerr << "GL Error(s): " << NtString(ss.str()) << "\n";
        //return false;   // There were errors.
    }

private:

    static
    NtString
    _str(const GLenum flag)
    {
        switch (flag)
        {
        case GL_NO_ERROR:
            return NtString("GL_NO_ERROR");
        case GL_INVALID_ENUM:
            return NtString("GL_INVALID_ENUM");
        case GL_INVALID_VALUE:
            return NtString("GL_INVALID_VALUE");
        case GL_INVALID_OPERATION:
            return NtString("GL_INVALID_OPERATION");
        case GL_STACK_OVERFLOW:
            return NtString("GL_STACK_OVERFLOW");
        case GL_STACK_UNDERFLOW:
            return NtString("GL_STACK_UNDERFLOW");
        case GL_OUT_OF_MEMORY:
            return NtString("GL_OUT_OF_MEMORY");
        default:
            return NtString("GL_UNKNOWN_ERROR");
        }
    }

};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_ERROR_H
