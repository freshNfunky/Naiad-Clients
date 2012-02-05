// -----------------------------------------------------------------------------
//
// Ns3DManipulatorComponent.h
//
// Interface for manipulator components.
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

#ifndef NS3D_MANIPULATOR_COMPONENT_H
#define NS3D_MANIPULATOR_COMPONENT_H

#include <NglTypes.h>
#include <cmath>
#include <vector>


namespace Ngl { class Viewport; }
class Ns3DManipulator;

// -----------------------------------------------------------------------------

class Ns3DManipulatorComponent
{
public:

    explicit
    Ns3DManipulatorComponent(Ns3DManipulator  *parent,
                             int32_t           selId,
                             const Ngl::vec3f &axis,
                             const Ngl::vec3f &nml0,
                             const Ngl::vec3f &nml1)
        : _parent(parent),
          _selId(selId),
          _pressX(-1),
          _pressY(-1),
          _pressDist(0.f),
          _pressTranslate(0.f),
          _pressRotate(0.f),
          _pressScale(1.f),
          _dragDist(0.f),
          _axis(axis),
          _nml0(nml0),
          _nml1(nml1),
          _dpNml(nml0),
          _firstDrag(true)
    {}

    virtual ~Ns3DManipulatorComponent() {}

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
                           int                  my,
                           bool                 pivot = false) = 0;

    virtual void mousePress(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj,
                            int                  mx,
                            int                  my,
                            bool                 pivot = false) = 0;

    virtual void
    mouseRelease()
    {
        _pressX         = -1;
        _pressY         = -1;
        _pressDist      = 0.f;
        _pressTranslate = Ngl::vec3f(0.f);
        _pressRotate    = Ngl::vec3f(0.f);
        _pressScale     = Ngl::vec3f(1.f);
        _dragDist       = 0.f;
        _firstDrag      = true;
    }

    bool isPressed() const { return (-1 < _pressX && -1 < _pressY); }

public:

    Ns3DManipulator       *parent()       { return _parent; }
    const Ns3DManipulator *parent() const { return _parent; }

    int32_t selId() const                 { return _selId; }

    int               pressX()         const { return _pressX;         }
    int               pressY()         const { return _pressY;         }
    GLfloat           pressDist()      const { return _pressDist;      }
    const Ngl::vec3f &pressTranslate() const { return _pressTranslate; }
    const Ngl::vec3f &pressRotate()    const { return _pressRotate;    }
    const Ngl::vec3f &pressScale()     const { return _pressScale;     }
    GLfloat           dragDist()       const { return _dragDist;       }

    void setPressX(const int x)                  { _pressX = x;          }
    void setPressY(const int y)                  { _pressY = y;          }
    void setPressDist(const GLfloat pd)          { _pressDist = pd;      }
    void setPressTranslate(const Ngl::vec3f &pt) { _pressTranslate = pt; }
    void setPressRotate(const Ngl::vec3f &pr)    { _pressRotate = pr;    }
    void setPressScale(const Ngl::vec3f &ps)     { _pressScale = ps;     }
    void setDragDist(const GLfloat dd)           { _dragDist = dd;       }

    const std::vector<Ngl::vec3f> &pressValues() const
    { return _pressValues; }

    const Ngl::vec3f &pressValue(const int i) const
    { return _pressValues[i]; }

    const Ngl::vec3f &axis()  const   { return _axis;    }
    GLfloat           axisX() const   { return _axis[0]; }
    GLfloat           axisY() const   { return _axis[1]; }
    GLfloat           axisZ() const   { return _axis[2]; }

    const Ngl::vec3f &nml0()  const   { return _nml0;    }
    GLfloat           nml0X() const   { return _nml0[0]; }
    GLfloat           nml0Y() const   { return _nml0[1]; }
    GLfloat           nml0Z() const   { return _nml0[2]; }

    const Ngl::vec3f &nml1()  const   { return _nml1;    }
    GLfloat           nml1X() const   { return _nml1[0]; }
    GLfloat           nml1Y() const   { return _nml1[1]; }
    GLfloat           nml1Z() const   { return _nml1[2]; }

    void setAxis(const Ngl::vec3f &ax) { _axis = ax; }
    void setNml0(const Ngl::vec3f &n0) { _nml0 = n0; }
    void setNml1(const Ngl::vec3f &n1) { _nml1 = n1; }

    const Ngl::vec3f &dragPlaneNormal() const      { return _dpNml; }
    void setDragPlaneNormal(const Ngl::vec3f &dpn) { _dpNml = dpn;  }

    bool firstDrag() const           { return _firstDrag; }
    void setFirstDrag(const bool fd) { _firstDrag = fd;   }


    void clearPressValues()                   { _pressValues.clear();        }
    void addPressValue(const Ngl::vec3f &val) { _pressValues.push_back(val); }

    const Ngl::vec3f absAxis() const { return Ngl::vec3f(fabs(_axis[0]),
                                                         fabs(_axis[1]),
                                                         fabs(_axis[2])); }

    const Ngl::vec3f negAxis() const { return Ngl::vec3f(-fabs(_axis[0]),
                                                         -fabs(_axis[1]),
                                                         -fabs(_axis[2])); }

private:    // Member variables

    Ns3DManipulator *_parent;
    int32_t          _selId;
    int              _pressX;
    int              _pressY;
    GLfloat          _pressDist;
    Ngl::vec3f       _pressTranslate;
    Ngl::vec3f       _pressRotate;
    Ngl::vec3f       _pressScale;
    GLfloat          _dragDist;

    std::vector<Ngl::vec3f> _pressValues;

    Ngl::vec3f _axis;    // NB: Assumed to be normalized
    Ngl::vec3f _nml0;    // Normal 0
    Ngl::vec3f _nml1;    // Normal 1
    Ngl::vec3f _dpNml;   // Drag plane normal

    bool _firstDrag;

private:    // Hidden

    // Disabled

    Ns3DManipulatorComponent(const Ns3DManipulatorComponent&);
    Ns3DManipulatorComponent &operator=(const Ns3DManipulatorComponent&);
};

#endif // NS3D_MANIPULATOR_COMPONENT_H
