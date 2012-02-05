// -----------------------------------------------------------------------------
//
// Ns3DManipulator.cc
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

#include "Ns3DManipulator.h"
#include "Ns3DManipulatorPivot.h"
#include "Ns3DManipulatorComponent.h"
#include <NglViewport.h>
#include <NglUtils.h>
#include <NglState.h>
#include <cmath>

// -----------------------------------------------------------------------------

const Ngl::Material Ns3DManipulator::pivotMaterial(
        Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
        Ngl::vec4f(0.9f, 0.9f, 0.9f, 1.f),
        Ngl::vec4f(0.1f, 0.1f, 0.1f, 1.f),
        0.f,
        GL_FRONT);

const Ngl::Material Ns3DManipulator::xCompMaterial(
        Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
        Ngl::vec4f(0.9f, 0.1f, 0.1f, 1.f),
        Ngl::vec4f(0.1f, 0.1f, 0.1f, 1.f),
        0.f,
        GL_FRONT);

const Ngl::Material Ns3DManipulator::yCompMaterial(
        Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
        Ngl::vec4f(0.1f, 0.9f, 0.1f, 1.f),
        Ngl::vec4f(0.1f, 0.1f, 0.1f, 1.f),
        0.f,
        GL_FRONT);

const Ngl::Material Ns3DManipulator::zCompMaterial(
        Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
        Ngl::vec4f(0.1f, 0.1f, 0.9f, 1.f),
        Ngl::vec4f(0.1f, 0.1f, 0.1f, 1.f),
        0.f,
        GL_FRONT);

const Ngl::Material Ns3DManipulator::pressMaterial(
        Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
        Ngl::vec4f(0.9f, 0.9f, 0.1f, 1.f),
        Ngl::vec4f(0.1f, 0.1f, 0.1f, 1.f),
        0.f,
        GL_FRONT);

const Ngl::vec3f Ns3DManipulator::pivotColor(0.9f, 0.9f, 0.9f);
const Ngl::vec3f Ns3DManipulator::pressColor(0.9f, 0.9f, 0.1f);


const Ngl::Light Ns3DManipulator::light(
        Ngl::vec4f(1.f, 1.f, 1.f, 1.f),
        Ngl::vec4f(1.f, 1.f, 1.f, 1.f),
        Ngl::vec4f(1.f, 1.f, 1.f, 1.f));

const Ngl::vec3f Ns3DManipulator::xAxis(1.f, 0.f, 0.f);
const Ngl::vec3f Ns3DManipulator::yAxis(0.f, 1.f, 0.f);
const Ngl::vec3f Ns3DManipulator::zAxis(0.f, 0.f, 1.f);

const GLfloat Ns3DManipulator::maxViewDot(0.95f);

// -----------------------------------------------------------------------------

// Ns3DManipulator
// ---------------
//! Constructor.

Ns3DManipulator::Ns3DManipulator(Ns3DManipulatorPivot     *pivot,
                                 Ns3DManipulatorComponent *xComp,
                                 Ns3DManipulatorComponent *yComp,
                                 Ns3DManipulatorComponent *zComp,
                                 const GLfloat             perspScale,
                                 NsUndoStack              *undoStack)
    : _pivot(pivot),
      _xComp(xComp),
      _yComp(yComp),
      _zComp(zComp),
      _perspScale(perspScale),
      _undoStack(undoStack)
{
}


// ~Ns3DManipulator
// ---------------
//! Destructor.

Ns3DManipulator::~Ns3DManipulator()
{
    // Free memory for components.

    delete _pivot;
    delete _xComp;
    delete _yComp;
    delete _zComp;
}


// selectionDraw
// -------------
//! Draw on screen.

