// -----------------------------------------------------------------------------
//
// NglTile.cc
//
// Holds information about scalar field data from a Ng::FieldShape.
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

#include "NglTile.h"
#include "NglProxyPlane.h"
#include "NglViewport.h"

#include <NbTileLayout.h>
#include <NbFieldChannel1f.h>

#include <limits>
#include <algorithm>    // std::sort


namespace
{
// -----------------------------------------------------------------------------

// Linear interpolation on one dimension.

GLfloat
lerp(const GLfloat v0, const GLfloat v1, const GLfloat t)
{
    return ((1.f - t)*v0 + t*v1);
}


// Component-wise linear interpolation on three dimensions.

Ngl::vec3f
lerp(const Ngl::vec3f &v0,
     const Ngl::vec3f &v1,
     const GLfloat t)
{
    return Ngl::vec3f(lerp(v0[0], v1[0], t),
                      lerp(v0[1], v1[1], t),
                      lerp(v0[2], v1[2], t));
}


// Instead of using atan2 to sort vertices we use pseudoangles.
// These angles have the property that the sorting order is the same
// as atan2 but are cheaper to compute.
//
// Note that we are currently not checking 0 == dx == dy.

//GLfloat
//pseudoAngle(const GLfloat dx, const GLfloat dy)
//{
//    const GLfloat r(dy/(abs(dx) + abs(dy)));
//
//    if (dx >= 0.f) //{
//        return 1.f + r;
//    //}
//    //else {
//        return 3.f - r;
//    //}
//}


// Simple struct for associating a vertex (by index) with an angle
// for CCW sorting.

struct CCW
{
    GLint i;
    GLfloat theta;
};


// Operator used by std::sort

bool
operator<(const CCW &lhs, const CCW &rhs)
{
    if (lhs.theta < rhs.theta)
        return true;
    return false;
}

// -----------------------------------------------------------------------------
}   // Namespace: Anonymous


