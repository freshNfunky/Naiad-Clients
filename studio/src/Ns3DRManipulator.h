// -----------------------------------------------------------------------------
//
// Ns3DRManipulator.h
//
// Widget for manipulating the rotation angles of selected object(s).
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

#ifndef NS3D_RMANIPULATOR_H
#define NS3D_RMANIPULATOR_H

#include "Ns3DManipulator.h"
#include "Ns3DManipulatorComponent.h"
#include "Ns3DManipulatorPivot.h"

#include <NglTypes.h>
#include <NglUtils.h>
#include <NglMaterial.h>

namespace Ngl { class Viewport; }   // Fwd.

// -----------------------------------------------------------------------------

class Ns3DRManipulatorPivot : public Ns3DManipulatorPivot
{
public:

    //! CTOR.
    explicit
    Ns3DRManipulatorPivot(Ns3DManipulator  *parent,
                          const int32_t     selId,
                          const Ngl::vec3f &color,
                          const Ngl::vec3f &pressColor,
                          const GLfloat     lineWidth,
                          const GLfloat     radius,
                          const GLint       subd)
        : Ns3DManipulatorPivot(parent, selId),
          _color(color),
          _pressColor(pressColor),
          _lineWidth(lineWidth),
          _radius(radius),
          _ring(Ns3DManipulator::xAxis,
                Ns3DManipulator::yAxis,
                radius,
                subd,
                Ngl::vec3f(0.f)),
          _disk(Ns3DManipulator::xAxis,
                Ns3DManipulator::yAxis,
                radius,
                subd,
                Ngl::vec3f(0.f)),
          _p0(0.f)
    {}

    //! DTOR.
    virtual 
    ~Ns3DRManipulatorPivot() 
    {}

    virtual void 
    draw(const Ngl::Viewport &vp,
         const em::glmat44f  &mv,
         const em::glmat44f  &proj) const;

    virtual void 
    selectionDraw(const Ngl::Viewport &vp,
                  const em::glmat44f  &mv,
                  const em::glmat44f  &proj) const;

    virtual void 
    mouseDrag(const Ngl::Viewport &vp,
              const em::glmat44f  &mv,
              const em::glmat44f  &proj,
              int                  mdx,
              int                  mdy,
              int                  mx,
              int                  my);

    virtual void 
    mousePress(const Ngl::Viewport &vp,
               const em::glmat44f  &mv,
               const em::glmat44f  &proj,
               int                  mx,
               int                  my);

    virtual void 
    mouseRelease();

private:

    Ngl::vec3f _color;
    Ngl::vec3f _pressColor;
    GLfloat    _lineWidth;
    GLfloat    _radius;
    Ngl::Ring  _ring;
    Ngl::Disk  _disk;

    //Ngl::Torus    _torus;
    //Ngl::Sphere   _sphere;

    Ngl::vec3f _p0;
};

// -----------------------------------------------------------------------------

class Ns3DRManipulatorComponent : public Ns3DManipulatorComponent
{
public:

    //! CTOR.
    explicit
    Ns3DRManipulatorComponent(Ns3DManipulator     *parent,
                              const int32_t        selId,
                              const Ngl::vec3f    &axis,
                              const Ngl::vec3f    &nml0,
                              const Ngl::vec3f    &nml1,
                              const Ngl::Material &mat,
                              const Ngl::Material &pressMat,
                              const GLfloat        innerRadius,
                              const GLfloat        outerRadius,
                              const GLint          subdSide,
                              const GLint          subdRing)
        : Ns3DManipulatorComponent(parent, selId, axis, nml0, nml1),
          _material(mat),
          _pressMaterial(pressMat),
          _torus(innerRadius, outerRadius, subdSide, subdRing),
          _p0(0.f)
    {}

    //! DTOR.
    virtual 
    ~Ns3DRManipulatorComponent() 
    {}

    virtual void 
    draw(const Ngl::Viewport &vp,
         const em::glmat44f  &mv,
         const em::glmat44f  &proj) const;

    virtual void 
    selectionDraw(const Ngl::Viewport &vp,
                  const em::glmat44f  &mv,
                  const em::glmat44f  &proj) const;

    virtual void 
    mouseDrag(const Ngl::Viewport  &vp,
              const em::glmat44f   &mv,
              const em::glmat44f   &proj,
              int                   mdx,
              int                   mdy,
              int                   mx,
              int                   my,
              bool                  pivot = false);

    virtual void 
    mousePress(const Ngl::Viewport &vp,
               const em::glmat44f  &mv,
               const em::glmat44f  &proj,
               int                  mx,
               int                  my,
               bool                 pivot = false);

    virtual void 
    mouseRelease();

private:

    Ngl::Material   _material;
    Ngl::Material   _pressMaterial;
    Ngl::Torus      _torus;
    Ngl::vec3f      _p0;
};

// -----------------------------------------------------------------------------

class Ns3DRManipulator : public Ns3DManipulator
{
public:

    //! CTOR.
    explicit
    Ns3DRManipulator(NsUndoStack *undoStack,
                     GLfloat      innerRadius    = 0.025f,
                     GLfloat      outerRadius    = 1.f,
                     GLfloat      pivotRadius    = 250.f,
                     GLfloat      pivotLineWidth = 2.f,
                     GLint        subdSide       = 32,
                     GLint        subdRing       = 32);

    //! DTOR.
    virtual 
    ~Ns3DRManipulator() 
    {}

    // Ns3DManipulator interface

//    virtual void draw() const;
    virtual void
    selectionDraw(const Ngl::Viewport &vp,
                  const em::glmat44f  &mv,
                  const em::glmat44f  &proj) const;

    virtual void 
    makeXForm(const Ngl::Viewport &vp,
              const em::glmat44f  &mv,
              const em::glmat44f  &proj,
              GLfloat             *xForm) const;

public:

    GLfloat 
    innerRadius() const 
    { return _innerRadius; }

    GLfloat 
    outerRadius() const 
    { return _outerRadius; }

    GLint   
    subdSide() const 
    { return _subdSide; }

    GLint   
    subdRing() const 
    { return _subdRing;    }

private:

    GLfloat _innerRadius;
    GLfloat _outerRadius;
    GLint   _subdSide;
    GLint   _subdRing;
};

#endif // NS3D_RMANIPULATOR_H
