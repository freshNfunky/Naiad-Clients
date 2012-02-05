// -----------------------------------------------------------------------------
//
// NglBBox.h
//
// Bounding box.
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

#ifndef NGL_BBOX_H
#define NGL_BBOX_H

#include "NglExtensions.h"

#include <em_vec.h>

namespace Ngl
{
// -----------------------------------------------------------------------------

template<typename T>
class BBox
{
public:

    typedef T ValueType;


    static
    BBox<T>
    combine(const BBox<T>& lhs, const BBox<T>& rhs)
    {
        return BBox<T>(std::min(_min[0], rhs._min[0]),
                       std::max(_maxX, rhs._maxX),
                       std::min(_minY, rhs._minY),
                       std::max(_maxY, rhs._maxY),
                       std::min(_minZ, rhs._minZ),
                        std::max(_maxZ, rhs._maxZ));
    }


    explicit
    BBox(const em::vec<3,T>& min,
         const em::vec<3,T>& max)
         : _min(std::min<T>(min[0], max[0]),
                std::min<T>(min[1], max[1]),
                std::min<T>(min[2], max[2])),
           _max(std::max<T>(min[0], max[0]),
                std::max<T>(min[1], max[1]),
                std::max<T>(min[2], max[2]))
    {}

    em::vec<3,GLfloat>
    mid() const
    {
        return em::vec<3,GLfloat>(0.5f*(_min[0] + _max[0]),
                                  0.5f*(_min[1] + _max[1]),
                                  0.5f*(_min[2] + _max[2]));
    }

    GLfloat midX() const { return 0.5f*(_min[0] + _max[0]); }
    GLfloat midY() const { return 0.5f*(_min[1] + _max[1]); }
    GLfloat midZ() const { return 0.5f*(_min[2] + _max[2]); }

    const em::vec<3,T>& min()  const { return _min;    }
    T                   minX() const { return _min[0]; }
    T                   minY() const { return _min[1]; }
    T                   minZ() const { return _min[2]; }

    const em::vec<3,T>& max()  const { return _max;    }
    T                   maxX() const { return _max[0]; }
    T                   maxY() const { return _max[1]; }
    T                   maxZ() const { return _max[2]; }


    void
    setMinX(const T minX)
    {
        _min[0] = std::min<T>(minX, _max[0]);
        _max[0] = std::max<T>(minX, _max[0]);
    }

    void
    setMaxX(const T maxX)
    {
        _min[0] = std::min<T>(_min[0], maxX);
        _max[0] = std::max<T>(_min[0], maxX);
    }

    void
    setMinY(const T minY)
    {
        _min[1] = std::min<T>(minY, _max[1]);
        _max[1] = std::max<T>(minY, _max[1]);
    }

    void
    setMaxY(const T maxY)
    {
        _min[1] = std::min<T>(_min[1], maxY);
        _max[1] = std::max<T>(_min[1], maxY);
    }

    void
    setMinZ(const T minZ)
    {
        _min[2] = std::min<T>(minZ, _max[2]);
        _max[2] = std::max<T>(minZ, _max[2]);
    }

    void
    setMaxZ(const T maxZ)
    {
        _min[2] = std::min<T>(_min[2], maxZ);
        _max[2] = std::max<T>(_min[2], maxZ);
    }


    BBox<T>
    combined(const BBox<T>& rhs)
    {
        return
            BBox<T>(
                std::min<T>(_min[0], rhs._min[0]),
                std::min<T>(_min[1], rhs._min[1]),
                std::min<T>(_min[2], rhs._min[2]),
                std::max<T>(_max[0], rhs._max[0]),
                std::max<T>(_max[1], rhs._max[1]),
                std::max<T>(_max[2], rhs._max[2]));
    }

    bool
    intersect(const BBox<T>& rhs)
    {

    }

private:    // Member variables.

    em::vec<3,GLfloat> _min;
    em::vec<3,GLfloat> _max;

private:    // Utility functions.

    // NB: No checking.

    explicit
    BBox(const T minX, const T minY, const T minZ,
         const T maxX, const T maxY, const T maxZ)
        : _min(minX, minY, minZ),
          _max(maxX, maxY, maxZ)
    {}

private:    // Disabled.

    BBox();
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_BBOX_H