namespace Ngl
{
// -----------------------------------------------------------------------------

// Vertex indices for cube edges.

vec2i Tile::edge[12] = {
    vec2i(0, 1), vec2i(1, 2), vec2i(2, 3),
    vec2i(3, 0), vec2i(4, 5), vec2i(5, 6),
    vec2i(6, 7), vec2i(7, 4), vec2i(0, 4),
    vec2i(1, 5), vec2i(2, 6), vec2i(3, 7)
};


// edgeIntersect
// -------------
//! Compute the intersection of a line between two points and a plane.
//! The intersection is stored in 'u' and is the normalized distance
//! between v1 & v2. If the plane does not intersect the line between
//! v1 & v2 or if the line is parallel to the plane 'false' is returned.

bool
Tile::edgeIntersect(const ProxyPlane &pp,
                    const vec3f      &v1,
                    const vec3f      &v2,
                    GLfloat          *u)
{
    const GLfloat denom(pp.a()*(v1[0] - v2[0]) +
                        pp.b()*(v1[1] - v2[1]) +
                        pp.c()*(v1[2] - v2[2]));

    if (0.f == denom)
        return false;   // The edge is parallel to the plane

    const GLfloat nom(pp.a()*v1[0] +
                      pp.b()*v1[1] +
                      pp.c()*v1[2] +
                      pp.z());

    *u = nom/denom;

    if (0.f >= *u || *u >= 1.f)
        return false;   // Intersection is not on line segment

    return true;
}

// -----------------------------------------------------------------------------

// Tile
// ----
//! Initialize spatial information for this tile.
//! * Object coordinates are fetched from the layout by the TileManager.
//! * Texture coordinates are set to [0,1] in each dimension.
//! * Eye coordinates are initially zero and will be set when the tile
//!   is projected.
//! * Window coordinates are also set upon projection.

Tile::Tile(const vec3f &wmin,
           const vec3f &wmax,
           const GLuint texId)
    : _vboIdxOffsetSize(0),
      _vboIdxStart(0),
      _vboIdxEnd(0),
      _texId(texId),
      _texData(0, 0, 0),
      _minEyeZ( std::numeric_limits<GLfloat>::max()),
      _maxEyeZ(-std::numeric_limits<GLfloat>::max()),
      _minPhi( std::numeric_limits<GLfloat>::max()),
      _maxPhi(-std::numeric_limits<GLfloat>::max()),
      _visVtx(0)
{
    _vtx[0].obj = wmin;
    _vtx[0].tex = vec3f( 0.f,  0.f, 0.f);
    _vtx[0].eye = vec3f( 0.f,  0.f, 0.f);
    _vtx[0].win = vec2f(-1.f, -1.f);

    _vtx[1].obj = vec3f(wmax[0], wmin[1], wmin[2]);
    _vtx[1].tex = vec3f( 1.f,  0.f, 0.f);
    _vtx[1].eye = vec3f( 0.f,  0.f, 0.f);
    _vtx[1].win = vec2f(-1.f, -1.f);

    _vtx[2].obj = vec3f(wmax[0], wmin[1], wmax[2]);
    _vtx[2].tex = vec3f( 1.f,  0.f, 1.f);
    _vtx[2].eye = vec3f( 0.f,  0.f, 0.f);
    _vtx[2].win = vec2f(-1.f, -1.f);

    _vtx[3].obj = vec3f(wmin[0], wmin[1], wmax[2]);
    _vtx[3].tex = vec3f( 0.f,  0.f, 1.f);
    _vtx[3].eye = vec3f( 0.f,  0.f, 0.f);
    _vtx[3].win = vec2f(-1.f, -1.f);

    _vtx[4].obj = vec3f(wmin[0], wmax[1], wmin[2]);
    _vtx[4].tex = vec3f( 0.f,  1.f, 0.f);
    _vtx[4].eye = vec3f( 0.f,  0.f, 0.f);
    _vtx[4].win = vec2f(-1.f, -1.f);

    _vtx[5].obj = vec3f(wmax[0], wmax[1], wmin[2]);
    _vtx[5].tex = vec3f( 1.f,  1.f, 0.f);
    _vtx[5].eye = vec3f( 0.f,  0.f, 0.f);
    _vtx[5].win = vec2f(-1.f, -1.f);

    _vtx[6].obj = wmax;
    _vtx[6].tex = vec3f( 1.f,  1.f, 1.f);
    _vtx[6].eye = vec3f( 0.f,  0.f, 0.f);
    _vtx[6].win = vec2f(-1.f, -1.f);

    _vtx[7].obj = vec3f(wmin[0], wmax[1], wmax[2]);
    _vtx[7].tex = vec3f( 0.f,  1.f, 1.f);
    _vtx[7].eye = vec3f( 0.f,  0.f, 0.f);
    _vtx[7].win = vec2f(-1.f, -1.f);
}


// ~Tile
// -----
//! No resources to free.

Tile::~Tile()
{
}


// setVboIdxOffsetSize
// -------------------
//! Set the offset in bytes to where this tiles index data starts
//! in the master VBO maintaned by the TileManager.

void
Tile::setVboIdxOffsetSize(const GLuint offset)
{
    _vboIdxOffsetSize = offset;
}


// shiftVboIdx
// -----------
//! Shift the VBO indices of this tile by some integer value. This is
//! necessary in the TileManager because tile indices are initially local,
//! not taking into account vertices from other tiles.

void
Tile::shiftVboIdx(const GLuint i)
{
    const std::vector<GLuint>::size_type size(_vboIdx.size());
    for (std::vector<GLuint>::size_type idx = 0; idx < size; ++idx) {
        _vboIdx[idx] += i;
    }

    _vboIdxStart += i;
    _vboIdxEnd += i;
}


// sampleTexData
// -------------
//! Extract and store texture data from a field channel.

void
Tile::sampleTexData(const Nb::TileLayout     &layout,
                    const Nb::FieldChannel1f &fch,
                    const int                 ni,
                    const int                 nj,
                    const int                 nk)
{
    // Reset and allocate memory for texture data. Texture dimensions must
    // be at least 2x2x2.

    _texData.resize(std::max(2, ni), std::max(2, nj), std::max(2, nk));

    _minPhi =  std::numeric_limits<GLfloat>::max();
    _maxPhi = -std::numeric_limits<GLfloat>::max();

    // We sample along the edges of the tile and internally. This means we
    // can set the texture coordinates at the corners to be [0,1].
    //
    // Vertex[0] is min(world).
    // Vertex[6] is max(world).

    const vec3f wdelta(
            (_vtx[6].obj[0] - _vtx[0].obj[0])/(_texData.ni - 1),
            (_vtx[6].obj[1] - _vtx[0].obj[1])/(_texData.nj - 1),
            (_vtx[6].obj[2] - _vtx[0].obj[2])/(_texData.nk - 1));

    GLfloat phi;
    vec3f gradPhi;
    int k = 0;
    int j = 0;
    int i = 0;

    for (k = 0; k < _texData.nk; ++k) {
        for (j = 0; j < _texData.nj; ++j) {
            for (i = 0; i < _texData.ni; ++i) {
                const vec3f wsx(_vtx[0].obj[0] + i*wdelta[0],
                                _vtx[0].obj[1] + j*wdelta[1],
                                _vtx[0].obj[2] + k*wdelta[2]);

                // Use the field channel to compute gradient and value.

                phi = fch.sampleGradientLinear(wsx, layout, gradPhi);
                //phi = fch.sampleGradientQuadratic(wsx, layout, gradPhi);

                // HACK!!
                //if(phi>EM_BIG_FLOAT/2)
                //    phi=wdelta[0];

                // Texture is RGBA, where RGB is the gradient (nx, ny, nz)
                // and A is the phi-value (distance).

                _texData(i, j, k) = vec4f(gradPhi[0],
                                          gradPhi[1],
                                          gradPhi[2],
                                          phi);

                _minPhi = std::min(_minPhi, phi);
                _maxPhi = std::max(_maxPhi, phi);
            }
        }
    }
}


// project
// -------
//! Compute eye and window coordinates for this tile given camera information.
//! The results of the computations are stored in tile member variables, which
//! means that we can parallelize these computations when projecting
//! multiple tiles.

void
Tile::project(const mat44f   &mv,
              const mat44f   &p,
              const Viewport &vp)
{
    // Reset state

    _minEyeZ =  std::numeric_limits<GLfloat>::max();
    _maxEyeZ = -std::numeric_limits<GLfloat>::max();
    _visVtx = 0;

    // Apply transformations to every vertex of the tile (cube)

    for (int v = 0; v < 8; ++v) {
        // Apply modelview transformation, hard-coded matrix multiplication

        _vtx[v].eye[0] = mv[0][0]*_vtx[v].obj[0] +
                         mv[1][0]*_vtx[v].obj[1] +
                         mv[2][0]*_vtx[v].obj[2] + mv[3][0];
        _vtx[v].eye[1] = mv[0][1]*_vtx[v].obj[0] +
                         mv[1][1]*_vtx[v].obj[1] +
                         mv[2][1]*_vtx[v].obj[2] + mv[3][1];
        _vtx[v].eye[2] = mv[0][2]*_vtx[v].obj[0] +
                         mv[1][2]*_vtx[v].obj[1] +
                         mv[2][2]*_vtx[v].obj[2] + mv[3][2];

        // Store the Z-range in eye-space

        _minEyeZ = std::min<GLfloat>(_minEyeZ, _vtx[v].eye[2]);
        _maxEyeZ = std::max<GLfloat>(_maxEyeZ, _vtx[v].eye[2]);

        // Homogeneous coordinate

        const GLfloat w(mv[0][3]*_vtx[v].obj[0] +
                        mv[1][3]*_vtx[v].obj[1] +
                        mv[2][3]*_vtx[v].obj[2] + mv[3][3]);

        // Use projection transformation to bring eye coords
        // into clip coords. We don't bother with the z-value
        // from here on. Hard-coded matrix multiplication.

        const vec3f clip(
            p[0][0]*_vtx[v].eye[0] +
            p[1][0]*_vtx[v].eye[1] +
            p[2][0]*_vtx[v].eye[2] + p[3][0]*w,
            p[0][1]*_vtx[v].eye[0] +
            p[1][1]*_vtx[v].eye[1] +
            p[2][1]*_vtx[v].eye[2] + p[3][1]*w,
            // NB: Ignoring z-coord.
            p[0][3]*_vtx[v].eye[0] +
            p[1][3]*_vtx[v].eye[1] +
            p[2][3]*_vtx[v].eye[2] + p[3][3]*w);

        // Normalized device coordinates. If invalid clip[2] we stop here,
        // vertex is not visible.

        if (0.f != clip[2]) {
            const vec2f ndc(clip[0]/clip[2], clip[1]/clip[2]);

            // Window (viewport) coordinates.

            _vtx[v].win[0] = vp.x() + 0.5f*vp.width()*(ndc[0] + 1.f);
            _vtx[v].win[1] = vp.y() + 0.5f*vp.height()*(ndc[1] + 1.f);

            // Check if the vertex is on the screen. Note that the vertex
            // must be in front of the camera, i.e. its z-coord in eye-space
            // must be less than zero.

            if (vp.x() < _vtx[v].win[0] &&
                _vtx[v].win[0] < (vp.x() + vp.width()) &&
                vp.y() < _vtx[v].win[1] &&
                _vtx[v].win[1] < (vp.y() + vp.height()) &&
                0.f > _vtx[v].eye[2]) {
                ++_visVtx;
            }
        }
        else {
            // Set window coordinates to be off-screen

            _vtx[v].win[0] = vp.width() + 1.f;
            _vtx[v].win[1] = vp.height() + 1.f;
        }
    }
}


// slice
// -----
//! Generate slice triangles given a set of planes. The results are stored
//! locally in tile members so we can parallelize slicing for multiple tiles.

void
Tile::slice(const std::vector<ProxyPlane> &pp)
{
    // Reset

    _vboData.clear();
    _vboIdx.clear();
    _vboIdxStart = 0;
    _vboIdxEnd = 0;

    // We assume that the planes are sorted by distance from eye.
    // This lets us do a binary search to find the first plane that
    // can intersect this tile. We use the tile's range in eye-space
    // to determine when to stop testing planes.

    typedef std::vector<ProxyPlane>::const_iterator IterType;

    IterType iter(std::lower_bound(pp.begin(), pp.end(), _minEyeZ));

    const IterType iend(pp.end());
    while (iter != iend && iter->z() < _maxEyeZ) {
        planeIntersect(*iter);
        ++iter;
    }
}


// planeIntersect
// --------------
//! Take a plane and find its intersection points with tile edges. The
//! intersection points become the vertices of the triangles that are
//! used as proxy geometry (i.e. the slices).

bool
Tile::planeIntersect(const ProxyPlane &pp)
{
    int e = 0;        // Edge index, loop variable
    int i = 0;        // Number of intersections
    GLfloat u = 0.f;  // LERP variable
    VboVtx vboVtx[6];
    vec3f eyeVtx[6];

    // Check intersection with all tile edges, or until 6
    // intersections have been found.

    while (i < 6 && e < 12) {
        if (Tile::edgeIntersect(pp,
                                _vtx[Tile::edge[e][0]].eye,
                                _vtx[Tile::edge[e][1]].eye, &u)) {
            // If edge intersects, interpolate intersection point
            // from tile corners.

            vboVtx[i].vtx = lerp(_vtx[Tile::edge[e][0]].obj,
                                 _vtx[Tile::edge[e][1]].obj, u);
            vboVtx[i].tex = lerp(_vtx[Tile::edge[e][0]].tex,
                                 _vtx[Tile::edge[e][1]].tex, u);

            // Interpolate eye-coordinates for CCW sorting.

            eyeVtx[i] = lerp(_vtx[Tile::edge[e][0]].eye,
                             _vtx[Tile::edge[e][1]].eye, u);
            ++i;
        }
        ++e;
    }

    // There must be either 3, 4, 5 or 6 intersections.

    if (3 > i)
        return false;

    // Compute the center of the intersection points in eye-space. This
    // point is used to sort the vertices in CCW order.

    vec3f eyeCenter = eyeVtx[0];
    for (int v = 1; v < i; ++v) {
        eyeCenter += eyeVtx[v];
    }
    eyeCenter *= (1.f/i);

    // Each intersection point is associated with its angle to the center,
    // in eye-space.

    CCW ccw[6];
    for (int v = 0; v < i; ++v) {

        ccw[v].i = v;
        //ccw[v].theta = pseudoAngle(eyeCenter[0] - eyeVtx[v][0],
        //                           eyeCenter[1] - eyeVtx[v][1]);
        ccw[v].theta = atan2(eyeCenter[1] - eyeVtx[v][1],
                             eyeCenter[0] - eyeVtx[v][0]);
    }

    std::sort(ccw, ccw + i);

    // Add vertices to VBO cache in sorted order.

    for (int v = 0; v < i; ++v) {
        _vboData.push_back(vboVtx[ccw[v].i]);
    }

    // Build index buffer using slightly different methods depending
    // on number of intersections.

    if (3 == i) {
        // 1 triangle

        _vboIdx.push_back(_vboData.size() - 3);
        _vboIdx.push_back(_vboData.size() - 2);
        _vboIdx.push_back(_vboData.size() - 1);
    }
    else if (4 == i) {
        // 2 triangles

        _vboIdx.push_back(_vboData.size() - 4);
        _vboIdx.push_back(_vboData.size() - 3);
        _vboIdx.push_back(_vboData.size() - 2);
        _vboIdx.push_back(_vboData.size() - 4);
        _vboIdx.push_back(_vboData.size() - 2);
        _vboIdx.push_back(_vboData.size() - 1);
    }
    else if (5 == i || 6 == i) {
        // Tesselation required.
        // Compute the center of the intersection points.

        VboVtx center(vboVtx[0].vtx, vboVtx[0].tex);

        for (int v = 1; v < i; ++v) {
            center.vtx += vboVtx[v].vtx;
            center.tex += vboVtx[v].tex;
        }

        const GLfloat invSize(1.f/i);
        center.vtx *= invSize;
        center.tex *= invSize;

        _vboData.push_back(center);

        for (int t = 0; t < (i - 1); ++t) {
            _vboIdx.push_back(_vboData.size() - 1);
            _vboIdx.push_back(_vboData.size() - (i + 1) + t);
            _vboIdx.push_back(_vboData.size() -  i + t);
        }
        _vboIdx.push_back(_vboData.size() - 1);
        _vboIdx.push_back(_vboData.size() - 2);
        _vboIdx.push_back(_vboData.size() - (i + 1));
    }

    _vboIdxEnd = _vboData.size() - 1;

    return true;    // Plane intersects tile
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.
