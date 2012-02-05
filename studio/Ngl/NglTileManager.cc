// -----------------------------------------------------------------------------
//
// NglTileManager.cc
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

//#define GL_GLEXT_PROTOTYPES

#include "NglState.h"
#include "NglTileManager.h"
#include "NglProxyPlane.h"
#include "NglViewport.h"
#include "NglExtensions.h"

#include <NbBody.h>
#include <Nbx.h>

//#include <QtOpenGL>
//#include <QGLShaderProgram>

#include <em_glmat44_algo.h>

#include <sstream>
#include <limits>
//#include <omp.h>

#include <iostream>

#ifndef WIN32
#include <sys/time.h>
#endif

#if 0

namespace Ngl
{
// -----------------------------------------------------------------------------

// TileManager
// -----------
//! Create object.

TileManager::TileManager(const GLfloat  isoValue,
                         const GLfloat  isoUpperBand,
                         const GLfloat  isoLowerBand,
                         const GLfloat  texSampleRate,
                         const GLfloat  sliceSpacingZ,
                         const vec3f   &frontDiffColor,
                         const vec3f   &backDiffColor,
                         const GLenum   sliceVboUsage)
    : _shader(0),
      _phiTexLoc(-1),
      _cellSizeLoc(-1),
      _isoValueLoc(-1),
      _isoUpperBandLoc(-1),
      _isoLowerBandLoc(-1),
      _maxPhiLoc(-1),
      _minPhiLoc(-1),
      _frontDiffColorLoc(-1),
      _backDiffColorLoc(-1),
      _invBBoxXFormLoc(-1),
      _isoValue(isoValue),
      //_maxDist(1),
      _isoUpperBand(isoUpperBand),
      _isoLowerBand(isoLowerBand),
      _texSampleRate(texSampleRate),
      _sliceSpacingZ(sliceSpacingZ),
      _frontDiffColor(frontDiffColor),
      _backDiffColor(backDiffColor),
      _sliceVboUsage(sliceVboUsage),
      _invBBoxXForm(1.f),
      _wmin( std::numeric_limits<GLfloat>::max(),
             std::numeric_limits<GLfloat>::max(),
             std::numeric_limits<GLfloat>::max()),
      _wmax(-std::numeric_limits<GLfloat>::max(),
            -std::numeric_limits<GLfloat>::max(),
            -std::numeric_limits<GLfloat>::max()),
      _minEyeZ( std::numeric_limits<GLfloat>::max()),
      _maxEyeZ(-std::numeric_limits<GLfloat>::max()),
      _minPhi( std::numeric_limits<GLfloat>::max()),
      _maxPhi(-std::numeric_limits<GLfloat>::max()),
      _cellSize(1.f),
      _tileSize(1),
      _sliceVboDataId(0),
      _sliceVboIdxId(0)
{
}


// ~TileManager
// ------------
//! Destroy object and free resources.

TileManager::~TileManager()
{
    delete _shader;
    glDeleteTextures(_texIdVec.size(), &_texIdVec[0]);
    glDeleteBuffers(1, &_sliceVboDataId);
    glDeleteBuffers(1, &_sliceVboIdxId);
    //glDeleteBuffers(1, &_lineVboDataId);
    //glDeleteBuffers(1, &_lineVboIdxId);
}


// setShader
// ---------
//! Set the shader to be used when drawing slices.

void
TileManager::setShader(const QGLContext*  context,
                       const std::string& shader)
{
    delete _shader;
    _shader = new QGLShaderProgram(context);
    _phiTexLoc = -1;
    _cellSizeLoc = -1;
    _isoValueLoc = -1;
    _isoUpperBandLoc = -1;
    _isoLowerBandLoc = -1;
    _maxPhiLoc = -1;
    _minPhiLoc = -1;
    _frontDiffColorLoc = -1;
    _backDiffColorLoc = -1;
    _invBBoxXFormLoc = -1;

    std::stringstream ss;
    ss << qgetenv("NAIAD_PATH") << "/shaders/" << shader;
    const std::string vertex(ss.str()+".vs");
    const std::string frag(ss.str()+".fs");

    QGLShader vShader(QGLShader::Vertex);
    if (!vShader.compileSourceFile(vertex.c_str())) {
        delete _shader;
        _shader = 0;
        NB_THROW("Error compiling vertex shader '" << vertex << "': " <<
                 vShader.log().toStdString());
    }

    if (!_shader->addShader(&vShader)) {
        delete _shader;
        _shader = 0;
        NB_THROW("Error adding vertex shader '" << vertex << "'");
    }

#if 0
    std::cerr << "Vertex shader log: '" << vShader.log().toStdString() << "'\n";
#endif

    QGLShader fShader(QGLShader::Fragment);
    if (!fShader.compileSourceFile(frag.c_str())) {
        delete _shader;
        _shader = 0;
        NB_THROW("Error compiling fragment shader '" << frag << "': " <<
                 fShader.log().toStdString());
    }

    if (!_shader->addShader(&fShader)) {
        delete _shader;
        _shader = 0;
        NB_THROW("Error adding fragment shader '" << frag << "'");
    }

#if 1
    std::cerr << "Fragment shader log: '" << fShader.log().toStdString() << "'\n";
#endif

    if (!_shader->link()) {
        delete _shader;
        _shader = 0;
        NB_THROW("Error linking shader: " <<
                 _shader->log().toStdString());
    }

    qDebug() << "TileManager::setShader: '" << vertex.c_str() << "' '" <<
                                               frag.c_str() << "'";

    // HACK!! TMP!!
    _shaderStr = shader;
    _ghost=false;
    if ("iso" == _shaderStr) {
        _phiTexLoc          = _shader->uniformLocation("uPhiTex");
        _cellSizeLoc        = _shader->uniformLocation("uCellSize");
        _isoValueLoc        = _shader->uniformLocation("uIsoValue");
        _isoUpperBandLoc    = _shader->uniformLocation("uIsoUpperBand");
        _isoLowerBandLoc    = _shader->uniformLocation("uIsoLowerBand");
        _frontDiffColorLoc  = _shader->uniformLocation("uFrontDiffColor");
        _backDiffColorLoc   = _shader->uniformLocation("uBackDiffColor");
        _invBBoxXFormLoc    = _shader->uniformLocation("uInvBBoxXForm");
    }
    else if ("iso-distance" == _shaderStr) {
        _phiTexLoc         = _shader->uniformLocation("uPhiTex");
        _maxPhiLoc         = _shader->uniformLocation("uMaxPhi");
        _minPhiLoc         = _shader->uniformLocation("uMinPhi");
        _invBBoxXFormLoc   = _shader->uniformLocation("uInvBBoxXForm");
    } else if( "ghost" == _shaderStr) {
        _phiTexLoc         = _shader->uniformLocation("uPhiTex");
        _invBBoxXFormLoc   = _shader->uniformLocation("uInvBBoxXForm");
        _ghost=true;
    }
}

// setBody
// -------
//! Create internal data-structures from information in a body.

void
TileManager::setBody(const Nb::Body*    body,
                     const std::string &channelName)
{
#ifndef WIN32
    std::stringstream ss;
    ss << "TileManager::setBody: '" << this << "': '"
       << body->name() << "' " << body << " : '" << channelName << "': ";

    timeval tv0;
    timeval tv1;

    gettimeofday(&tv0, 0);
#endif

    const Nb::TileLayout& layout(body->constLayout());
    const int fineTileCount(layout.fineTileCount());
    const int coarseTileCount(layout.coarseTileCount());
    const int allTileCount(fineTileCount+coarseTileCount);

    _tileSize = layout.tileSize();
    _cellSize = layout.cellSize();

    _tileVec.clear();
    _tileVec.reserve(allTileCount);
    _isoTileVec.clear();
    _isoTileVec.reserve(allTileCount);
    _visTileVec.clear();
    _visTileVec.reserve(allTileCount);

    // Generate a texture Id for each tile

    glDeleteTextures(_texIdVec.size(), &_texIdVec[0]);
    _texIdVec.resize(allTileCount, 0);
    glGenTextures(_texIdVec.size(), &_texIdVec[0]);

    _wmin[0] =  std::numeric_limits<GLfloat>::max();
    _wmin[1] =  std::numeric_limits<GLfloat>::max();
    _wmin[2] =  std::numeric_limits<GLfloat>::max();
    _wmax[0] = -std::numeric_limits<GLfloat>::max();
    _wmax[1] = -std::numeric_limits<GLfloat>::max();
    _wmax[2] = -std::numeric_limits<GLfloat>::max();

    vec3f wmin;
    vec3f wmax;

    // Create internal representation of tiles and compute
    // world-space bounding box of all tiles

    for (int t = 0; t < fineTileCount; ++t) {
        Nb::FineTile fineTile(layout.fineTile(t));
        fineTile.bounds(wmin, wmax);

        _tileVec.push_back(
            Tile(wmin, wmax, _texIdVec[t])
            );

        _wmin[0] = std::min(_wmin[0], wmin[0]);
        _wmin[1] = std::min(_wmin[1], wmin[1]);
        _wmin[2] = std::min(_wmin[2], wmin[2]);
        _wmax[0] = std::max(_wmax[0], wmax[0]);
        _wmax[1] = std::max(_wmax[1], wmax[1]);
        _wmax[2] = std::max(_wmax[2], wmax[2]);
    }

    for (int t = 0; t < coarseTileCount; ++t) {
        Nb::CoarseTile coarseTile(layout.coarseTile(t));
        coarseTile.bounds(wmin, wmax);

        _tileVec.push_back(
            Tile(wmin, wmax, _texIdVec[t+fineTileCount])
            );

        _wmin[0] = std::min(_wmin[0], wmin[0]);
        _wmin[1] = std::min(_wmin[1], wmin[1]);
        _wmin[2] = std::min(_wmin[2], wmin[2]);
        _wmax[0] = std::max(_wmax[0], wmax[0]);
        _wmax[1] = std::max(_wmax[1], wmax[1]);
        _wmax[2] = std::max(_wmax[2], wmax[2]);
    }

#if 0
    std::cerr
    << "\n"
    << ss.str() << "Tile Count: " << _tileVec.size() << " | Iso: "
        << _isoTileVec.size() << " | Vis: " << _visTileVec.size() << "\n"
    << ss.str() << "Tile Size: " << _tileSize << "\n"
    << ss.str() << "Cell Size: " << _cellSize << "\n"
    << ss.str()
        << (_tileSize*_tileSize*_tileSize)*_tileVec.size() << " voxels\n"
        << ss.str() << "Tex Id: [" << _texIdVec[0] << ", " << _texIdVec.back() << "]\n"
    << ss.str() << "Wx: [" << _wmin[0] << ", " << _wmax[0] << "]\n"
    << ss.str() << "Wy: [" << _wmin[1] << ", " << _wmax[1] << "]\n"
    << ss.str() << "Wz: [" << _wmin[2] << ", " << _wmax[2] << "]\n";
#endif


#ifndef WIN32
    timeval s0;
    timeval s1;

    gettimeofday(&s0, 0);
#endif

    // Get the field channel to visualize from the given body
    // TODO: Check that the fieldshape has a field_channel1f with the
    //       specified name.

    const Nb::FieldShape& fsh(body->constFieldShape());
    const Nb::FieldChannel1f& fch(fsh.constChannel1f(channelName));

    // Generate the texture data for each tile by sampling the field
    // channel. This can be done in parallel since the data is stored
    // per tile and there are no OpenGL calls.

    const int texDim(static_cast<int>(_texSampleRate*_tileSize));
#ifdef OMP_ENABLED
    #pragma omp parallel
#endif
    {
        int t;
#ifdef OMP_ENABLED
    #pragma omp for
#endif
        for (t = 0; t < allTileCount; ++t) {
            _tileVec[t].sampleTexData(layout,
                                      fch,
                                      texDim,
                                      texDim,
                                      texDim);
        }
    }

#ifndef WIN32
    gettimeofday(&s1, 0);
    std::cerr
    << ss.str() << "Tex Dim: " << texDim << "\n"
    << ss.str() << "Tex Sampling: "
    << (s1.tv_sec - s0.tv_sec) + (s1.tv_usec - s0.tv_usec)/1000000.0
    << " [s]\n";
#endif

    // Upload tile texture data to the GPU, needs to be done
    // sequentially because of OpenGL.

    _minPhi =  std::numeric_limits<GLfloat>::max();
    _maxPhi = -std::numeric_limits<GLfloat>::max();

    for (int t = 0; t < allTileCount; ++t) {
        // NB: All tiles uploaded to GPU, right now this is done only once.
        // For large data sets it would be better to have a caching mechanism
        // that handled the transfer of texture data to the GPU dynamically.

        uploadTex(_tileVec[t]);

        if(t<fineTileCount) {
            if(_tileVec[t].minPhi() > -EM_BACKGROUND_PHI)
                _minPhi = std::min(_minPhi, _tileVec[t].minPhi());
            if(_tileVec[t].maxPhi() < EM_BACKGROUND_PHI)
                _maxPhi = std::max(_maxPhi, _tileVec[t].maxPhi());
        }
    }

#ifndef WIN32
    gettimeofday(&tv1, 0);
    std::cerr
    << ss.str() << "Phi Min: " << _minPhi << " | Max:" << _maxPhi << "\n"
    << ss.str()
        << (tv1.tv_sec - tv0.tv_sec) + (tv1.tv_usec - tv0.tv_usec)/1000000.0
        << " [s]\n\n";
#endif

    setIso(_isoValue, _isoUpperBand, _isoLowerBand);
}


// setBBox
// -------
//! Document!

void
TileManager::setBBox(const vec3f &t,
                     const vec3f &r,
                     const vec3f &s)
{
    em::glmat44f M(em::make_transform(t, r, s));
    em::invert(M, _invBBoxXForm);

    _bBoxTileVec.clear();

    for (std::vector<Tile>::size_type t = 0;
         t < _tileVec.size();
         ++t) {
        bool inside = false;
        int i = 0;
        vec3f c;

        while (!inside && i < 8) {
            // Transform tile corner into BBox space and check with
            // (hard-coded) BBox dimensions.


            const vec3f c(_invBBoxXForm*_tileVec[t][i].obj);
            //multMat44fVec3f(_invBBoxXForm[0], _tileVec[t][i].obj, &c);

            if (-1.0 <= c[0] && c[0] <= 1.0 &&
                -1.0 <= c[1] && c[1] <= 1.0 &&
                -1.0 <= c[2] && c[2] <= 1.0) {
                _bBoxTileVec.push_back(&_tileVec[t]);
                inside = true;
            }
            ++i;
        }
    }

#if 0
    std::cerr << "BBox Tiles: " << _bBoxTileVec.size() << "\n";
#endif
}

// setIsoValue
// -----------
//! Generate a list of tiles that are of interest given an iso-value.

void
TileManager::setIso(const GLfloat isoValue,
                    const GLfloat isoUpperBand,
                    const GLfloat isoLowerBand)
{
    if(_ghost) {
        _isoTileVec=_bBoxTileVec;
        return;
    }

#ifndef WIN32
    std::stringstream ss;
    ss << "TileManager::setIso: '" << this << "': ";

    timeval tv0;
    timeval tv1;

    gettimeofday(&tv0, 0);
#endif

    _isoValue = isoValue;
    _isoUpperBand = fabs(isoUpperBand);   // Cannot be negative
    _isoLowerBand = fabs(isoLowerBand);   // Cannot be negative
    _isoTileVec.clear();

//    for (std::vector<Tile>::size_type t = 0;
//         t < _tileVec.size();
//         ++t) {
    for (std::vector<Tile*>::size_type t = 0;
         t < _bBoxTileVec.size();
         ++t) {
        // Check if interface passes through tile, or part of tile is
        // close to the interface

        const GLfloat minPhiIso(_bBoxTileVec[t]->minPhi() - _isoValue);
        const GLfloat maxPhiIso(_bBoxTileVec[t]->maxPhi() - _isoValue);
        if (minPhiIso*maxPhiIso < 0.0f ||
            std::min(
                fabs(minPhiIso),
                fabs(maxPhiIso)) < (_isoUpperBand + _isoLowerBand)*_cellSize) {
            _isoTileVec.push_back(_bBoxTileVec[t]);
        }
    }

#if 0
    gettimeofday(&tv1, 0);
    std::cerr
    << "\n"
    << ss.str() << "Iso Value: " << _isoValue << "\n"
    << ss.str() << "Iso Upper Band: " << _isoUpperBand << "\n"
    << ss.str() << "Iso Lower Band: " << _isoLowerBand << "\n"
    << ss.str() << "Tile Count: " << _tileVec.size() << " | Iso: "
        << _isoTileVec.size() << " | Vis: " << _visTileVec.size() << "\n"
    << ss.str()
        << (tv1.tv_sec - tv0.tv_sec) + (tv1.tv_usec - tv0.tv_usec)/1000000.0
        << " [s]\n\n";
#endif
}


// project
// -------
//! Given camera information, project iso-active tiles into eye- and
//! screenspace. Generates a list of visible tiles and stores tile ranges
//! in eyespace.

void
TileManager::project(const mat44f &mv,
                     const mat44f &p,
                     const Viewport &vp)
{
#ifndef WIN32
    std::stringstream ss;
    ss << "TileManager::project: '" << this << "': ";

    timeval tv0;
    timeval tv1;

    gettimeofday(&tv0, 0);
#endif

    const std::vector<Tile*>::size_type size(_isoTileVec.size());
#ifdef OMP_ENABLED
#pragma omp parallel
#endif
    {
        std::vector<Tile*>::size_type i;
#ifdef OMP_ENABLED
#pragma omp for
#endif
        for (i = 0; i < size; ++i) {
            _isoTileVec[i]->project(mv, p, vp);
        }
    }

    _visTileVec.clear();
    _minEyeZ = std::numeric_limits<GLfloat>::max();
    _maxEyeZ = -std::numeric_limits<GLfloat>::max();

    for (std::vector<Tile*>::size_type i = 0; i < size; ++i) {
        _minEyeZ = std::min(_minEyeZ, _isoTileVec[i]->minEyeZ());
        _maxEyeZ = std::max(_maxEyeZ, _isoTileVec[i]->maxEyeZ());
        if (_isoTileVec[i]->isVisible()) {
            _visTileVec.push_back(_isoTileVec[i]);
        }
    }

#if 0
    gettimeofday(&tv1, 0);

    std::cerr
    << "\n"
    << ss.str() << "EyeZ Min: " << _minEyeZ << " | Max:" << _maxEyeZ << "\n"
    << ss.str() << "Tile Count: " << _tileVec.size() << " | Iso: "
        << _isoTileVec.size() << " | Vis: " << _visTileVec.size() << "\n"
    << ss.str()
        << (tv1.tv_sec - tv0.tv_sec) + (tv1.tv_usec - tv0.tv_usec)/1000000.0
        << " [s]\n\n";
#endif
}


// slice
// -----
//! Generate planes in eyespace and intersect these with visible tiles.

void
TileManager::slice()
{
#ifndef WIN32
    std::stringstream ss;
    ss << "TileManager::slice: '" << this << "': ";

    timeval tv0;
    timeval tv1;

    gettimeofday(&tv0, 0);
#endif

    // Create proxy planes

    const GLfloat sliceMinZ(_minEyeZ + 0.5f*_cellSize);
    //const GLfloat sliceMaxZ(_maxEyeZ - 0.5f*_cellSize);
    const GLfloat sliceRangeZ(_maxEyeZ - _minEyeZ);
    const int sliceCount(
            static_cast<int>(sliceRangeZ/fabs(_sliceSpacingZ)) + 1);

    _ppVec.clear();

    for (int s = 0; s < sliceCount; ++s) {
        _ppVec.push_back(ProxyPlane(
                vec3f(0.0f,
                      0.0f,
                      sliceMinZ + s*fabs(_sliceSpacingZ))));
    }

    // Test intersections of planes with visible tiles

    const std::vector<Tile*>::size_type size(_visTileVec.size());
#ifdef OMP_ENABLED
#pragma omp parallel
#endif
    {
        std::vector<Tile*>::size_type i;
#ifdef OMP_ENABLED
#pragma omp for
#endif
        for (i = 0; i < size; ++i) {
            _visTileVec[i]->slice(_ppVec);
        }
    }

#if 0
    gettimeofday(&tv1, 0);

    std::cerr
    << "\n"
    << ss.str() << "Slice Count: " << sliceCount << "\n"
    << ss.str()
        << (tv1.tv_sec - tv0.tv_sec) + (tv1.tv_usec - tv0.tv_usec)/1000000.0
        << " [s]\n\n";
#endif
}


// draw
// ----
//! Draw the slice geometry of visible tiles using the VBO maintained by
//! the manager.

#define BUFFER_OFFSET(i) ((char *)NULL + (i))

void
TileManager::draw()
{
#ifndef WIN32
    std::stringstream ss;
    ss << "TileManager::draw: '" << this << "': ";

    timeval tv0;
    timeval tv1;

    gettimeofday(&tv0, 0);
#endif

    // Transfer data to GPU

    uploadSliceVbo();

    // Render

    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    FlipState<GL_BLEND>      blendState;
    FlipState<GL_DEPTH_TEST> depthTest;
    if(_ghost) {
        FlipState<GL_BLEND>::enable();
        FlipState<GL_DEPTH_TEST>::disable();
        glBlendFunc(GL_SRC_ALPHA,GL_ONE);
    }

    // We call gl___Pointer only once per draw, since a lot of work
    // is done behind the scenes for these functions

    glBindBuffer(GL_ARRAY_BUFFER, _sliceVboDataId);
    glVertexPointer(3,
                    GL_FLOAT,
                    sizeof(Tile::VboVtx),
                    BUFFER_OFFSET(0));
    glClientActiveTexture(GL_TEXTURE0);
    glTexCoordPointer(3,
                      GL_FLOAT,
                      sizeof(Tile::VboVtx),
                      BUFFER_OFFSET(sizeof(vec3f)));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _sliceVboIdxId);

