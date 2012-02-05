// -----------------------------------------------------------------------------
//
// Ns3DBBox.h
//
// Axis-aligned bounding box.
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
//
// -----------------------------------------------------------------------------

#ifndef NS3D_BBOX_H
#define NS3D_BBOX_H

#include <em_vec.h>
#include <QtOpenGL>     // OGL types
#include <cmath>

// -----------------------------------------------------------------------------

class Ns3DBBox
{
public:

    //! CTOR.
    explicit
    Ns3DBBox(const GLfloat minX = 0.f, const GLfloat maxX = 0.f,
             const GLfloat minY = 0.f, const GLfloat maxY = 0.f,
             const GLfloat minZ = 0.f, const GLfloat maxZ = 0.f)
         : _minX(std::min<GLfloat>(minX, maxX)) 
         , _maxX(std::max<GLfloat>(minX, maxX))
         , _minY(std::min<GLfloat>(minY, maxY)) 
         , _maxY(std::max<GLfloat>(minY, maxY))
         , _minZ(std::min<GLfloat>(minZ, maxZ)) 
         , _maxZ(std::max<GLfloat>(minZ, maxZ))
    {}

    GLfloat 
    midX() const 
    { return 0.5f*(_minX + _maxX); }

    GLfloat 
    midY() const 
    { return 0.5f*(_minY + _maxY); }

    GLfloat 
    midZ() const 
    { return 0.5f*(_minZ + _maxZ); }

    GLfloat 
    dimX() const 
    { return (_maxX - _minX); }

    GLfloat 
    dimY() const 
    { return (_maxY - _minY); }

    GLfloat 
    dimZ() const 
    { return (_maxZ - _minZ); }

    GLfloat 
    minX() const 
    { return _minX; }

    GLfloat 
    maxX() const 
    { return _maxX; }

    GLfloat 
    minY() const 
    { return _minY; }

    GLfloat 
    maxY() const 
    { return _maxY; }

    GLfloat 
    minZ() const 
    { return _minZ; }

    GLfloat 
    maxZ() const 
    { return _maxZ; }

    //! No zero dim checking!
    GLfloat
    diagonal() const
    { return em::mag(em::vec<3,GLfloat>(dimX(), dimY(), dimZ())); }

    //! No zero dim checking!
    GLfloat
    diagonalXY() const
    { return em::mag(em::vec<2,GLfloat>(dimX(), dimY())); }

    //! No zero dim checking!
    GLfloat
    diagonalXZ() const
    { return em::mag(em::vec<2,GLfloat>(dimX(), dimZ())); }

    //! No zero dim checking!
    GLfloat
    diagonalYZ() const
    { return em::mag(em::vec<2,GLfloat>(dimY(), dimZ())); }


    void
    setMinX(const GLfloat minX)
    {
        _minX = std::min<GLfloat>(minX, _maxX);
        _maxX = std::max<GLfloat>(minX, _maxX);
    }

    void
    setMaxX(const GLfloat maxX)
    {
        _minX = std::min<GLfloat>(_minX, maxX);
        _maxX = std::max<GLfloat>(_minX, maxX);
    }

    void
    setMinY(const GLfloat minY)
    {
        _minY = std::min<GLfloat>(minY, _maxY);
        _maxY = std::max<GLfloat>(minY, _maxY);
    }

    void
    setMaxY(const GLfloat maxY)
    {
        _minY = std::min<GLfloat>(_minY, maxY);
        _maxY = std::max<GLfloat>(_minY, maxY);
    }

    void
    setMinZ(const GLfloat minZ)
    {
        _minZ = std::min<GLfloat>(minZ, _maxZ);
        _maxZ = std::max<GLfloat>(minZ, _maxZ);
    }

    void
    setMaxZ(const GLfloat maxZ)
    {
        _minZ = std::min<GLfloat>(_minZ, maxZ);
        _maxZ = std::max<GLfloat>(_minZ, maxZ);
    }

    Ns3DBBox
    united(const Ns3DBBox &other) const
    {
        return Ns3DBBox(
            std::min<GLfloat>(_minX, other._minX),
            std::max<GLfloat>(_maxX, other._maxX),
            std::min<GLfloat>(_minY, other._minY),
            std::max<GLfloat>(_maxY, other._maxY),
            std::min<GLfloat>(_minZ, other._minZ),
            std::max<GLfloat>(_maxZ, other._maxZ));
    }

    void
    unite(const Ns3DBBox &other)
    {
        _minX = std::min<GLfloat>(_minX, other._minX);
        _maxX = std::max<GLfloat>(_maxX, other._maxX);
        _minY = std::min<GLfloat>(_minY, other._minY);
        _maxY = std::max<GLfloat>(_maxY, other._maxY);
        _minZ = std::min<GLfloat>(_minZ, other._minZ);
        _maxZ = std::max<GLfloat>(_maxZ, other._maxZ);
    }

    bool 
    intersects(const Ns3DBBox &other) const
    {
        return
            (_minX < other._maxX) && (_maxX > other._minX) &&
            (_minY < other._maxY) && (_maxY > other._minY) &&
            (_minZ < other._maxZ) && (_maxZ > other._minZ);
    }

private:    // Member variables.

    GLfloat _minX;
    GLfloat _maxX;
    GLfloat _minY;
    GLfloat _maxY;
    GLfloat _minZ;
    GLfloat _maxZ;
};

#endif // NS3D_BBOX_H
