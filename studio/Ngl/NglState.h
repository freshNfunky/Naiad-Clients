// -----------------------------------------------------------------------------
//
// NglState.h
//
// Manage default OpenGL render state through objects.
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

#ifndef NGL_STATE_H
#define NGL_STATE_H

#include "NglNonCopyable.h"
#include "NglExtensions.h"

#ifdef __APPLE__
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#ifdef WIN32
#undef near
#undef far
#endif

namespace Ngl
{
// -----------------------------------------------------------------------------
// Generic version, not implemented, gives linker error on attempted use.
// The idea is that a GLState object is created at function level scope
// in functions that alter that state. The destructor of the state
// specialization should always reset the state back to its default value.
// This means that the GL state is always in default configuration when the
// program leaves the function. For every state that the function will
// alter, a corresponding object should be created.
//
// Specializations              [default-state]:
// ---------------------------------------------
// * GL_DEPTH_TEST              [enabled]
// * GL_MULTISAMPLE             [enabled]
// * GL_ALPHA_TEST              [Disabled]
// * GL_AUTONORMAL              [Disabled]
// * GL_BLEND                   [Disabled]
// * GL_COLOR_MATERIAL          [Disabled]
// * GL_CULL_FACE               [Disabled]
// * GL_FOG                     [Disabled]
// * GL_LIGHTING                [Disabled]
// * GL_LINE_SMOOTH             [Disabled]
// * GL_LINE_STIPPLE            [Disabled]
// * GL_NORMALIZE               [Disabled]
// * GL_POINT_SMOOTH            [Disabled]
// * GL_POLYGON_OFFSET_FILL     [Disabled]
// * GL_POLYGON_OFFSET_LINE     [Disabled]
// * GL_POLYGON_STIPPLE         [Disabled]
// * GL_SCISSOR_TEST            [Disabled]
// * GL_STENCIL_TEST            [Disabled]
// * GL_TEXTURE_1D              [Disabled]
// * GL_TEXTURE_2D              [Disabled]
// * GL_TEXTURE_3D              [Disabled]
//
// * glActiveTexture            [GL_TEXTURE0]
// * glDepthRange               [(0, 1)]
// * glLightModel               [(GL_LIGHT_MODEL_TWO_SIDE, 0)]
// * glMatrixMode               [GL_MODELVIEW]
// * glPixelStore               [(GL_UNPACK_ALIGNMENT, 1),
//                               (GL_PACK_ALIGNMENT,   1)]
// * glPolygonMode              [(GL_FRONT_AND_BACK, GL_FILL)]
// * glShadeModel               [GL_SMOOTH]
//
// TODO: If classes provide (static) interfaces we can do some error checking
//       in debug mode, if desired.

template<GLenum E, bool D>
class FlipStateBase;

// Specialization for FlipState that enable on destruction.

template<GLenum E>
class FlipStateBase<E, true> : private NonCopyable
{
public:

    static const bool defaultState = true;
    static void enable() { glEnable(E); }
    static void disable() { glDisable(E); }

protected:

    FlipStateBase() {}
    ~FlipStateBase() { glEnable(E); }
};

// Specialization for FlipState that disable on destruction.

template<GLenum E>
class FlipStateBase<E, false> : private NonCopyable
{
public:

    static const bool defaultState = false;
    static void enable() { glEnable(E); }
    static void disable() { glDisable(E); }