    if (hasShader()) {
        _shader->bind();

        if ("iso" == _shaderStr) {
            _shader->setUniformValue(_phiTexLoc, 0);    // Texture unit 0
            _shader->setUniformValue(_cellSizeLoc, _cellSize);
            _shader->setUniformValue(_isoValueLoc, _isoValue);
            _shader->setUniformValue(_isoUpperBandLoc, _isoUpperBand);
            _shader->setUniformValue(_isoLowerBandLoc, _isoLowerBand);
            //_shader->setUniformValue(_maxDistLoc, _maxDist);
            _shader->setUniformValue(_frontDiffColorLoc,
                                     _frontDiffColor[0],
                                     _frontDiffColor[1],
                                     _frontDiffColor[2]);
            _shader->setUniformValue(_backDiffColorLoc,
                                     _backDiffColor[0],
                                     _backDiffColor[1],
                                     _backDiffColor[2]);
        }
        else if ("iso-distance" == _shaderStr) {
            _shader->setUniformValue(_phiTexLoc, 0);    // Texture unit 0
            _shader->setUniformValue(_maxPhiLoc, _maxPhi);
            _shader->setUniformValue(_minPhiLoc, _minPhi);
        } else if(_ghost) {
            _shader->setUniformValue(_phiTexLoc, 0);    // Texture unit 0
        }

        // we'll always need a bounding box...
        _shader->setUniformValue(
            _invBBoxXFormLoc,
            QMatrix4x4(
                _invBBoxXForm[0][0], _invBBoxXForm[1][0], _invBBoxXForm[2][0], _invBBoxXForm[3][0],
                _invBBoxXForm[0][1], _invBBoxXForm[1][1], _invBBoxXForm[2][1], _invBBoxXForm[3][1],
                _invBBoxXForm[0][2], _invBBoxXForm[1][2], _invBBoxXForm[2][2], _invBBoxXForm[3][2],
                _invBBoxXForm[0][3], _invBBoxXForm[1][3], _invBBoxXForm[2][3], _invBBoxXForm[3][3]
                )
            );
    }
    else {
        glColor3fv(&_frontDiffColor[0]);
    }

