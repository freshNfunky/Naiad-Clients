// -----------------------------------------------------------------------------
//
// Ns3DManipulator.h
//
// Interface for manipulators.
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

#ifndef NS3D_MANIPULATOR_H
#define NS3D_MANIPULATOR_H

#include <NglTypes.h>
#include <NglMaterial.h>
#include <NglLight.h>
#include <set>


namespace Ngl { class Viewport; }
class Ns3DManipulatorComponent;
class Ns3DManipulatorPivot;
class Ns3DGraphicsItem;
class NsUndoStack;

// -----------------------------------------------------------------------------

class Ns3DManipulator
{
public:

    // Manipulator components have negative selection Id'so that they
    // are easy to separate from selectable objects in the Id-buffer

    static const int32_t pivotId = -2147483647-1; // avoid compiler complaints
    static const int32_t xCompId = -2147483647;
    static const int32_t yCompId = -2147483646;
    static const int32_t zCompId = -2147483645;

    static bool isManipId(const int32_t id)
    {return (pivotId == id || xCompId == id || yCompId == id || zCompId == id);}

    // Materials

    static const Ngl::Material pivotMaterial;
    static const Ngl::Material xCompMaterial;
    static const Ngl::Material yCompMaterial;
    static const Ngl::Material zCompMaterial;
    static const Ngl::Material pressMaterial;

    static const Ngl::vec3f pivotColor;
    static const Ngl::vec3f pressColor;

    static const Ngl::Light light;

    // Component axes

    static const Ngl::vec3f xAxis;
    static const Ngl::vec3f yAxis;
    static const Ngl::vec3f zAxis;

    // Value for determining if an axis is facing the viewer.

    static const GLfloat maxViewDot;

public:

    // Interface

    explicit
    Ns3DManipulator(Ns3DManipulatorPivot     *pivot,
                    Ns3DManipulatorComponent *xComp,
                    Ns3DManipulatorComponent *yComp,
                    Ns3DManipulatorComponent *zComp,
                    GLfloat                   perspScale = 0.3f,
                    NsUndoStack              *undoStack  = 0);

    virtual ~Ns3DManipulator();

    virtual void makeXForm(const Ngl::Viewport &vp,
                           const em::glmat44f  &mv,
                           const em::glmat44f  &proj,
                           GLfloat *xForm) const = 0;

    virtual void draw(const Ngl::Viewport &vp,
                      const em::glmat44f  &mv,
                      const em::glmat44f  &proj) const;

    virtual void selectionDraw(const Ngl::Viewport &vp,
                               const em::glmat44f  &mv,
                               const em::glmat44f  &proj) const;

    virtual void mousePress(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj,
                            int32_t              id,
                            int                  mx,
                            int                  my);

    virtual void mouseRelease();

    virtual void mouseDrag(const Ngl::Viewport &vp,
                           const em::glmat44f  &mv,
                           const em::glmat44f  &proj,
                           int                  mdx,
                           int                  mdy,
                           int                  mx,
                           int                  my);

    virtual bool isPressed() const;

    int itemCount() const { return static_cast<int>(_items.size()); }

    void attachItem(Ns3DGraphicsItem *item)          { _items.insert(item);    }
    void removeItem(Ns3DGraphicsItem *item)          { _items.erase(item);     }
    void clearItems()                                { _items.clear();         }
    bool hasItems() const                            { return !_items.empty(); }
    const std::set<Ns3DGraphicsItem*> &items() const { return _items;          }

    Ns3DManipulatorPivot           *pivot()          { return _pivot; }
    Ns3DManipulatorComponent       *xComp()          { return _xComp; }
    Ns3DManipulatorComponent       *yComp()          { return _yComp; }
    Ns3DManipulatorComponent       *zComp()          { return _zComp; }
    const Ns3DManipulatorPivot     *pivot() const    { return _pivot; }
    const Ns3DManipulatorComponent *xComp() const    { return _xComp; }
    const Ns3DManipulatorComponent *yComp() const    { return _yComp; }
    const Ns3DManipulatorComponent *zComp() const    { return _zComp; }

    GLfloat perspScale() const                       { return _perspScale; }
    void setPerspScale(const GLfloat ps)             { _perspScale = ps;   }

    NsUndoStack*
    undoStack() const
    { return _undoStack; }

private:    // Member variables

    Ns3DManipulatorPivot     *_pivot;
    Ns3DManipulatorComponent *_xComp;
    Ns3DManipulatorComponent *_yComp;
    Ns3DManipulatorComponent *_zComp;

    GLfloat _perspScale;

    NsUndoStack *_undoStack;

    std::set<Ns3DGraphicsItem*> _items;    // Items being manipulated

// -----------------------------------------------------------------------------

public:

    // Stipple pattern for rendering manipulators with "fake transparency"

    static const GLubyte halftone[];

public:

    static Ngl::vec3f dragPlaneNormal(const Ngl::Viewport &vp,
                                      const em::glmat44f  &mv,
                                      const em::glmat44f  &proj,
                                      const Ngl::vec3f    &ori,
                                      const Ngl::vec3f    &nml0,
                                      const Ngl::vec3f    &nml1);

    static GLfloat dragPlaneDist(const Ngl::Viewport &vp,
                                 const em::glmat44f  &mv,
                                 const em::glmat44f  &proj,
                                 int                  mx,
                                 int                  my,
                                 const Ngl::vec3f    &dpn,
                                 const Ngl::vec3f    &dpOri,
                                 const Ngl::vec3f    &axis);

    static bool viewFacingAxis(const Ngl::vec3f    &view,
                               const Ngl::vec3f    &axis,
                               GLfloat              maxViewDot);

private:    // Disabled

    Ns3DManipulator();
    Ns3DManipulator(const Ns3DManipulator&);
    Ns3DManipulator& operator=(const Ns3DManipulator&);
};

#endif // NS3D_MANIPULATOR_H