    FlipStateBase() {}
    ~FlipStateBase() { glDisable(E); }
};

template<GLenum T>
class FlipState;

// -----------------------------------------------------------------------------

// Enabled by default.

template<> class FlipState<GL_DEPTH_TEST>
    : public FlipStateBase<GL_DEPTH_TEST, true> {};


template<> class FlipState<GL_MULTISAMPLE>
    : public FlipStateBase<GL_MULTISAMPLE, true> {};

// -----------------------------------------------------------------------------

// Disabled by default.

template<> class FlipState<GL_ALPHA_TEST>
    : public FlipStateBase<GL_ALPHA_TEST, false> {};

template<> class FlipState<GL_AUTO_NORMAL>
    : public FlipStateBase<GL_AUTO_NORMAL, false> {};

template<> class FlipState<GL_BLEND>
    : public FlipStateBase<GL_BLEND, false> {};

template<> class FlipState<GL_COLOR_MATERIAL>
    : public FlipStateBase<GL_COLOR_MATERIAL, false> {};

template<> class FlipState<GL_CULL_FACE>
    : public FlipStateBase<GL_CULL_FACE, false> {};

template<> class FlipState<GL_FOG>
    : public FlipStateBase<GL_FOG, false> {};

template<> class FlipState<GL_LIGHTING>
    : public FlipStateBase<GL_LIGHTING, false> {};

template<> class FlipState<GL_LINE_SMOOTH>
    : public FlipStateBase<GL_LINE_SMOOTH, false> {};

template<> class FlipState<GL_LINE_STIPPLE>
    : public FlipStateBase<GL_LINE_STIPPLE, false> {};

template<> class FlipState<GL_NORMALIZE>
    : public FlipStateBase<GL_NORMALIZE, false> {};

template<> class FlipState<GL_POINT_SMOOTH>
    : public FlipStateBase<GL_POINT_SMOOTH, false> {};

template<> class FlipState<GL_POLYGON_OFFSET_FILL>
    : public FlipStateBase<GL_POLYGON_OFFSET_FILL, false> {};

template<> class FlipState<GL_POLYGON_OFFSET_LINE>
    : public FlipStateBase<GL_POLYGON_OFFSET_LINE, false> {};

template<> class FlipState<GL_POLYGON_STIPPLE>
    : public FlipStateBase<GL_POLYGON_STIPPLE, false> {};

template<> class FlipState<GL_SCISSOR_TEST>
    : public FlipStateBase<GL_SCISSOR_TEST, false> {};

template<> class FlipState<GL_STENCIL_TEST>
    : public FlipStateBase<GL_STENCIL_TEST, false> {};

template<> class FlipState<GL_TEXTURE_1D>
    : public FlipStateBase<GL_TEXTURE_1D, false> {};

template<> class FlipState<GL_TEXTURE_2D>
    : public FlipStateBase<GL_TEXTURE_2D, false> {};

template<> class FlipState<GL_TEXTURE_3D>
    : public FlipStateBase<GL_TEXTURE_3D, false> {};

// -----------------------------------------------------------------------------

    class ActiveTextureState : private NonCopyable
{
public:

    //static const GLenum defaultTexture;

    ActiveTextureState() {}
    ~ActiveTextureState() { set(GL_TEXTURE0); }

    static void set(const GLenum texture) { glActiveTexture(texture); }
};

// -----------------------------------------------------------------------------

class DepthRangeState : private NonCopyable
{
public:

    //static const GLclampd defaultNear;
    //static const GLclampd defaultFar;

    DepthRangeState() {}
    ~DepthRangeState() { set(0., 1.); }

    static void set(const GLclampd near, const GLclampd far)
    { glDepthRange(near, far); }
};

// -----------------------------------------------------------------------------

class LightModelState : private NonCopyable
{
public:

    LightModelState() {}
    ~LightModelState() { set(GL_LIGHT_MODEL_TWO_SIDE, 0.f); }

    static void set(const GLenum pname, const GLfloat param)
    { glLightModelf(pname, param); }
};

// -----------------------------------------------------------------------------

class MatrixModeState : private NonCopyable
{
public:

    //static const GLenum defaultMode;

    MatrixModeState() {}
    ~MatrixModeState() { set(GL_MODELVIEW); }

    static void set(const GLenum mode) { glMatrixMode(mode); }
};

// -----------------------------------------------------------------------------

class PixelStoreState : private NonCopyable
{
public:

    PixelStoreState() {}

    ~PixelStoreState()
    {
        set(GL_UNPACK_ALIGNMENT,1);
        set(GL_PACK_ALIGNMENT,1);
    }

    static void set(const GLenum pname, const GLint param)
    { glPixelStorei(pname, param); }
};

// -----------------------------------------------------------------------------

class PolygonModeState : private NonCopyable
{
public:

    PolygonModeState() {}
    ~PolygonModeState() { set(GL_FRONT_AND_BACK, GL_FILL); }

    static void set(const GLenum face, const GLenum mode)
    { glPolygonMode(face, mode); }
};

// -----------------------------------------------------------------------------

class ShadeModelState : private NonCopyable
{
public:

    //static const GLenum defaultMode;

    ShadeModelState() {}
    ~ShadeModelState() { set(GL_SMOOTH); }

    static void set(const GLenum mode) { glShadeModel(mode); }
};

// -----------------------------------------------------------------------------
}   // Namespace: Ngl.

#endif  // NGL_STATE_H