void
Ns3DManipulator::draw(const Ngl::Viewport &vp,
                      const em::glmat44f  &mv,
                      const em::glmat44f  &proj) const
{
    if(!hasItems())
        return;

    Ngl::FlipState<GL_DEPTH_TEST>      depthTestState;
    Ngl::FlipState<GL_LIGHTING>        lightingState;
    Ngl::FlipState<GL_POLYGON_STIPPLE> polygonStippleState;
    Ngl::FlipState<GL_NORMALIZE>       normalizeState;
    Ngl::FlipState<GL_CULL_FACE>       cullFaceState;

    // Lighting

    glPushMatrix();
        glLoadIdentity();
        Ngl::FlipState<GL_LIGHTING>::enable();
        glEnable(Ns3DManipulator::light.index);
        Ns3DManipulator::light.set();
    glPopMatrix();

    glPushMatrix();

        // Normals are scaled to length 1
        Ngl::FlipState<GL_NORMALIZE>::enable();

        GLfloat xForm[16];
        makeXForm(vp, mv, proj, xForm); // Virtual
        glMultMatrixf(xForm);

        // Pass 1, draw stippled

        Ngl::FlipState<GL_DEPTH_TEST>::disable();
        Ngl::FlipState<GL_POLYGON_STIPPLE>::enable();
        Ngl::FlipState<GL_CULL_FACE>::enable();
        glCullFace(GL_BACK);

        glPolygonStipple(Ns3DManipulator::halftone);

        if (0 != pivot())
            pivot()->draw(vp, mv, proj);

        if (0 != xComp())
            xComp()->draw(vp, mv, proj);

        if (0 != yComp())
            yComp()->draw(vp, mv, proj);

        if (0 != zComp())
            zComp()->draw(vp, mv, proj);

        Ngl::FlipState<GL_CULL_FACE>::disable();
        Ngl::FlipState<GL_POLYGON_STIPPLE>::disable();
        Ngl::FlipState<GL_DEPTH_TEST>::enable();

        // Pass 2, draw normally

        if (0 != pivot())
            pivot()->draw(vp, mv, proj);

        if (0 != xComp())
            xComp()->draw(vp, mv, proj);

        if (0 != yComp())
            yComp()->draw(vp, mv, proj);

        if (0 != zComp())
            zComp()->draw(vp, mv, proj);
    glPopMatrix();
}


// selectionDraw
// -------------
//! Draw for selection.

void
Ns3DManipulator::selectionDraw(const Ngl::Viewport &vp,
                               const em::glmat44f  &mv,
                               const em::glmat44f  &proj) const
{
    if(!hasItems())
        return;

    glPushMatrix();
        GLfloat xForm[16];
        makeXForm(vp, mv, proj, xForm);   // Virtual
        glMultMatrixf(xForm);
        if (0 != pivot())
            pivot()->selectionDraw(vp, mv, proj);

        if (0 != xComp())
            xComp()->selectionDraw(vp, mv, proj);

        if (0 != yComp())
            yComp()->selectionDraw(vp, mv, proj);

        if (0 != zComp())
            zComp()->selectionDraw(vp, mv, proj);
    glPopMatrix();
}


// mousePress
// ----------
//! Propagate mouse press to a (non-null) component based on id.

void
Ns3DManipulator::mousePress(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj,
                            const int32_t        id,
                            const int            mx,
                            const int            my)
{
    switch (id) {
    case Ns3DManipulator::pivotId:
        if (0 != _pivot)
            _pivot->mousePress(vp, mv, proj, mx, my);
        break;
    case Ns3DManipulator::xCompId:
        if (0 != _xComp)
            _xComp->mousePress(vp, mv, proj, mx, my);
        break;
    case Ns3DManipulator::yCompId:
        if (0 != _yComp)
            _yComp->mousePress(vp, mv, proj, mx, my);
        break;
    case Ns3DManipulator::zCompId:
        if (0 != _zComp)
            _zComp->mousePress(vp, mv, proj, mx, my);
        break;
    default:
        // Id does not correspond to any component, do nothing.
        break;
    }
}


// mouseRelease
// ------------
//! Propagate mouse release to all (non-null) components.

void
Ns3DManipulator::mouseRelease()
{
    if (0 != _pivot)
        _pivot->mouseRelease();
    if (0 != _xComp)
        _xComp->mouseRelease();
    if (0 != _yComp)
        _yComp->mouseRelease();
    if (0 != _zComp)
        _zComp->mouseRelease();
}


// mouseDrag
// ---------
//! Propagate mouse drag to all (non-null) components.

