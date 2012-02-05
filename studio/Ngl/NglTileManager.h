// -----------------------------------------------------------------------------
//
// NglTileManager.h
//
// Manage tiles.
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

#ifndef NGL_TILEMANAGER_H
#define NGL_TILEMANAGER_H

#include "NglTypes.h"
#include "NglTile.h"
#include "NglUtils.h"
//#include "em_mat44.h"
//#include "em_vec.h"
//
//#ifdef __APPLE__
//#include <OpenGL/gl.h>
//#else
//#include <GL/gl.h>  // OGL types
//#endif

#include <QtOpenGL>

#include <string>
#include <vector>

#include <NbBody.h>

class QGLShaderProgram;
class QGLContext;

namespace Ngl
{
// -----------------------------------------------------------------------------

// Fwd

class ProxyPlane;
class Viewport;


class TileManager
{
public:

    explicit
    TileManager(GLfloat isoValue            = 0.f,
                GLfloat isoUpperBand        = 0.5f,
                GLfloat isoLowerBand        = 2.f,
                GLfloat texSampleRate       = 1.f,
                GLfloat sliceSpacingZ       = 1.f,
                const vec3f &frontDiffColor = vec3f(0.8f, 0.25f, 0.25f),
                const vec3f &backDiffColor  = vec3f(0.5f, 0.5f, 0.5f),
                const GLenum sliceVboUsage  = GL_STREAM_DRAW);

    ~TileManager();

    // Params

    GLfloat isoValue()            const { return _isoValue;       }
    GLfloat isoUpperBand()        const { return _isoUpperBand;   }
    GLfloat isoLowerBand()        const { return _isoLowerBand;   }
    //GLfloat maxDist()             const { return _maxDist;        }
    GLfloat texSampleRate()       const { return _texSampleRate;  }
    GLfloat sliceSpacingZ()       const { return _sliceSpacingZ;  }
    const vec3f &frontDiffColor() const { return _frontDiffColor; }
    const vec3f &backDiffColor()  const { return _backDiffColor;  }
    GLenum sliceVboUsage()        const { return _sliceVboUsage;  }

    //void setMaxDist(GLfloat maxDist) { _maxDist = maxDist; }

    void setIso(GLfloat isoValue,
                GLfloat isoUpperBand,
                GLfloat isoLowerBand);

    // NB: Has no effect if set after setBody
    void setTexSampleRate(GLfloat texSampleRate)
    { _texSampleRate = texSampleRate; }

    void setSliceSpacingZ(const GLfloat sliceSpacingZ)
    { _sliceSpacingZ = sliceSpacingZ; }

    void setFrontDiffColor(const vec3f &frontDiffColor)
    { _frontDiffColor = frontDiffColor; }

    void setBackDiffColor(const vec3f &backDiffColor)
    { _backDiffColor = backDiffColor; }

    // NB: No error checking
    void setSliceVboUsage(const GLenum sliceVboUsage)
    { _sliceVboUsage = sliceVboUsage; }

    // Stats

    const vec3f &wmin() const { return _wmin;     }
    const vec3f &wmax() const { return _wmax;     }
    GLfloat minEyeZ()   const { return _minEyeZ;  }
    GLfloat maxEyeZ()   const { return _maxEyeZ;  }
    GLfloat minPhi()    const { return _minPhi;   }
    GLfloat maxPhi()    const { return _maxPhi;   }
    GLfloat cellSize()  const { return _cellSize; }
    GLint tileSize()    const { return _tileSize; }

    std::vector<Tile>::size_type tileCount() const
    { return _tileVec.size(); }

    std::vector<Tile*>::size_type isoTileCount() const
    { return _isoTileVec.size(); }

    std::vector<Tile*>::size_type visTileCount() const
    { return _visTileVec.size(); }

    std::vector<Tile*>::size_type bBoxTileCount() const
    { return _bBoxTileVec.size(); }


    bool hasTiles()     const { return !_tileVec.empty(); }
    bool hasIsoTiles()  const { return !_isoTileVec.empty(); }
    bool hasVisTiles()  const { return !_visTileVec.empty(); }
    bool hasBBoxTiles() const { return !_bBoxTileVec.empty(); }
    bool hasShader()    const { return 0 != _shader; }

    void setShader(const QGLContext *context,
                   const std::string& shader);

    void setBody(const Nb::Body *body, const std::string &channelName);

    void setBBox(const vec3f &t,
                 const vec3f &r,
                 const vec3f &s);

    void project(const mat44f   &mv,
                 const mat44f   &p,
                 const Viewport &vp);

    void slice();

    void draw();

private:

    // Disabled

    TileManager(const TileManager&);
    TileManager& operator=(const TileManager&);

private:

    // TileManager is responsible for when these functions get called

    void uploadSliceVbo();

    void uploadTex(const Tile &tile);

private:

    std::vector<Tile> _tileVec;
    std::vector<Tile*> _isoTileVec;
    std::vector<Tile*> _bBoxTileVec;
    std::vector<Tile*> _visTileVec;
    std::vector<ProxyPlane> _ppVec;

    // Shading

    QGLShaderProgram* _shader;
    GLint _phiTexLoc;
    GLint _cellSizeLoc;
    GLint _isoValueLoc;
    GLint _isoUpperBandLoc;
    GLint _isoLowerBandLoc;
    GLint _maxPhiLoc;
    GLint _minPhiLoc;
    GLint _frontDiffColorLoc;
    GLint _backDiffColorLoc;
    GLint _invBBoxXFormLoc;

    // Params

    GLfloat _isoValue;                      // [uniform]
    //GLfloat _maxDist;                       // [uniform]
    GLfloat _isoUpperBand;                  // [uniform] In _cellSize units
    GLfloat _isoLowerBand;                  // [uniform] In _cellSize units
    GLfloat _texSampleRate;
    GLfloat _sliceSpacingZ;                 // Eye space
    vec3f _frontDiffColor;     // [uniform]
    vec3f _backDiffColor;      // [uniform]
    GLenum _sliceVboUsage;

    mat44f _invBBoxXForm;   // TMP!! [uniform]

    // Stats

    vec3f _wmin;       // All tiles
    vec3f _wmax;
    GLfloat _minEyeZ;               // Iso-tiles
    GLfloat _maxEyeZ;
    GLfloat _minPhi;                // [uniform] All tiles
    GLfloat _maxPhi;                // [uniform]
    GLfloat _cellSize;              // [uniform] Same for all tiles
    GLint _tileSize;                // Same for all tiles

    // Handles

    std::vector<GLuint> _texIdVec;
    GLuint _sliceVboDataId;
    GLuint _sliceVboIdxId;


    // TMP!!
    std::string _shaderStr;
    bool        _ghost;
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif // NGL_TILEMANAGER_H
