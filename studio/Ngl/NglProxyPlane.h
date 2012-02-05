// -----------------------------------------------------------------------------
//
// NglProxyPlane.h
//
// Plane in eye-space.
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

#ifndef NGL_PROXY_PLANE_H
#define NGL_PROXY_PLANE_H

#ifdef WIN32
#include <windows.h>
//#include <glew.h>
#endif

#include "NglTypes.h"
//#include "em_vec.h"
//#ifdef __APPLE__
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>  // OGL types
//#endif

namespace Ngl
{
// -----------------------------------------------------------------------------

class ProxyPlane
{
public:

    // Create plane from a point and a normal direction, assumed
    // to be normalized. The default plane normal direction is along
    // the negative Z-axis, which corresponds to the view direction
    // in eye-space. In this configuration we call the plane distance
    // to the origin 'z' rather than the more general 'd'.

    explicit
    ProxyPlane(const vec3f &p,
               const vec3f &n = vec3f(0, 0, -1))
       : _plane(n[0], n[1], n[2],
                -n[0]*p[0] - n[1]*p[1] - n[2]*p[2])
    {}

    const vec4f &plane() const { return _plane; }

    GLfloat a() const { return _plane[0]; }
    GLfloat b() const { return _plane[1]; }
    GLfloat c() const { return _plane[2]; }
    GLfloat z() const { return _plane[3]; }


private:
    vec4f _plane;
};

// Operators for STL binary search (std::lower_bound)

bool operator<(const ProxyPlane &lhs, const ProxyPlane &rhs);
bool operator<(const ProxyPlane &lhs, GLfloat rhs);
bool operator<(GLfloat lhs, const ProxyPlane &rhs);

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_PROXY_PLANE_H