void
Ns3DManipulator::mouseDrag(const Ngl::Viewport &vp,
                           const em::glmat44f  &mv,
                           const em::glmat44f  &proj,
                           const int            mdx,
                           const int            mdy,
                           const int            mx,
                           const int            my)
{
    // If the pivot is clicked the other components do not
    // receive any events.

    if (_pivot != 0 && _pivot->isPressed())
        _pivot->mouseDrag(vp, mv, proj, mdx, mdy, mx, my);
    else {
        if (_xComp != 0 && _xComp->isPressed())
            _xComp->mouseDrag(vp, mv, proj, mdx, mdy, mx, my);
        if (_yComp != 0 && _yComp->isPressed())
            _yComp->mouseDrag(vp, mv, proj, mdx, mdy, mx, my);
        if (_zComp != 0 && _zComp->isPressed())
            _zComp->mouseDrag(vp, mv, proj, mdx, mdy, mx, my);
    }
}


// isPressed
// ---------
//! Returns true if at least one component is non-null and pressed.

bool
Ns3DManipulator::isPressed() const
{
    return ((_pivot != 0 && _pivot->isPressed()) ||
            (_xComp != 0 && _xComp->isPressed()) ||
            (_yComp != 0 && _yComp->isPressed()) ||
            (_zComp != 0 && _zComp->isPressed()));
}

// -----------------------------------------------------------------------------

const GLubyte Ns3DManipulator::halftone[] = {
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55,
    0xAA, 0xAA, 0xAA, 0xAA, 0x55, 0x55, 0x55, 0x55 };

// -----------------------------------------------------------------------------

// dragPlaneNormal
// ---------------
//! Drag plane normal is set to axis with shortest projection in
//! window coords.

Ngl::vec3f
Ns3DManipulator::dragPlaneNormal(const Ngl::Viewport &vp,
                                 const em::glmat44f  &mv,
                                 const em::glmat44f  &proj,
                                 const Ngl::vec3f    &ori,
                                 const Ngl::vec3f    &nml0,
                                 const Ngl::vec3f    &nml1)
{
    // Project normals into window coords.

    Ngl::vec2f winNml0;
    Ngl::vec2f winNml1;

    Ngl::projectAxis(vp, mv, proj, ori, ori + nml0, &winNml0);
    Ngl::projectAxis(vp, mv, proj, ori, ori + nml1, &winNml1);

    // Compared squared magnitudes.

    if (em::mag2(winNml1) < em::mag2(winNml0)) {
        return nml1;
    }

    return nml0;
}


// dragPlaneDist
// -------------
//! Compute distance to a point on a plane in world space given camera
//! information and mouse coordinates.

GLfloat
Ns3DManipulator::dragPlaneDist(const Ngl::Viewport &vp,
                               const em::glmat44f  &mv,
                               const em::glmat44f  &proj,
                               const int            mx,
                               const int            my,
                               const Ngl::vec3f    &dpn,
                               const Ngl::vec3f    &dpOri,
                               const Ngl::vec3f    &axis)
{
    Ngl::vec3f r0;
    Ngl::vec3f rD;
    Ngl::vec3f isect;

    if (Ngl::mouseRay(vp, mv, proj, mx, my, &r0, &rD) &&
        Ngl::rayPlaneIntersect(r0, rD, dpn, dpOri, &isect)) {

        return em::dot(axis, isect) -
                   axis[0]*dpOri[0] -
                   axis[1]*dpOri[1] -
                   axis[2]*dpOri[2];
    }
    else {
        return 0.f;
    }
}


// viewFacingAxis
// --------------
//! Returns true of the provided axis (in world space) is facing the
//! camera more than a certain amount.

bool
Ns3DManipulator::viewFacingAxis(const Ngl::vec3f    &view,
                                const Ngl::vec3f    &axis,
                                GLfloat              maxViewDot)
{
#if 0
    std::cerr   << "View: " << view << "\n"
                << "Axis: " << axis << "\n"
                << "Dot : " << fabs(em::dot(axis, view)) << "\n\n";
#endif

    if (fabs(em::dot(axis, view)) > maxViewDot)
        return true;

    return false;
}
