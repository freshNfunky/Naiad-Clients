// -----------------------------------------------------------------------------
//
// NglUtils.h
//
// Utility functions.
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

#ifndef NGL_UTILS_H
#define NGL_UTILS_H

#include "NglTypes.h"
#include <vector>


namespace Ngl
{
// -----------------------------------------------------------------------------

class Viewport;     // Fwd

GLfloat deg2rad(GLfloat deg);
GLfloat rad2deg(GLfloat rad);

// These functions operate on "raw" matrices, the code is similar
// to that found in GLU.

//void makeXform(const vec3f &translate,
//               const vec3f &rotate,
//               const vec3f &scale,
//               GLfloat     *M);

//void multMat44f(const GLfloat *a, const GLfloat *b, GLfloat *r);

//bool invMat44f(const GLfloat *m, GLfloat *i);

//void multMat44fVec4f(const GLfloat *m,
//                     const vec4f   &vin,
//                     vec4f         *vout);

//void multMat44fVec3f(const GLfloat *m,
//                     const vec3f   &vin,
//                     vec3f         *vout);

//bool project(const vec3f    &obj,
//             const Viewport &vp,
//             const GLfloat  *mv,
//             const GLfloat  *p,
//             vec4f          *eye,
//             vec4f          *clip,
//             vec3f          *ndc,
//             vec2f          *win);

// "Returns" object coords and inverse(projection*modelview)
//bool unproject(const vec3f    &win,
//               const Viewport &vp,
//               const GLfloat  *mv,
//               const GLfloat  *proj,
//               vec3f          *obj,
//               GLfloat        *invPMV);

// Uses inverse(projection*modelview) directly
//bool unproject(const vec3f    &win,
//               const Viewport &vp,
//               const GLfloat  *invPMV,
//               vec3f          *obj);

bool 
mouseRay(const Viewport     &vp,
         const em::glmat44f &mv,
         const em::glmat44f &proj,
         int                 mx,
         int                 my,
         vec3f              *ori,
         vec3f              *dir);

bool 
mouseRay(const Viewport     &vp,
         const em::glmat44f &mv,
         const em::glmat44f &proj,
         int                 mx, 
         int                 my,
         GLfloat             n, 
         GLfloat             f,
         vec3f              *ori,
         vec3f              *dir);

bool 
rayPlaneIntersect(const vec3f &rayOri,
                  const vec3f &rayDir,
                  const vec3f &planeNormal,
                  const vec3f &planeOri,
                  vec3f       *i);

bool 
raySphereIntersect(const vec3f &rayOri,
                   const vec3f &rayDir,
                   GLfloat      sr,    // Sphere radius
                   const vec3f &sc,    // Sphere center
                   vec3f       *isect);

void 
projectAxis(const Viewport &vp,
            const em::glmat44f &mv,
            const em::glmat44f &p,
            const vec3f    &start,
            const vec3f    &end,
            vec2f          *axisWin);

//void normalMatrix(const GLfloat* mv, GLfloat* nm);

// Quaternion methods

//GLfloat         norm(const em::quaternionf &q);
//em::quaternionf unit(const em::quaternionf &q);
//em::quaternionf conjugate(const em::quaternionf &q);
//em::quaternionf inverse(const em::quaternionf &q);
//vec3f eulerAngles(const em::quaternionf &q);    // Degrees.

// -----------------------------------------------------------------------------

class Cuboid
{
public:

    static const vec3i face[12];
    static const vec3f normal[6];

public:

    // default arguments here seem to break older compilers...
    explicit
    Cuboid(GLfloat dim, const vec3f &centre = vec3f(0.f));

    explicit
    Cuboid(GLfloat xmin, GLfloat xmax,
           GLfloat ymin, GLfloat ymax,
           GLfloat zmin, GLfloat zmax);

    explicit
    Cuboid(const vec3f &axis,
           GLfloat      thickness,
           GLfloat      len,
           GLfloat      offset);

    const vec3f &vtx(const GLint v) const { return _vtx[v]; }


    void set(GLfloat xmin, GLfloat xmax,
             GLfloat ymin, GLfloat ymax,
             GLfloat zmin, GLfloat zmax);

    void set(GLfloat dim, const vec3f &c = vec3f(0.f));

    void set(const vec3f &axis,
             GLfloat      thickness,
             GLfloat      len,
             GLfloat      offset);

    void draw() const;

private:

    vec3f _vtx[8];
};

// -----------------------------------------------------------------------------

class Ring
{
public:

    explicit
    Ring(const vec3f &t0,
         const vec3f &t1,
         GLfloat      r    = 1.f,
         GLint        subd = 8,
         const vec3f &c    = vec3f(0.f));

    const vec3f &vtx(const GLint v) const { return _vtx[v]; }

    void draw(GLfloat lineWidth) const;

private:

    std::vector<vec3f> _vtx;
};

// -----------------------------------------------------------------------------

class Disk
{
public:

    explicit
    Disk(const vec3f &t0,
         const vec3f &t1,
         GLfloat      radius = 1.f,
         GLint        subd   = 8,
         const vec3f &c      = vec3f(0.f));

    //const vec3f &vtx(const GLint v) const { return _vtx[v]; }

    void draw() const;

private:

    std::vector<vec3f> _vtx;
    vec3f _nml;
};

// -----------------------------------------------------------------------------

class Cone
{
public:

    explicit
    Cone(const vec3f   &axis,
         const vec3f   &t0,       // Tangent directions
         const vec3f   &t1,
         const GLfloat  h0   = 0.f,
         const GLfloat  h    = 1.f,
         const GLfloat  r    = 1.f,
         const GLint    subd = 8,
         const vec3f   &c    = vec3f(0.f));

    void draw() const;

private:

    vec3f _axis;
    vec3f _baseMid;
    vec3f _top;
    std::vector<vec3f> _vtx;
    std::vector<vec3f> _nml;
};

// -----------------------------------------------------------------------------

class Sphere
{
public:

    explicit
    Sphere(const GLfloat  radius = 1.f,
           const GLint    subd   = 8,
           const vec3f   &c      = vec3f(0.f));

    void draw() const;

    GLfloat radius() const { return _radius; }
    vec3f   center() const { return _center; }

private:

    std::vector<vec3f> _vtx;
    std::vector<vec3f> _nml;
    GLfloat            _radius;
    vec3f              _center;
};

// -----------------------------------------------------------------------------

class Torus
{
public:

    explicit
    Torus(//const vec3f &t0,
          //const vec3f &t1,
          GLfloat      innerRadius = 0.1f,
          GLfloat      outerRadius = 1.f,
          GLint        subdSide    = 8,
          GLint        subdRing    = 8,
          const vec3f &c           = vec3f(0.f));

    void draw() const;

private:

    vec3f vertex(GLfloat thetaRad,
                 GLfloat phiRad,
                 GLfloat innerRadius,
                 GLfloat outerRadius,
                 const vec3f &c) const;

    vec3f normal(const vec3f &vtx,
                 GLfloat      thetaRad,
                 GLfloat      outerRadius) const;

private:

    std::vector<vec3f> _vtx;
    std::vector<vec3f> _nml;
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_UTILS_H
