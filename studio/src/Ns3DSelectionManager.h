// -----------------------------------------------------------------------------
//
// Ns3DSelectionManager.h
//
// Class for managing selection.
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

#ifndef NS3D_SELECTIONMANAGER_H
#define NS3D_SELECTIONMANAGER_H

#include <NglTypes.h>

#include <QImage>

#include <string>
#include <vector>
#include <cmath>

namespace Ngl { class Viewport; }
class QGLFramebufferObject;
class Ns3DGraphicsItem;
class Ns3DScene;

// -----------------------------------------------------------------------------

class Ns3DSelectionManager
{
public:

    static void
    idToRgba(const int32_t id, Ngl::vec4uc *rgba)
    {
//        (*rgba)[0] = (id & 0xFF000000) >> 24;
//        (*rgba)[1] = (id & 0x00FF0000) >> 16;
//        (*rgba)[2] = (id & 0x0000FF00) >> 8;
//        (*rgba)[3] = (id & 0x000000FF);
        (*rgba)[3] = (id & 0xFF000000) >> 24;
        (*rgba)[2] = (id & 0x00FF0000) >> 16;
        (*rgba)[1] = (id & 0x0000FF00) >> 8;
        (*rgba)[0] = (id & 0x000000FF);
    }


    static void
    rgbaToId(const Ngl::vec4uc &rgba, int32_t *id)
    {
        (*id) = 0;  // ??
        (*id) = (static_cast<int32_t>(rgba[3]) << 24) |
                (static_cast<int32_t>(rgba[2]) << 16) |
                (static_cast<int32_t>(rgba[1]) << 8)  |
                 static_cast<int32_t>(rgba[0]);
    }

public:

    explicit
    Ns3DSelectionManager(
        const Ngl::vec4f &areaQuadColor   = Ngl::vec4f(1.f, 1.f, 1.f, 0.5f),
        const Ngl::vec4f &areaBorderColor = Ngl::vec4f(0.f, 0.f, 0.f, 1.f),
        const GLfloat areaBorderWidth = 1.0f)
        : _idFbo(0)
        , _hasIdImage(false)
        , _areaQuadColor(areaQuadColor)
        , _areaBorderColor(areaBorderColor)
        , _areaBorderWidth(areaBorderWidth)
        , _areaStart(-1, -1)
        , _areaEnd(-1, -1)
    {}

    ~Ns3DSelectionManager();

    void
    setAreaStart(const GLint px, const GLint py)
    {
        _areaStart[0] = px;
        _areaStart[1] = py;
    }


    void
    setAreaEnd(const GLint px, const GLint py)
    {
        _areaEnd[0] = px;
        _areaEnd[1] = py;
    }


    GLint
    areaWidth() const
    { return abs(_areaEnd[0] - _areaStart[0]) + 1; }

    GLint
    areaHeight() const
    { return abs(_areaEnd[1] - _areaStart[1]) + 1; }

    GLint
    areaMinX() const
    { return std::min(_areaStart[0], _areaEnd[0]); }

    GLint
    areaMaxX() const
    { return std::max(_areaStart[0], _areaEnd[0]); }

    GLint
    areaMinY() const
    { return std::min(_areaStart[1], _areaEnd[1]); }

    GLint
    areaMaxY() const
    { return std::max(_areaStart[1], _areaEnd[1]); }

    void
    clearArea()
    {
        _areaStart[0] = -1;
        _areaStart[1] = -1;
        _areaEnd[0] = -1;
        _areaEnd[1] = -1;
    }


    bool
    hasArea() const
    {
        return (-1 < _areaStart[0] && -1 < _areaEnd[0] &&
                -1 < _areaStart[1] && -1 < _areaEnd[1]);
    }


    void resizeIdBuffer(const Ngl::Viewport &vp);

    void drawArea(const Ngl::Viewport &vp) const;

    void drawIdBuffer(const Ngl::Viewport &vp,
                      const em::glmat44f  &mv,
                      const em::glmat44f  &proj,
                      const Ns3DScene     &graph);

    std::vector<int32_t> queryIdImage(int x0, int y0,
                                      int x1, int y1);

    int32_t queryIdPixel(int x, int y);

private:    // Member variables.

    QGLFramebufferObject *_idFbo;   //!< May be null.
    QImage _idImage;
    bool _hasIdImage;

    Ngl::vec4f _areaQuadColor;
    Ngl::vec4f _areaBorderColor;
    GLfloat _areaBorderWidth;

    Ngl::vec2i _areaStart;
    Ngl::vec2i _areaEnd;

private:

    Ns3DSelectionManager(const Ns3DSelectionManager&);            //!< Disabled.
    Ns3DSelectionManager& operator=(const Ns3DSelectionManager&); //!< Disabled.
};

#endif // NS3D_SELECTIONMANAGER_H