    // For each tile, bind its texture and use the corresponding part of the
    // index buffer to render the triangles within the tile

    const std::vector<Tile*>::iterator iend(_visTileVec.end());
    for (std::vector<Tile*>::iterator iter = _visTileVec.begin();
         iter != iend;
         ++iter)
    {
        // Bind the tile texture

        glBindTexture(GL_TEXTURE_3D, (*iter)->texId());

        // glDrawRangeElements is reported to be faster than
        // glDrawElements in some cases

        glDrawRangeElements(GL_TRIANGLES,
                            (*iter)->vboIdxStart(), // Smallest index
                            (*iter)->vboIdxEnd(),   // Largest index
                            (*iter)->vboIdxCount(), // Index count
                            GL_UNSIGNED_INT,
                            BUFFER_OFFSET((*iter)->vboIdxOffsetSize()));
    }

    if (hasShader()) {
        _shader->release();
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);

#if 0
    gettimeofday(&tv1, 0);

    std::cerr
    << "\n"
    << ss.str()
        << (tv1.tv_sec - tv0.tv_sec) + (tv1.tv_usec - tv0.tv_usec)/1000000.0
        << " [s]\n\n";
#endif
}

#undef BUFFER_OFFSET


// uploadSliceVbo
// --------------
//! Create a "super VBO" that stores all geometric tile data, i.e. the
//! triangulated slices.
//!
//! The postfix "Size" is used for variables that represent a quantity of bytes.

