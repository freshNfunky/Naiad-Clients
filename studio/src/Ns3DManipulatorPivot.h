// -----------------------------------------------------------------------------
//
// Ns3DManipulatorPivot.h
//
// Interface for manipulator pivots.
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
//
// -----------------------------------------------------------------------------

#ifndef NS3D_MANIPULATOR_PIVOT_H
#define NS3D_MANIPULATOR_PIVOT_H

#include <NglTypes.h>
#include <vector>

namespace Ngl { class Viewport; }

class Ns3DManipulator;

// -----------------------------------------------------------------------------

class Ns3DManipulatorPivot
{
public:

    explicit
    Ns3DManipulatorPivot(Ns3DManipulator *parent,
                         const int32_t    selId)
        : _parent(parent),
          _selId(selId),
          _pressX(-1),
          _pressY(-1),
          _pressTranslate(0.f), // [0 0 0]
          _pressRotate(0.f),    // [0 0 0]
          _pressScale(1.f),     // [1 1 1]
          _firstDrag(true)
    {
    }

    virtual ~Ns3DManipulatorPivot() {}

    virtual void draw(const Ngl::Viewport &vp,
                      const em::glmat44f  &mv,
                      const em::glmat44f  &proj) const = 0;

    virtual void selectionDraw(const Ngl::Viewport &vp,
                               const em::glmat44f  &mv,
                               const em::glmat44f  &proj) const = 0;

    virtual void mouseDrag(const Ngl::Viewport &vp,
                           const em::glmat44f  &mv,
                           const em::glmat44f  &proj,
                           int                  mdx,
                           int                  mdy,
                           int                  mx,
                           int                  my) = 0;

    virtual void mousePress(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj,
                            int                  mx,
                            int                  my) = 0;

    virtual void
    mouseRelease()
    {
        _pressX         = -1;
        _pressY         = -1;
        _pressTranslate = Ngl::vec3f(0.f);
        _pressRotate    = Ngl::vec3f(0.f);
        _pressScale     = Ngl::vec3f(1.f);
        _pressValues.clear();
        _firstDrag      = true;
    }

    bool isPressed() const { return (-1 < _pressX && -1 < _pressY); }

protected:

    Ns3DManipulator       *parent()       { return _parent; }
    const Ns3DManipulator *parent() const { return _parent; }
    int32_t                selId()  const { return _selId;  }

    int               pressX() const         { return _pressX;         }
    int               pressY() const         { return _pressY;         }
    const Ngl::vec3f &pressTranslate() const { return _pressTranslate; }
    const Ngl::vec3f &pressRotate()    const { return _pressRotate;    }
    const Ngl::vec3f &pressScale()     const { return _pressScale;     }

    void setPressX(const int x)                  { _pressX = x;          }
    void setPressY(const int y)                  { _pressY = y;          }
    void setPressTranslate(const Ngl::vec3f &pt) { _pressTranslate = pt; }
    void setPressRotate(const Ngl::vec3f &pr)    { _pressRotate = pr;    }
    void setPressScale(const Ngl::vec3f &ps)     { _pressScale = ps;     }

    const std::vector<Ngl::vec3f> &pressValues() const
    { return _pressValues; }

    const Ngl::vec3f &pressValue(const int i) const
    { return _pressValues[i]; }

    bool firstDrag() const           { return _firstDrag; }
    void setFirstDrag(const bool fd) { _firstDrag = fd;   }


    void clearPressValues()                   { _pressValues.clear();        }
    void addPressValue(const Ngl::vec3f &val) { _pressValues.push_back(val); }

private:

    // Disabled

    Ns3DManipulatorPivot();
    Ns3DManipulatorPivot(const Ns3DManipulatorPivot&);
    Ns3DManipulatorPivot& operator=(const Ns3DManipulatorPivot&);

private:
    Ns3DManipulator        *_parent;
    int32_t                 _selId;
    int                     _pressX;
    int                     _pressY;
    Ngl::vec3f              _pressTranslate;
    Ngl::vec3f              _pressRotate;
    Ngl::vec3f              _pressScale;
    std::vector<Ngl::vec3f> _pressValues;
    bool                    _firstDrag;
};

#endif // NS3D_MANIPULATOR_PIVOT_H
