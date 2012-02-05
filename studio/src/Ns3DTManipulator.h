// -----------------------------------------------------------------------------
//
// Ns3DTManipulator.h
//
// Widget for manipulating the position(s) of selected object(s).
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

#ifndef NS3D_TMANIPULATOR_H
#define NS3D_TMANIPULATOR_H

#include "Ns3DManipulator.h"
#include "Ns3DManipulatorComponent.h"
#include "Ns3DManipulatorPivot.h"

#include <NglTypes.h>
#include <NglUtils.h>
#include <NglMaterial.h>

namespace Ngl { class Viewport; }


// -----------------------------------------------------------------------------

class Ns3DTManipulatorPivot : public Ns3DManipulatorPivot
{
public:

    explicit
    Ns3DTManipulatorPivot(Ns3DManipulator     *parent,
                          const int32_t        selId,
                          const Ngl::Material &mat,
                          const Ngl::Material &pressMat,
                          const GLfloat        pivotCubeDim)
        : Ns3DManipulatorPivot(parent, selId),
          _material(mat),
          _pressMaterial(pressMat),
          _cube(pivotCubeDim, Ngl::vec3f(0.f))
    {}

    virtual ~Ns3DTManipulatorPivot() {}

    virtual void draw(const Ngl::Viewport &vp,
                      const em::glmat44f  &mv,
                      const em::glmat44f  &proj) const;

    virtual void selectionDraw(const Ngl::Viewport &vp,
                               const em::glmat44f  &mv,
                               const em::glmat44f  &proj) const;

    virtual void mouseDrag(const Ngl::Viewport &vp,
                           const em::glmat44f  &mv,
                           const em::glmat44f  &proj,
                           int                  mdx,
                           int                  mdy,
                           int                  mx,
                           int                  my);

    virtual void mousePress(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj,
                            int                  mx,
                            int                  my);

private:    // Member variables

    Ngl::Material _material;
    Ngl::Material _pressMaterial;
    Ngl::Cuboid   _cube;
};

// -----------------------------------------------------------------------------

class Ns3DTManipulatorComponent : public Ns3DManipulatorComponent
{
public:

    explicit
    Ns3DTManipulatorComponent(
          Ns3DManipulator     *parent,
          const int32_t        selId,
          const Ngl::vec3f    &axis,
          const Ngl::vec3f    &nml0,
          const Ngl::vec3f    &nml1,
          const Ngl::Material &mat,
          const Ngl::Material &pressMat,
          const GLfloat        pivotCubeDim,
          const GLfloat        tubeThickness,
          const GLfloat        tubeLen,
          const GLfloat        coneHeight,
          const GLfloat        coneRadius,
          const GLint          coneSubd)
        : Ns3DManipulatorComponent(parent, selId, axis, nml0, nml1),
          _material(mat),
          _pressMaterial(pressMat),
          _tube(axis, tubeThickness, tubeLen, 0.5f*pivotCubeDim),
          _cone(axis, nml0, nml1,
                tubeLen, tubeLen + coneHeight,
                coneRadius, coneSubd)
    {}

    virtual ~Ns3DTManipulatorComponent() {}

    virtual void draw(const Ngl::Viewport &vp,
                      const em::glmat44f  &mv,
                      const em::glmat44f  &proj) const;

    virtual void selectionDraw(const Ngl::Viewport &vp,
                               const em::glmat44f  &mv,
                               const em::glmat44f  &proj) const;

    virtual void mouseDrag(const Ngl::Viewport &vp,
                           const em::glmat44f  &mv,
                           const em::glmat44f  &proj,
                           int                  mdx,
                           int                  mdy,
                           int                  mx,
                           int                  my,
                           bool                 pivot = false);

    virtual void mousePress(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj,
                            int                  mx,
                            int                  my,
                            bool                 pivot = false);

private:    // Member variables

    Ngl::Material _material;
    Ngl::Material _pressMaterial;
    Ngl::Cuboid   _tube;
    Ngl::Cone     _cone;
};

// -----------------------------------------------------------------------------

class Ns3DTManipulator : public Ns3DManipulator
{
public:

    explicit
    Ns3DTManipulator(NsUndoStack *undoStack,
                     GLfloat      pivotCubeDim   = 0.2f,
                     GLfloat      tubeLen        = 1.f,
                     GLfloat      tubeThickness  = 0.05f,
                     GLfloat      coneHeight     = 0.2f,
                     GLfloat      coneRadius     = 0.1f,
                     GLint        coneSubd       = 8);

    virtual ~Ns3DTManipulator() {}

    // Manipulator interface

    virtual void makeXForm(const Ngl::Viewport &vp,
                           const em::glmat44f  &mv,
                           const em::glmat44f  &proj,
                           GLfloat             *xForm) const;

public:

    GLfloat pivotCubeDim()  const { return _pivotCubeDim;  }
    GLfloat tubeLen()       const { return _tubeLen;       }
    GLfloat tubeThickness() const { return _tubeThickness; }
    GLfloat coneHeight()    const { return _coneHeight;    }
    GLfloat coneRadius()    const { return _coneRadius;    }
    GLint   coneSubd()      const { return _coneSubd;      }

private:    // Member variables

    GLfloat _pivotCubeDim;
    GLfloat _tubeLen;
    GLfloat _tubeThickness;
    GLfloat _coneHeight;
    GLfloat _coneRadius;
    GLint   _coneSubd;
};

#endif // NS3D_TMANIPULATOR_H
