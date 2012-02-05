// -----------------------------------------------------------------------------
//
// NglUtils.cc
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

#include "NglUtils.h"
#include "NglViewport.h"
//#include <em_mat44_algo.h>
#include <em_glmat44.h>
#include <em_glmat44_algo.h>
#include <cmath>



namespace Ngl
{
// -----------------------------------------------------------------------------

GLfloat
deg2rad(const GLfloat deg)
{
    return ((static_cast<GLfloat>(M_PI)/180.f)*deg);
}


GLfloat
rad2deg(const GLfloat rad)
{
    return ((180.f/static_cast<GLfloat>(M_PI))*rad);
}


bool
mouseRay(const Viewport     &vp,
         const em::glmat44f &mv,
         const em::glmat44f &proj,
         const int           mx,
         const int           my,
         vec3f              *ori,
         vec3f              *dir)
{
    return mouseRay(vp, mv, proj, mx, my, 0, 1, ori, dir);
}


bool 
mouseRay(const Viewport     &vp,
         const em::glmat44f &mv,
         const em::glmat44f &proj,
         const int           mx, 
         const int           my,
         const GLfloat       n, 
         const GLfloat       f,
         vec3f              *ori,
         vec3f              *dir)
{
    const vec3f nearSrc(mx, my, n);
    const vec3f farSrc(mx, my, f);
    vec3f nearPnt;
    vec3f farPnt;
    em::glmat44f invPMV;
    if (!em::unproject(nearSrc, vp.toArray(), mv, proj, nearPnt, invPMV) ||
        !em::unproject(farSrc, vp.toArray(), invPMV, farPnt)) {
        return false;
    }
    *ori = nearPnt;
    *dir = em::normalized(farPnt - nearPnt);
    return true;
}


bool
rayPlaneIntersect(const vec3f &rayOri,
                  const vec3f &rayDir,
                  const vec3f &planeNormal,
                  const vec3f &planeOri,
                  vec3f       *i)
{
    const GLfloat denom = em::dot(planeNormal, rayDir);
    if (0.f == denom) {
        return false;
    }

    const GLfloat d = -planeNormal[0]*planeOri[0]
                      -planeNormal[1]*planeOri[1]
                      -planeNormal[2]*planeOri[2];
    const GLfloat num = em::dot(planeNormal, rayOri) + d;
    *i = rayOri - (num/denom)*rayDir;
    return true;
}


bool raySphereIntersect(const vec3f &rayOri,
                        const vec3f &rayDir,
                        GLfloat      sr,    // Sphere radius
                        const vec3f &sc,    // Sphere center
                        vec3f       *isect)
{
    // Assumes rayDir is normalized.

    vec3f dst = rayOri - sc;
    const GLfloat b = em::dot(dst, rayDir);
    const GLfloat c = em::dot(dst, dst) - sr*sr;
    const GLfloat d = b*b - c;

    if (0.f > d)
        return false;

    const GLfloat t(-b - sqrtf(d)); // Smallest root

    if (0.f > t)
        return false;

    if (0 != isect) {
        *isect = rayOri + t*rayDir;
    }

    return true;
}


void
projectAxis(const Viewport     &vp,
            const em::glmat44f &mv,
            const em::glmat44f &p,
            const vec3f        &start,
            const vec3f        &end,
            vec2f              *axisWin)
{
    vec4f obj[2] = {
        vec4f(start[0], start[1], start[2], 1.f),
        vec4f(end[0], end[1], end[2], 1.f)
    };

    vec4f eye[2];
    vec4f clip[2];
    vec2f ndc[2];
    vec2f win[2];

    for (int i = 0; i < 2; ++i) {
        eye[i][0] = mv[0][0]*obj[i][0] +
                    mv[1][0]*obj[i][1] +
                    mv[2][0]*obj[i][2] +
                    mv[3][0]*obj[i][3];
        eye[i][1] = mv[0][1]*obj[i][0] +
                    mv[1][1]*obj[i][1] +
                    mv[2][1]*obj[i][2] +
                    mv[3][1]*obj[i][3];
        eye[i][2] = mv[0][2]*obj[i][0] +
                    mv[1][2]*obj[i][1] +
                    mv[2][2]*obj[i][2] +
                    mv[3][2]*obj[i][3];
        eye[i][3] = mv[0][3]*obj[i][0] +
                    mv[1][3]*obj[i][1] +
                    mv[2][3]*obj[i][2] +
                    mv[3][3]*obj[i][3];

        clip[i][0] = p[0][0]*eye[i][0] +
                     p[1][0]*eye[i][1] +
                     p[2][0]*eye[i][2] +
                     p[3][0]*eye[i][3];
        clip[i][1] = p[0][1]*eye[i][0] +
                     p[1][1]*eye[i][1] +
                     p[2][1]*eye[i][2] +
                     p[3][1]*eye[i][3];
        clip[i][2] = p[0][2]*eye[i][0] +
                     p[1][2]*eye[i][1] +
                     p[2][2]*eye[i][2] +
                     p[3][2]*eye[i][3];
        clip[i][3] = p[0][3]*eye[i][0] +
                     p[1][3]*eye[i][1] +
                     p[2][3]*eye[i][2] +
                     p[3][3]*eye[i][3];

        ndc[i][0] = clip[i][0]/clip[i][3];
        ndc[i][1] = clip[i][1]/clip[i][3];

        win[i][0] = vp.x() + 0.5f*vp.width()*(ndc[i][0] + 1.f);
        win[i][1] = vp.y() + 0.5f*vp.height()*(ndc[i][1] + 1.f);
    }

    (*axisWin)[0] = win[1][0] - win[0][0];
    (*axisWin)[1] = win[1][1] - win[0][1];
}


//// normalMatrix
//// ------------
////! Compute the matrix used to transform normals given a modelview
////! matrix. The normal matrix is the transpose of the inverse of the
////! top left 3x3 of the modelview matrix.

//void
//normalMatrix(const GLfloat* mv, GLfloat* nm)
//{

//    // 3x3 determinant

//    const GLfloat det(
//        mv[0]*(mv[10]*mv[5] - mv[6]*mv[9]) -
//        mv[1]*(mv[10]*mv[4] - mv[6]*mv[8]) +
//        mv[2]*(mv[ 9]*mv[4] - mv[5]*mv[8]));

//    if (det != 0.f) {
//        // Compute inverse of upper-left 3x3 of modelview matrix.
//        // Transpose is done in the assignment to normal matrix.

//        const GLfloat invDet(1.f/det);

////      nm[0] = invNmDet*(  mv[10]*mv[5] - mv[6]*mv[9]);
////      nm[1] = invNmDet*(-(mv[10]*mv[1] - mv[2]*mv[9]));
////      nm[2] = invNmDet*(  mv[ 6]*mv[1] - mv[2]*mv[5]);
////      nm[3] = invNmDet*(-(mv[10]*mv[4] - mv[6]*mv[8]));
////      nm[4] = invNmDet*(  mv[10]*mv[0] - mv[2]*mv[8]);
////      nm[5] = invNmDet*(-(mv[ 6]*mv[0] - mv[2]*mv[4]));
////      nm[6] = invNmDet*(  mv[ 9]*mv[4] - mv[5]*mv[8]);
////      nm[7] = invNmDet*(-(mv[ 9]*mv[0] - mv[1]*mv[8]));
////      nm[8] = invNmDet*(  mv[ 5]*mv[0] - mv[1]*mv[4]);
//        nm[0] = invDet*(  mv[10]*mv[5] - mv[6]*mv[9]);
//        nm[3] = invDet*(-(mv[10]*mv[1] - mv[2]*mv[9]));
//        nm[6] = invDet*(  mv[ 6]*mv[1] - mv[2]*mv[5]);
//        nm[1] = invDet*(-(mv[10]*mv[4] - mv[6]*mv[8]));
//        nm[4] = invDet*(  mv[10]*mv[0] - mv[2]*mv[8]);
//        nm[7] = invDet*(-(mv[ 6]*mv[0] - mv[2]*mv[4]));
//        nm[2] = invDet*(  mv[ 9]*mv[4] - mv[5]*mv[8]);
//        nm[5] = invDet*(-(mv[ 9]*mv[0] - mv[1]*mv[8]));
//        nm[8] = invDet*(  mv[ 5]*mv[0] - mv[1]*mv[4]);
//    }
//    else {
//        // Determinant is zero, we cannot invert.
//        // Set normal matrix to identity.

//        nm[0] = 1.f; nm[3] = 0.f; nm[6] = 0.f;
//        nm[1] = 0.f; nm[4] = 1.f; nm[7] = 0.f;
//        nm[2] = 0.f; nm[5] = 0.f; nm[8] = 1.f;
//    }
//}

//GLfloat norm(const em::quaternionf& q)
//{
//    // Computes the norm (sum of squares)
//
//    return (q[0]*q[0] + q[1]*q[1]+ q[2]*q[2] + q[3]*q[3]);
//}
//
//// Reduce to unit quaternion
//Quaternion Quaternion::Unit()
//{ // Unit()
//    Quaternion result;
//    // get the square root of the norm
//    GLfloat sqrtNorm = Norm();
//    // now divide by it
//    for (int i = 0; i < 4; i++)
//            result.coords[i] = coords[i] / sqrtNorm;
//    return result;
//} // Unit()
//

//em::quaternionf
//conjugate(const em::quaternionf& q)
//{
//    // Conjugate the quaternion
//
//    return em::quaternionf(q[0], -q[1], -q[2], -q[3]);
//}
//
//
//em::quaternionf
//inverse(const em::quaternionf& q)
//{
//    // Invert the quaternion
//
//    return conjugate(q)/mag(q);
//}
//
//
//vec3f
//eulerAngles(const em::quaternionf &q)
//{
//    em::mat44f rot(q);
//    return vec3f(
//        rad2deg(-atan2(rot[2][1], rot[2][2])),
//        rad2deg(asin(rot[2][0])),
//        rad2deg(-atan2(rot[1][0], rot[0][0])));
//
////    return vec3f(
////        Ngl::rad2deg(atan2(
////            2.f*(q[0]*q[1] + q[2]*q[3]),
////            1.f - 2.f*(q[1]*q[1] + q[2]*q[2]))),
////        Ngl::rad2deg(asin(
////            2.f*(q[0]*q[2] - q[3]*q[1]))),
////        Ngl::rad2deg(atan2(
////            2.f*(q[0]*q[3] + q[1]*q[2]),
////            1.f - 2.f*(q[2]*q[2] + q[3]*q[3]))));
//}

// -----------------------------------------------------------------------------

const vec3i
Cuboid::face[12] = {
    vec3i(1, 5, 6), vec3i(6, 2, 1),     // X+
    vec3i(7, 4, 0), vec3i(0, 3, 7),     // X-
    vec3i(6, 5, 4), vec3i(4, 7, 6),     // Y+
    vec3i(3, 0, 1), vec3i(1, 2, 3),     // Y-
    vec3i(2, 6, 7), vec3i(7, 3, 2),     // Z+
    vec3i(0, 4, 5), vec3i(5, 1, 0)      // Z-
};


const vec3f
Cuboid::normal[6] = {
    vec3f( 1.f,  0.f,  0.f),     // X+
    vec3f(-1.f,  0.f,  0.f),     // X-
    vec3f( 0.f,  1.f,  0.f),     // Y+
    vec3f( 0.f, -1.f,  0.f),     // Y-
    vec3f( 0.f,  0.f,  1.f),     // Z+
    vec3f( 0.f,  0.f, -1.f)      // Z-
};


Cuboid::Cuboid(const GLfloat  dim,
               const vec3f   &centre)
{
    set(dim, centre);
}


Cuboid::Cuboid(const GLfloat xmin, const GLfloat xmax,
               const GLfloat ymin, const GLfloat ymax,
               const GLfloat zmin, const GLfloat zmax)
{
    set(xmin, xmax,
        ymin, ymax,
        zmin, zmax);
}


Cuboid::Cuboid(const vec3f   &axis,
               const GLfloat  thickness,
               const GLfloat  len,
               const GLfloat  offset)
{
    set(axis, thickness, len, offset);
}


void
Cuboid::set(const GLfloat xmin, const GLfloat xmax,
            const GLfloat ymin, const GLfloat ymax,
            const GLfloat zmin, const GLfloat zmax)
{
    _vtx[0] = vec3f(xmin, ymin, zmin);
    _vtx[1] = vec3f(xmax, ymin, zmin);
    _vtx[2] = vec3f(xmax, ymin, zmax);
    _vtx[3] = vec3f(xmin, ymin, zmax);
    _vtx[4] = vec3f(xmin, ymax, zmin);
    _vtx[5] = vec3f(xmax, ymax, zmin);
    _vtx[6] = vec3f(xmax, ymax, zmax);
    _vtx[7] = vec3f(xmin, ymax, zmax);
}


void
Cuboid::set(const GLfloat dim, const vec3f &c)
{
    const GLfloat hdim(0.5f*std::max<GLfloat>(dim, 0.f));
    _vtx[0] = vec3f(-hdim + c[0], -hdim + c[1], -hdim + c[2]);
    _vtx[1] = vec3f( hdim + c[0], -hdim + c[1], -hdim + c[2]);
    _vtx[2] = vec3f( hdim + c[0], -hdim + c[1],  hdim + c[2]);
    _vtx[3] = vec3f(-hdim + c[0], -hdim + c[1],  hdim + c[2]);
    _vtx[4] = vec3f(-hdim + c[0],  hdim + c[1], -hdim + c[2]);
    _vtx[5] = vec3f( hdim + c[0],  hdim + c[1], -hdim + c[2]);
    _vtx[6] = vec3f( hdim + c[0],  hdim + c[1],  hdim + c[2]);
    _vtx[7] = vec3f(-hdim + c[0],  hdim + c[1],  hdim + c[2]);
}


void
Cuboid::set(const vec3f   &axis,
            const GLfloat  thickness,
            const GLfloat  len,
            const GLfloat  offset)
{
    const GLfloat hThickness(0.5f*thickness);

    vec3f v0(0.f, 0.f, 0.f);
    vec3f v1(0.f, 0.f, 0.f);
    v0[0] += axis[0]*offset;
    v0[0] += (1.f - fabs(axis[0]))*(-hThickness);
    v1[0] += axis[0]*len;
    v1[0] += (1.f - fabs(axis[0]))*hThickness;

    v0[1] += axis[1]*offset;
    v0[1] += (1.f - fabs(axis[1]))*(-hThickness);
    v1[1] += axis[1]*len;
    v1[1] += (1.f - fabs(axis[1]))*hThickness;

    v0[2] += axis[2]*offset;
    v0[2] += (1.f - fabs(axis[2]))*(-hThickness);
    v1[2] += axis[2]*len;
    v1[2] += (1.f - fabs(axis[2]))*hThickness;

    set(std::min<GLfloat>(v0[0], v1[0]), std::max<GLfloat>(v0[0], v1[0]),
        std::min<GLfloat>(v0[1], v1[1]), std::max<GLfloat>(v0[1], v1[1]),
        std::min<GLfloat>(v0[2], v1[2]), std::max<GLfloat>(v0[2], v1[2]));
}


void
Cuboid::draw() const
{
    glBegin(GL_TRIANGLES);
    for (int f = 0; f < 12; ++f) {
        glNormal3fv(&Cuboid::normal[f >> 1][0]);  // NB: Bit-shift divide-by-two
        for (int v = 0; v < 3; ++v) {
            glVertex3fv(&_vtx[Cuboid::face[f][v]][0]);
        }
    }
    glEnd();
}

// -----------------------------------------------------------------------------

Ring::Ring(const vec3f &t0,
           const vec3f &t1,
           GLfloat      r,
           GLint        subd,
           const vec3f &c)
{
    _vtx.resize(std::max<GLint>(3, subd));

    GLfloat dTheta = deg2rad(360.f/_vtx.size());
    const GLfloat rad(std::max<GLfloat>(0.f, r));

    for (std::vector<vec3f >::size_type v = 0;
         v < _vtx.size();
         ++v) {
        _vtx[v] = c + (rad*cosf(v*dTheta))*t0 + (rad*sinf(v*dTheta))*t1;
    }
}


void
Ring::draw(GLfloat lineWidth) const
{
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
    for (std::vector<vec3f >::size_type v = 0;
         v < _vtx.size();
         ++v) {
        glVertex3fv(&_vtx[v][0]);
    }
    glEnd();
}

// -----------------------------------------------------------------------------

Disk::Disk(const vec3f &t0,
           const vec3f &t1,
           GLfloat      radius,
           GLint        subd,
           const vec3f &c)
{
    _vtx.resize(std::max<GLint>(3, subd));

    GLfloat dTheta = deg2rad(360.f/_vtx.size());
    const GLfloat rad(std::max<GLfloat>(0.f, radius));

    for (std::vector<vec3f >::size_type v = 0;
         v < _vtx.size();
         ++v) {
        _vtx[v] = c + (rad*cosf(v*dTheta))*t0 + (rad*sinf(v*dTheta))*t1;
    }

    _nml = em::normalized(em::cross(t0, t1));
}


void
Disk::draw() const
{
    glNormal3fv(&_nml[0]);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3fv(&_vtx[0][0]);
    for (int v = _vtx.size() - 1; v >= 0 ; --v) {
        glVertex3fv(&_vtx[v][0]);
    }
    glVertex3fv(&_vtx[_vtx.size() - 1][0]);
    glEnd();

//    glBegin(GL_LINE_LOOP);
//    for (std::vector<vec3f >::size_type v = 0;
//         v < _vtx.size();
//         ++v) {
//        glVertex3fv(&_vtx[v][0]);
//    }
//    glEnd();
}

// -----------------------------------------------------------------------------

Sphere::Sphere(const GLfloat  radius,
               const GLint    subd,
               const vec3f   &c)
    : _radius(radius),
      _center(c)
{
    const GLint subdh(subd/2);
    const GLfloat factor(2.f*M_PI/subd);
    GLint lat;
    GLint lon;
    GLfloat latf;
    GLfloat lonf;
    vec3f vtx;
    vec3f nml;

    _vtx.reserve(2*subdh*subd);
    _nml.reserve(2*subdh*subd);

    for (lat = 0; lat < subdh; ++lat) {
        for (lon = 0; lon <= subd; ++lon) {
            latf = lat*factor;
            lonf = lon*factor;
            GLfloat sinLat = sin(latf);
            nml[0] = cos(lonf)*sinLat;
            nml[1] = sin(lonf)*sinLat;
            nml[2] = cos(latf);
            vtx[0] = c[0] + radius*nml[0];
            vtx[1] = c[1] + radius*nml[1];
            vtx[2] = c[2] + radius*nml[2];
            _vtx.push_back(vtx);
            _nml.push_back(nml);

            latf = (lat+1)*factor;
            lonf = lon*factor;
            sinLat = sin(latf);
            nml[0] = cos(lonf)*sinLat;
            nml[1] = sin(lonf)*sinLat;
            nml[2] = cos(latf);
            vtx[0] = c[0] + radius*nml[0];
            vtx[1] = c[1] + radius*nml[1];
            vtx[2] = c[2] + radius*nml[2];
            _vtx.push_back(vtx);
            _nml.push_back(nml);
        }
    }
}


void
Sphere::draw() const
{
    glBegin(GL_TRIANGLE_STRIP);
    for (std::vector<vec3f>::size_type v = 0;
         v < _vtx.size();
         ++v) {
        glNormal3fv(&_nml[v][0]);
        glVertex3fv(&_vtx[v][0]);
    }
    glEnd();
}

// -----------------------------------------------------------------------------

Cone::Cone(const vec3f   &axis,
           const vec3f   &t0,
           const vec3f   &t1,
           const GLfloat  h0,
           const GLfloat  h,
           const GLfloat  r,
           const GLint    subd,
           const vec3f   &c)
    : _axis(axis),
      _baseMid(c[0] + axis[0]*std::min<GLfloat>(h0, h),
               c[1] + axis[1]*std::min<GLfloat>(h0, h),
               c[2] + axis[2]*std::min<GLfloat>(h0, h)),
      _top(c[0] + axis[0]*(std::max<GLfloat>(h0, h)),
           c[1] + axis[1]*(std::max<GLfloat>(h0, h)),
           c[2] + axis[2]*(std::max<GLfloat>(h0, h)))
{
    _vtx.resize(std::max<GLint>(3, subd));
    _nml.resize(_vtx.size());

    GLfloat dTheta = deg2rad(360.f/_vtx.size());

    const GLfloat rad(std::max<GLfloat>(0.f, r));
    for (std::vector<vec3f >::size_type v = 0;
         v < _vtx.size();
         ++v) {
        _vtx[v] = _baseMid + (rad*cosf(v*dTheta))*t0 + (rad*sinf(v*dTheta))*t1;
    }

    for (std::vector<vec3f >::size_type n = 0;
         n < _nml.size() - 1;
         ++n) {
        _nml[n] = em::cross(_top - _vtx[n], _top - _vtx[n+1]);
        em::normalize(_nml[n]);
    }

    _nml.back() = em::cross(_top - _vtx.back(), _top - _vtx[0]);
    em::normalize(_nml.back());
}


void
Cone::draw() const
{
    glNormal3f(-_axis[0], -_axis[1], -_axis[2]);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3fv(&_baseMid[0]);
    for (int v = _vtx.size() - 1; v >= 0 ; --v) {
        glVertex3fv(&_vtx[v][0]);
    }
    glVertex3fv(&_vtx[_vtx.size() - 1][0]);
    glEnd();

    glBegin(GL_TRIANGLE_FAN);
    glNormal3f(_axis[0], _axis[1], _axis[2]);
    glVertex3fv(&_top[0]);
    for (std::vector<vec3f >::size_type v = 0;
         v < _vtx.size();
         ++v) {
        glNormal3fv(&_nml[v][0]);
        glVertex3fv(&_vtx[v][0]);
    }
    glNormal3fv(&_nml[0][0]);
    glVertex3fv(&_vtx[0][0]);
    glEnd();
}

// -----------------------------------------------------------------------------

Torus::Torus(//const vec3f   &t0,
             //const vec3f   &t1,
             const GLfloat  innerRadius,
             const GLfloat  outerRadius,
             const GLint    subdSide,
             const GLint    subdRing,
             const vec3f   &c)
{
    const GLint ss(std::max<GLint>(4, subdSide));
    const GLint sr(std::max<GLint>(4, subdRing));

    const GLfloat ringDelta(deg2rad(360.f/sr));
    const GLfloat sideDelta(deg2rad(360.f/ss));

    GLfloat thetaRad(0.f);

    for (GLint r = sr; r >= 0; --r) {
        GLfloat phiRad(0.f);

        // Generate segments having the total number of
        // requested sides

        for (GLint s = ss; s >= 0; --s) {
            phiRad += sideDelta;

            // Begin the segment here with the normal and the vertex

            _vtx.push_back(vertex(thetaRad, phiRad,
                                  innerRadius, outerRadius,
                                  c));
            _nml.push_back(normal(_vtx.back(), thetaRad, outerRadius));

            thetaRad += ringDelta;

            // End the segment here with the normal and the vertex

            _vtx.push_back(vertex(thetaRad, phiRad,
                                  innerRadius, outerRadius,
                                  c));
            _nml.push_back(normal(_vtx.back(), thetaRad, outerRadius));
        }
    }
}


void
Torus::draw() const
{
    glBegin(GL_QUAD_STRIP);
    for (std::vector<vec3f>::size_type v = 0; v < _vtx.size(); ++v) {
        glNormal3fv(&_nml[v][0]);
        glVertex3fv(&_vtx[v][0]);
    }
    glEnd();
}


vec3f
Torus::vertex(const GLfloat thetaRad,
              const GLfloat phiRad,
              const GLfloat innerRadius,
              const GLfloat outerRadius,
              const vec3f  &c) const
{
    const GLfloat factor(outerRadius + innerRadius*cosf(phiRad));

    return c + vec3f( factor*cosf(thetaRad),
                     -factor*sinf(thetaRad),
                      innerRadius*sinf(phiRad));
}


vec3f
Torus::normal(const vec3f   &vtx,
              const GLfloat  thetaRad,
              const GLfloat  outerRadius) const
{
    return em::normalized(vtx - vec3f( outerRadius*cosf(thetaRad),
                                      -outerRadius*sinf(thetaRad),
                                       0.f));
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