void
TileManager::uploadSliceVbo()
{
#if 0
    std::stringstream ss;
    ss << "TileManager::uploadSliceVBO: '" << this << "': ";

    unsigned int numTri = 0;

    timeval tv0;
    timeval tv1;

    gettimeofday(&tv0, 0);
#endif

    const std::vector<Tile*>::size_type visCount(_visTileVec.size());

    GLsizei totalVboDataSize = 0;   // [bytes]
    GLsizei totalVboIdxSize = 0;    // [bytes]

    GLuint idxShift = 0;
    GLsizei idxOffsetSize = 0;      // [bytes]
    for (std::vector<Tile*>::size_type t = 0; t < visCount; ++t) {
        // Tile vertex indices are local, so they must be shifted to
        // account for other tiles. We do this by simply add the sum of
        // vertices in previously processed tiles to the indices. We must also
        // keep track of the number of indices preceeding those in the current
        // tile. Finally, we collect information about the total size of the
        // VBO.

        _visTileVec[t]->shiftVboIdx(idxShift);
        _visTileVec[t]->setVboIdxOffsetSize(idxOffsetSize);

        idxShift += _visTileVec[t]->vboDataCount();
        idxOffsetSize += _visTileVec[t]->vboIdxSize();

        totalVboDataSize += _visTileVec[t]->vboDataSize();
        totalVboIdxSize += _visTileVec[t]->vboIdxSize();

#if 0
        numTri += _visTileVec[t]->vboIdxCount()/3;
#endif
    }

    // Data buffer

    if (0 == _sliceVboDataId) {
        glGenBuffers(1, &_sliceVboDataId);

        if (0 == _sliceVboDataId) {
            NB_THROW("Invalid VBO[Data] Id!")
        }
    }

    // Create a buffer large enough to hold all vertex data. The memory is not
    // initialized immediately. This has been reported to be faster, because
    // the driver knows that the current memory can be discarded.

    glBindBuffer(GL_ARRAY_BUFFER, _sliceVboDataId);
    glBufferData(GL_ARRAY_BUFFER,                   // Target
                 totalVboDataSize,                  // Size [bytes]
                 0,                                 // Data
                 _sliceVboUsage);                   // Usage

    // Upload vertex data for each tile, using previously computed offsets

    for (std::vector<Tile*>::size_type t = 0; t < visCount; ++t) {
        glBufferSubData(
            GL_ARRAY_BUFFER,                         // Target
            _visTileVec[t]->vboDataOffsetSize(),     // Offset [bytes]
            _visTileVec[t]->vboDataSize(),           // Size [bytes]
            _visTileVec[t]->vboData());              // Data
    }

    // Index buffer, see previous discussion on vertex data.

    if (0 == _sliceVboIdxId) {
        glGenBuffers(1, &_sliceVboIdxId);

        if (0 == _sliceVboIdxId) {
            NB_THROW("Invalid VBO[Idx] Id!")
        }
    }

    // Resize the VBO, see discussion above.

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _sliceVboIdxId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // Target
                 totalVboIdxSize,                   // Size [bytes]
                 0,                                 // Data
                 _sliceVboUsage);                   // Usage

    for (std::vector<Tile*>::size_type t = 0; t < visCount; ++t) {
        glBufferSubData(
            GL_ELEMENT_ARRAY_BUFFER,                // Target
            _visTileVec[t]->vboIdxOffsetSize(),     // Offset [bytes]
            _visTileVec[t]->vboIdxSize(),           // Size [bytes]
            _visTileVec[t]->vboIdx());              // Data
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

#if 0
    gettimeofday(&tv1, 0);

    std::cerr
    << "\n"
    << ss.str() << "VBO[Data] Id: " << _sliceVboDataId << "\n"
    << ss.str() << "VBO[Idx] Id: " << _sliceVboIdxId << "\n"
    << ss.str() << "VBO[Data] Size: " << totalVboDataSize << " [bytes]\n"
    << ss.str() << "VBO[Idx] Size: " << totalVboIdxSize << " [bytes]\n"
    << ss.str() << "Triangle Count: " << numTri << "\n"
    << ss.str()
        << (tv1.tv_sec - tv0.tv_sec) + (tv1.tv_usec - tv0.tv_usec)/1000000.0
        << " [s]\n\n";
#endif
}


// uploadTex
// ---------
//! Transfer texture data of a tile to GPU.

void
TileManager::uploadTex(const Tile &tile)
{
    // Pretty standard texture upload. Set filtering to always be
    // linear interpolation. Texture coordinate are clamped to "edge" to
    // ensure that values are always [0,1].

    glBindTexture(GL_TEXTURE_3D, tile.texId());
    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexImage3D(GL_TEXTURE_3D,         // Target
                 0,                     // Level
#ifdef GL_RGBA32F
                 GL_RGBA32F,            // Internal format
#else
                 GL_RGBA32F_ARB,        // Internal format
#endif
                 static_cast<GLsizei>(tile.texData().ni),  // Width
                 static_cast<GLsizei>(tile.texData().nj),  // Height
                 static_cast<GLsizei>(tile.texData().nk),  // Depth
                 0,                     // Border
                 GL_RGBA,               // Format
                 GL_FLOAT,              // Type
                 &tile.texData()[0]);   // "Pixels" (Voxels)
}

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif