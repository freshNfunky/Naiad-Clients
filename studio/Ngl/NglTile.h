// -----------------------------------------------------------------------------
//
// NglTile.h
//
// Holds information about scalar field data from a ng::FieldShape.
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

#ifndef NGL_TILE_H
#define NGL_TILE_H

#include "NglTypes.h"
#include <em_array3.h>
//#include <em_mat44.h>
//#include <em_vec.h>
//#ifdef __APPLE__
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>  // OGL types
//#endif
#include <vector>

#include <NbTileLayout.h>
#include <NbFieldChannel1f.h>

namespace Ngl
{
// -----------------------------------------------------------------------------

// TODO:
// * Tile should not need to know the GL-texture Id. We wish to explicitly
//   avoid GL related information since this causes problems when parallelizing
//   Tile-operations.
//
// * It doesn't make sense to copy/assign Tiles if they are responsible for
//   resources, such as GL-handles.
//
// * Currently, a tile must have at least one corner on screen to be visible.
//   This does not handle perfectly the case when the camera is inside the
//   Tile, looking out through one of the faces, in which case part of the
//   Tile contents may be visible despite the fact that no corners are on
//   screen.
//
// * Texture coordinates of cube vertices never change and should be static.


// Fwd

class ProxyPlane;
class Viewport;


class Tile
{
public:

    static vec2i edge[12];

    static bool edgeIntersect(const ProxyPlane &pp,
                              const vec3f      &v1,
                              const vec3f      &v2,
                              GLfloat          *u);

public:

    struct Vtx
    {
        // One for each corner of the 8 corners of a tile. Stores
        // positions in various coordinate systems.

        vec3f obj;
        vec3f tex;
        vec3f eye;
        vec2f win;
    };


    struct VboVtx
    {
        // It has been reported in some cases that memory alignment gives
        // increased performance, which is why we align our VBO data to a
        // 32 byte boundary. Vertex and texture data are stored in the same
        // VBO, managed by the TileManager.

        VboVtx() {}     // Uninitialized

        explicit
        VboVtx(const vec3f &v, const vec3f &t)
            : vtx(v), tex(t)
        {}

        // Members

        vec3f vtx;
        vec3f tex;

    private:

        // Align to 32 byte boundary

        GLbyte padding[32 - 2*sizeof(vec3f)];
    };

public:

    explicit
    Tile(const vec3f &wmin,
         const vec3f &wmax,
         GLuint       texId);

    ~Tile();

    // Default copy & assign

    const Vtx &operator[](const int v) const { return _vtx[v]; }
    const Vtx &vtx(const int v)        const { return _vtx[v]; }

    const VboVtx *vboData() const { return &_vboData[0]; }
    const GLuint *vboIdx()  const { return &_vboIdx[0]; }

    // (VBO) Data

    GLuint vboDataCount()     const { return _vboData.size(); }
    GLsizei vboDataSize()     const { return sizeof(VboVtx)*_vboData.size(); }
    GLint vboDataOffsetSize() const { return sizeof(VboVtx)*_vboIdxStart; }

    // (VBO) Idx

    GLuint vboIdxStart()     const { return _vboIdxStart; }
    GLuint vboIdxEnd()       const { return _vboIdxEnd; }
    GLsizei vboIdxCount()    const { return _vboIdx.size(); }
    GLsizei vboIdxSize()     const { return sizeof(GLuint)*_vboIdx.size(); }
    GLint vboIdxOffsetSize() const { return _vboIdxOffsetSize; }

    void setVboIdxOffsetSize(GLuint offset);
    void shiftVboIdx(GLuint i);

    GLuint texId()                     const { return _texId; }
    const em::array3<vec4f> &texData() const { return _texData; }

    GLfloat minEyeZ() const { return _minEyeZ; }
    GLfloat maxEyeZ() const { return _maxEyeZ; }
    GLfloat minPhi()  const { return _minPhi; }
    GLfloat maxPhi()  const { return _maxPhi; }
    bool isVisible()  const { return (0 < _visVtx); }

    const vec3f &wmin()   const { return _vtx[0].obj; }
    const vec3f &wmax()   const { return _vtx[6].obj; }
    const vec3f &texmin() const { return _vtx[0].tex; }
    const vec3f &texmax() const { return _vtx[6].tex; }
    GLfloat eyeZRange()   const { return (_maxEyeZ - _minEyeZ); }

    void sampleTexData(const Nb::TileLayout     &layout,
                       const Nb::FieldChannel1f &fch,
                       int                       ni,
                       int                       nj,
                       int                       nk);

    void project(const mat44f   &mv,
                 const mat44f   &p,
                 const Viewport &vp);

    void slice(const std::vector<ProxyPlane> &pp);

private:

    Tile();     // Disabled.

    bool planeIntersect(const ProxyPlane &pp);

private:
    std::vector<VboVtx> _vboData;
    std::vector<GLuint> _vboIdx;
    GLint _vboIdxOffsetSize;     // [bytes]
    GLuint _vboIdxStart;
    GLuint _vboIdxEnd;

    Vtx _vtx[8];                 // Cube corners

    GLuint _texId;
    em::array3<vec4f> _texData;

    GLfloat _minEyeZ;
    GLfloat _maxEyeZ;
    GLfloat _minPhi;
    GLfloat _maxPhi;
    int _visVtx;
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_TILE_H
