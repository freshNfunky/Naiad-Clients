// -----------------------------------------------------------------------------
//
// Ns3DRManipulator.cc
//
// Widget for manipulating the rotation angles of selected object(s).
//
// Copyright (c) 2011 Exotic Matter AB. All rights reserved. 
//
// This file is part of Open Naiad Studio.
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

#include "Ns3DRManipulator.h"
#include "Ns3DGraphicsItem.h"
#include "Ns3DSelectionManager.h"
#include "Ns3DOpItem.h"
#include "Ns3DOpImportTransformItem.h"
#include "Ns3DOpVectorItem.h"
#include "NsCmdSetParam3.h"
#include "NsOpObject.h"

#include <NglViewport.h>
#include <NglState.h>

#include <em_glmat44_algo.h>

// -----------------------------------------------------------------------------

GLfloat 
rad2deg(const GLfloat rad) 
{ return (180.f/M_PI)*rad; }

GLfloat 
deg2rad(const GLfloat deg) 
{ return (M_PI/180.f)*deg; }


Ngl::vec3f
eulerAngles(const em::mat33f &m)
{
    return Ngl::vec3f(
        rad2deg(atan2(-m(1,2), m(2,2))),
        rad2deg(asin(m(0,2))),
        rad2deg(atan2(-m(0,1), m(0,0))));
}


//! Convert Euler angles to quaternion.
em::quaternionf
eulerQuaternion(const Ngl::vec3f &eulerDeg)
{
    em::quaternionf q0;
    em::quaternionf q1;
    em::quaternionf q2;
    q0.set_axis_angle(em::vec3f(1.f, 0.f, 0.f), deg2rad(eulerDeg[0]));  // X
    q1.set_axis_angle(em::vec3f(0.f, 1.f, 0.f), deg2rad(eulerDeg[1]));  // Y
    q2.set_axis_angle(em::vec3f(0.f, 0.f, 1.f), deg2rad(eulerDeg[2]));  // Z
    return q0*q1*q2;
}


Ngl::vec3f
arcPoint(const int mx, const int my, const Ngl::Viewport &vp)
{
    // Normalize mouse coords to [-1, 1] in viewport.

    Ngl::vec3f p(-1.f + 2.f*(static_cast<GLfloat>(mx)/vp.width()),
                 -1.f + 2.f*(static_cast<GLfloat>(my)/vp.height()),
                  0.f);
    const GLfloat r = em::sqr(p[0]) + em::sqr(p[1]);
    if (r > 1.f) {
        const GLfloat s = 1.f/sqrtf(r);
        p[0] *= s;
        p[1] *= s;
        p[2] = 0.f;
    }
    else {
        p[2] = sqrtf(1.f - r);
    }
    return p;
}


Ngl::vec3f
arcPoint(const int mx, const int my, const Ngl::vec2f &ctr, const GLfloat rad)
{
    const GLfloat dx = mx - ctr[0];
    const GLfloat dy = my - ctr[1];
    Ngl::vec3f p(dx/rad, dy/rad, 0.f);
    const GLfloat r = em::sqr(p[0]) + em::sqr(p[1]);
    if (r > 1.f) {
        const GLfloat s(1.f/sqrtf(r));
        p[0] *= s;
        p[1] *= s;
        p[2] = 0.f;
    }
    else {
        p[2] = sqrtf(1.f - r);
    }
    return p;
}


Ngl::vec3f
arcPointAxis(const Ngl::vec3f &p, const Ngl::vec3f &axis)
{
    const Ngl::vec3f prj(p - em::dot(p, axis)*axis);
    const GLfloat    nrm(em::mag(prj));

    if (nrm > 0.f) {
        GLfloat s(1.f/nrm);
        if (prj[2] < 0.f) {
            s = -s;
        }
        return s*prj;
    }
    else if (axis[2] == 1.f) {
        return Ngl::vec3f(1.f, 0.f, 0.f);
    }
    return em::normalized(Ngl::vec3f(-axis[1], axis[0], 0.f));
}


Ngl::vec3f
arcPointCam(const Ngl::vec3f   &p,
            const em::glmat44f &mv)
{
    // Account for camera orientation.

//    const Ngl::vec3f camx(mv[0][0], mv[1][0], mv[2][0]);
//    const Ngl::vec3f camy(mv[0][1], mv[1][1], mv[2][1]);
//    const Ngl::vec3f camz(mv[0][2], mv[1][2], mv[2][2]);
    const Ngl::vec3f camx(mv[0][0], mv[0][1], mv[0][2]);
    const Ngl::vec3f camy(mv[1][0], mv[1][1], mv[1][2]);
    const Ngl::vec3f camz(mv[2][0], mv[2][1], mv[2][2]);
    return Ngl::vec3f(em::dot(p, camx),
                      em::dot(p, camy),
                      em::dot(p, camz));
}


em::quaternionf
arcQuaternion(const Ngl::vec3f &p0,
              const Ngl::vec3f &p1)
{
    const GLfloat    d01(em::dot(p0, p1));
    const Ngl::vec3f c01(em::cross(p0, p1));
    return em::quaternionf(d01, c01[0], c01[1], c01[2]);
}

// -----------------------------------------------------------------------------

// draw
// ----
//! Draw the pivot.

void
Ns3DRManipulatorPivot::draw(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj) const
{
    const Ngl::vec3f t((*parent()->items().begin())->translate());

    Ngl::vec4f eye;
    Ngl::vec4f clip;
    Ngl::vec3f ndc;
    Ngl::vec3f win;

    em::project(t, vp.toArray(), mv, proj, eye, clip, ndc, win);
    //Ngl::project(t, vp, mv, proj, 0, 0, 0, &win);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(win[0], win[1], 0.f);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, vp.width(), 0.0, vp.height(), -1, 1);

    glDisable(GL_LIGHTING);
    //glColor3f(0.4f, 0.2f, 0.1f);
    //_disk.draw();
    isPressed() ?
        glColor3fv(&_pressColor[0]) :
        glColor3fv(&_color[0]);
    _ring.draw(_lineWidth);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

}


// selectionDraw
// -------------
//! Draw the pivot for selection.

void
Ns3DRManipulatorPivot::selectionDraw(const Ngl::Viewport &vp,
                                     const em::glmat44f  &mv,
                                     const em::glmat44f  &proj) const
{
    const Ngl::vec3f t((*parent()->items().begin())->translate());
    Ngl::vec4f eye;
    Ngl::vec4f clip;
    Ngl::vec3f ndc;
    Ngl::vec3f win;
    em::project(t, vp.toArray(), mv, proj, eye, clip, ndc, win);
    //Ngl::project(t, vp, mv, proj, 0, 0, 0, &win);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(win[0], win[1], 0.f);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0, vp.width(), 0.0, vp.height(), -1, 1);

    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(selId(), &rgba);
    glColor4ubv(&rgba[0]);
    _disk.draw();
    glPopMatrix();

    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

}


// mouseDrag
// ---------
//! Update transforms of items being manipulated.

void
Ns3DRManipulatorPivot::mouseDrag(const Ngl::Viewport &vp,
                                 const em::glmat44f  &mv,
                                 const em::glmat44f  &proj,
                                 const int            mdx,
                                 const int            mdy,
                                 const int            mx,
                                 const int            my)
{
    if (!parent()->hasItems())
        return;

    // Initialize point on "sphere" (p1) from current mouse position.
    const Ngl::vec3f t((*parent()->items().begin())->translate());
    Ngl::vec4f eye;
    Ngl::vec4f clip;
    Ngl::vec3f ndc;
    Ngl::vec3f win;
    em::project(t, vp.toArray(), mv, proj, eye, clip, ndc, win);

    const Ngl::vec3f p1(
        arcPointCam(
            arcPoint(mx, my, Ngl::vec2f(win[0], win[1]), _radius), mv));

    // Use current (p1) and initial point (_p0) to construct
    // a quaternion (q1) for the rotation from _p0 to p1.

    const em::quaternionf q1(arcQuaternion(_p0, p1));

#if 0
    std::cerr << "P0: "    << _p0 << "\n"
              << "P1: "    << p1  << "\n";
#endif

    // Set rotation in items.

    typedef std::set<Ns3DGraphicsItem*>::iterator   ItemIter;
    typedef std::vector<Ngl::vec3f>::const_iterator ValIter;

    ItemIter   itemIter = parent()->items().begin();
    ValIter    valIter  = pressValues().begin();
    em::mat33f r1;  // Temp used for retrieving current rotation.
    NsCmdSetParam3::ArgsList argsList;

    while ((itemIter != parent()->items().end()) &&
           (valIter  != pressValues().end())) {

        if ((*itemIter)->isVisible()) {
            // Multiply current quaternion (q1) with quaternion
            // for initial rotation. Initial rotation computed separately
            // for each item.

            //em::rotation_matrix(q1*eulerQuaternion(*valIter), r1);

            // Set item rotation to Euler angles of current rotation.

            //(*itemIter)->setRotate(eulerAngles(r1), !firstDrag());


            Ns3DOpItem *opItem = dynamic_cast<Ns3DOpItem*>(*itemIter);
            Ns3DOpVectorItem *vecItem = dynamic_cast<Ns3DOpVectorItem*>(*itemIter);
            if (vecItem) {
                em::rotation_matrix(q1*eulerQuaternion(*valIter), r1);
                Ngl::vec3f v = r1*Ngl::vec3f(0.f, 0.f, 1.f); // Local z-axis.
                v *= em::mag(vecItem->scale()); // Preserve length.
                const Ngl::vec3f angles = eulerAngles(r1);

                NsCmdSetParam3::exec(
                    NsCmdSetParam3::ArgsList() << 
                        NsCmdSetParam3::Args(
                            opItem->opObject()->longName(),
                            "Vector",
                            QString::number(v[0]),
                            QString::number(v[1]),
                            QString::number(v[2])));

                vecItem->setRollDeg(angles[2]);

#if 0
                std::cerr << "v1: ["    <<v[0]<<", "<<v[1]<<", "<<v[2]<<"\n";
                std::cerr << "Euler0: " << angles[0] << "\n";
                std::cerr << "Euler1: " << angles[1] << "\n";
                std::cerr << "Euler2: " << angles[2] << "\n";
#endif

                //argsList +=
                //    NsCmdSetParam3::Args(
                //        opItem->opObject()->longName(),
                //        "Vector",
                //        QString::number(v[0]),
                //        QString::number(v[1]),
                //        QString::number(v[2]));
            }
            else if (0 != opItem) {
                em::rotation_matrix(q1*eulerQuaternion(*valIter), r1);
                const Ngl::vec3f angles(eulerAngles(r1));

                const QString paramName(opItem->rotateParamName());
                if (!paramName.isEmpty()) {
                    argsList +=
                        NsCmdSetParam3::Args(
                            opItem->opObject()->longName(),
                            paramName,
                            QString::number(angles[0]),
                            QString::number(angles[1]),
                            QString::number(angles[2]));
                }
            }
        }

        ++itemIter;
        ++valIter;
    }

    if (0 != parent()->undoStack()) {
        NsCmdSetParam3::exec(argsList, *parent()->undoStack(), !firstDrag());
    }
    else {
        NsCmdSetParam3::exec(argsList);
    }

    if (firstDrag()) {
        setFirstDrag(false);
    }
}


// mousePress
// ----------
//! Initialize dragging when the pivot is pressed.

void
Ns3DRManipulatorPivot::mousePress(const Ngl::Viewport &vp,
                                  const em::glmat44f  &mv,
                                  const em::glmat44f  &proj,
                                  const int            mx,
                                  const int            my)
{
    if (!parent()->hasItems()) {
        return;
    }

    setPressX(mx);
    setPressY(my);

    // Get the values of selected items at the start of the
    // manipulation.

    clearPressValues();

    typedef std::set<Ns3DGraphicsItem *>::iterator ItemIter;

    for (ItemIter iter = parent()->items().begin();
         iter != parent()->items().end();
         ++iter)
    {
        addPressValue((*iter)->rotate());
    }

    //setPressTranslate((*parent()->items().begin())->translate());
    //setPressRotate((*parent()->items().begin())->rotate());
    //setPressScale((*parent()->items().begin())->scale());

    // Set initial point (_p0) on "sphere" generated by mouse-click.

    const Ngl::vec3f t((*parent()->items().begin())->translate());
    Ngl::vec4f eye;
    Ngl::vec4f clip;
    Ngl::vec3f ndc;
    Ngl::vec3f win;
    em::project(t, vp.toArray(), mv, proj, eye, clip, ndc, win);

    _p0 =
        arcPointCam(
                arcPoint(mx, my, Ngl::vec2f(win[0], win[1]), _radius), mv);
    //_p0 = arcPointCam(arcPoint(mx, my, vp), mv);

    // Propagate to axis-components, make them look selected.

    //parent()->xComp()->mousePress(vp, mv, proj, mx, my, true);
    //parent()->yComp()->mousePress(vp, mv, proj, mx, my, true);
    //parent()->zComp()->mousePress(vp, mv, proj, mx, my, true);
}


// mouseRelease
// ----
//! Reset state.

void
Ns3DRManipulatorPivot::mouseRelease()
{
    Ns3DManipulatorPivot::mouseRelease();
    _p0 = Ngl::vec3f(0.f);
}

// -----------------------------------------------------------------------------

// draw
// ----
//! Draw the component.

void
Ns3DRManipulatorComponent::draw(const Ngl::Viewport &vp,
                                const em::glmat44f  &mv,
                                const em::glmat44f  &proj) const
{
    isPressed() ? _pressMaterial.set() : _material.set();
    glPushMatrix();
        glRotatef(90.f, axis()[1], nml0()[1], nml1()[1]);
        _torus.draw();
    glPopMatrix();
}


// selectionDraw
// -------------
//! Draw the component for selection.

void
Ns3DRManipulatorComponent::selectionDraw(const Ngl::Viewport &vp,
                                         const em::glmat44f  &mv,
                                         const em::glmat44f  &proj) const
{
    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(selId(), &rgba);
    glColor4ubv(&rgba[0]);
    glPushMatrix();
        glRotatef(90.f, axis()[1], nml0()[1], nml1()[1]);
        _torus.draw();
    glPopMatrix();
}


// mouseDrag
// ---------
//! Update transforms of items being manipulated.

void
Ns3DRManipulatorComponent::mouseDrag(const Ngl::Viewport &vp,
                                     const em::glmat44f  &mv,
                                     const em::glmat44f  &proj,
                                     const int            mdx,
                                     const int            mdy,
                                     const int            mx,
                                     const int            my,
                                     const bool           pivot)
{
    if (!parent()->hasItems()) {
        return;
    }

    // Initialize p1 from current mouse position.

    em::mat33f ra;
    em::rotation_matrix(eulerQuaternion(pressValue(0)), ra);

    const Ngl::vec3f p1(
        arcPointAxis(
            arcPointCam(arcPoint(mx, my, vp), mv), ra*axis()));

    // Use current and initial point to construct
    // a quaternion for the current rotation.

    const em::quaternionf q1(arcQuaternion(_p0, p1));

    // Set rotation in items.

    typedef std::set<Ns3DGraphicsItem *>::iterator  ItemIter;
    typedef std::vector<Ngl::vec3f>::const_iterator ValIter;

    ItemIter   itemIter = parent()->items().begin();
    ValIter    valIter  = pressValues().begin();
    em::mat33f r1;
    NsCmdSetParam3::ArgsList argsList;

    while ((itemIter != parent()->items().end()) &&
           (valIter  != pressValues().end())) {

        // Multiply current quaternion (q1) with quaternion
        // for initial rotation.

        //em::rotation_matrix(q1*eulerQuaternion(*valIter), r1);
        //(*itemIter)->setRotate(eulerAngles(r1), !firstDrag());

        if ((*itemIter)->isVisible()) {
            Ns3DOpItem *opItem = dynamic_cast<Ns3DOpItem*>(*itemIter);
            Ns3DOpVectorItem *vecItem = 
                dynamic_cast<Ns3DOpVectorItem*>(*itemIter);

            if (vecItem) {
                em::rotation_matrix(q1*eulerQuaternion(*valIter), r1);
                Ngl::vec3f v = r1*Ngl::vec3f(0.f, 0.f, 1.f); // Local z-axis.
                v *= em::mag(vecItem->scale()); // Preserve length.
                const Ngl::vec3f angles = eulerAngles(r1);

                NsCmdSetParam3::exec(
                    NsCmdSetParam3::ArgsList() << 
                        NsCmdSetParam3::Args(
                            opItem->opObject()->longName(),
                            "Vector",
                            QString::number(v[0]),
                            QString::number(v[1]),
                            QString::number(v[2])));

                vecItem->setRollDeg(angles[2]);
#if 0
                std::cerr << "v1: ["    << v[0]<<", "<<v[1]<<", "<<v[2]<<"]\n";
                std::cerr << "Euler0: " << angles[0] << "\n";
                std::cerr << "Euler1: " << angles[1] << "\n";
                std::cerr << "Euler2: " << angles[2] << "\n";
#endif
                //argsList +=
                //    NsCmdSetParam3::Args(
                //        opItem->opObject()->longName(),
                //        "Vector",
                //        QString::number(v[0]),
                //        QString::number(v[1]),
                //        QString::number(v[2]));
            }
            else if (0 != opItem) {
                em::rotation_matrix(q1*eulerQuaternion(*valIter), r1);
                //(*itemIter)->setRotate(eulerAngles(r1), !firstDrag());
                const Ngl::vec3f angles = eulerAngles(r1);

                const QString paramName(opItem->rotateParamName());
                if (!paramName.isEmpty()) {
                    argsList +=
                        NsCmdSetParam3::Args(
                            opItem->opObject()->longName(),
                            paramName,
                            QString::number(angles[0]),
                            QString::number(angles[1]),
                            QString::number(angles[2]));
                }
            }
        }
        ++itemIter;
        ++valIter;
    }

    if (0 != parent()->undoStack()) {
        NsCmdSetParam3::exec(argsList, *parent()->undoStack(), !firstDrag());
    }
    else {
        NsCmdSetParam3::exec(argsList);
    }

    if (firstDrag()) {
        setFirstDrag(false);
    }
}


// mousePress
// ----------
//! Initialize dragging when the component is pressed.

void
Ns3DRManipulatorComponent::mousePress(const Ngl::Viewport  &vp,
                                        const em::glmat44f &mv,
                                        const em::glmat44f &proj,
                                        const int           mx,
                                        const int           my,
                                        const bool          pivot)
{
    if (!parent()->hasItems())
        return;

    setPressX(mx);
    setPressY(my);

    if (pivot)
        return;

    // Get the values of selected items at the start of the
    // manipulation.

    clearPressValues();

    typedef std::set<Ns3DGraphicsItem*>::iterator ItemIter;

    for (ItemIter iter = parent()->items().begin();
         iter != parent()->items().end();
         ++iter)
    {
        addPressValue((*iter)->rotate());
    }

    //setPressTranslate((*parent()->items().begin())->translate());
    //setPressRotate((*parent()->items().begin())->rotate());
    //setPressScale((*parent()->items().begin())->scale());

    // Compute axis in object coords.

    em::mat33f ra;
    em::rotation_matrix(eulerQuaternion(pressValue(0)), ra);

    _p0 = arcPointAxis(arcPointCam(arcPoint(mx, my, vp), mv), ra*axis());
}


// mouseRelease
// ----
//! Reset state.

void
Ns3DRManipulatorComponent::mouseRelease()
{
    Ns3DManipulatorComponent::mouseRelease();
    _p0 = Ngl::vec3f(0.f);
}

// -----------------------------------------------------------------------------

// Ns3DRManipulator
// ---------------
//! Constructor.

Ns3DRManipulator::Ns3DRManipulator(
        NsUndoStack   *undoStack,
        const GLfloat  innerRadius,
        const GLfloat  outerRadius,
        const GLfloat  pivotRadius,
        const GLfloat  pivotLineWidth,
        const GLint    subdSide,
        const GLint    subdRing)
    : Ns3DManipulator(
        new Ns3DRManipulatorPivot(
                this,
                Ns3DManipulator::pivotId,
                Ns3DManipulator::pivotColor,
                Ns3DManipulator::pressColor,
                pivotLineWidth,
                pivotRadius,
                subdRing),
        new Ns3DRManipulatorComponent(
                this,
                Ns3DManipulator::xCompId,
                Ns3DManipulator::xAxis, // Axis
                Ns3DManipulator::yAxis, // Normal 0
                Ns3DManipulator::zAxis, // Normal 1
                Ns3DManipulator::xCompMaterial,
                Ns3DManipulator::pressMaterial,
                innerRadius,
                outerRadius,
                subdSide,
                subdRing),
        new Ns3DRManipulatorComponent(
                this,
                Ns3DManipulator::yCompId,
                Ns3DManipulator::yAxis, // Axis
                Ns3DManipulator::zAxis, // Normal 0
                Ns3DManipulator::xAxis, // Normal 1
                Ns3DManipulator::yCompMaterial,
                Ns3DManipulator::pressMaterial,
                innerRadius,
                outerRadius,
                subdSide,
                subdRing),
        new Ns3DRManipulatorComponent(
                this,
                Ns3DManipulator::zCompId,
                Ns3DManipulator::zAxis, // Axis
                Ns3DManipulator::xAxis, // Normal 0
                Ns3DManipulator::yAxis, // Normal 1
                Ns3DManipulator::zCompMaterial,
                Ns3DManipulator::pressMaterial,
                innerRadius,
                outerRadius,
                subdSide,
                subdRing),
        0.3f,
        undoStack),
      _innerRadius(innerRadius),
      _outerRadius(outerRadius),
      _subdSide(subdSide),
      _subdRing(subdRing)
{

#if 0
    float t1 = deg2rad(60.f);
    float t2 = deg2rad(50.f);
    float t3 = deg2rad(40.f);

    float s1 = sinf(t1);
    float s2 = sinf(t2);
    float s3 = sinf(t3);

    float c1 = cosf(t1);
    float c2 = cosf(t2);
    float c3 = cosf(t3);


    em::mat33f r1;
    r1(0,0) = 1.f; r1(0,1) = 0.f; r1(0,2) =  0.f;
    r1(1,0) = 0.f; r1(1,1) = c1;  r1(1,2) = -s1;
    r1(2,0) = 0.f; r1(2,1) = s1;  r1(2,2) =  c1;

    em::mat33f r2;
    r2(0,0) =  c2;  r2(0,1) = 0.f; r2(0,2) = s2;
    r2(1,0) =  0.f; r2(1,1) = 1.f; r2(1,2) = 0.f;
    r2(2,0) = -s2;  r2(2,1) = 0.f; r2(2,2) = c2;

    em::mat33f r3;
    r3(0,0) = c3;  r3(0,1) = -s3;  r3(0,2) = 0.f;
    r3(1,0) = s3;  r3(1,1) =  c3;  r3(1,2) = 0.f;
    r3(2,0) = 0.f; r3(2,1) =  0.f; r3(2,2) = 1.f;

    em::mat33f r123 = r1*r2*r3;
    em::mat33f r321 = r3*r2*r1;
/*
    std::cerr   << "R1:\n" << r1 << "\n"
                << "EA: " << eulerAngles(r1) << "\n"
                << "\n"
                << "R2:\n" << r2 << "\n"
                << "EA: " << eulerAngles(r2) << "\n"
                << "\n"
                << "R3:\n" << r3 << "\n"
                << "EA: " << eulerAngles(r3) << "\n"
                << "\n"
                << "R123:\n" << r123 << "\n"
                << "EA: " << eulerAngles(r123) << "\n"
                << "\n"
                << "R321:\n" << r321 << "\n"
                << "EA: " << eulerAngles(r321) << "\n"
                << "\n";
*/
    em::quaternionf q1;
    em::quaternionf q2;
    em::quaternionf q3;

    q1.set_axis_angle(em::vec3f(1.f, 0.f, 0.f), t1);
    q2.set_axis_angle(em::vec3f(0.f, 1.f, 0.f), t2);
    q3.set_axis_angle(em::vec3f(0.f, 0.f, 1.f), t3);

    em::mat33f rq1;
    em::rotation_matrix(q1, rq1);
    em::mat33f rq2;
    em::rotation_matrix(q2, rq2);
    em::mat33f rq3;
    em::rotation_matrix(q3, rq3);

    em::quaternionf q123 = q1*q2*q3;
    em::mat33f rq123;
    em::rotation_matrix(q123, rq123);
    em::quaternionf q321 = q3*q2*q1;
    em::mat33f rq321;
    em::rotation_matrix(q321, rq321);

/*
    std::cerr   << "R(q1):\n" << rq1 << "\n"
                << "EA: " << eulerAngles(rq1) << "\n"
                << "\n"
                << "R(q2):\n" << rq2 << "\n"
                << "EA: " << eulerAngles(rq2) << "\n"
                << "\n"
                << "R(q3):\n" << rq3 << "\n"
                << "EA: " << eulerAngles(rq3) << "\n"
                << "\n"
                << "R(q123):\n" << rq123 << "\n"
                << "EA: " << eulerAngles(rq123) << "\n"
                << "\n"
                << "R(q321):\n" << rq321 << "\n"
                << "EA: " << eulerAngles(rq321) << "\n"
                << "\n";
*/
#endif
}


// makeXForm
// ---------
//! Create a transformation used to render the manipulator.

void
Ns3DRManipulator::makeXForm(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj,
                            GLfloat             *xForm) const
{
    if (hasItems()) {

        // Find homogenuous component of pivot in clip-coords

        Ns3DGraphicsItem *item = *items().begin();
        Ns3DOpImportTransformItem *itItem =
            dynamic_cast<Ns3DOpImportTransformItem*>(item);
        Ngl::vec3f t;
        if (0 != itItem) {
            t = itItem->position();
        }
        else {
            t = item->translate();
        }
        //const Ngl::vec3f t = (*items().begin())->translate();

        Ngl::vec4f eye0;
        Ngl::vec4f clip0;
        Ngl::vec3f ndc0;
        Ngl::vec3f win0;
        em::project(t, vp.toArray(), mv, proj, eye0, clip0, ndc0, win0);

        // Scale away most of the perspective division.

        Ngl::vec3f s(1.f);
        const float invP = 1.f/std::min<float>(proj[0][0], proj[1][1]);
        if (-1 == proj[2][3]) {
            // Perspective projection.

            const float sv = perspScale()*invP*fabs(eye0[2]);
            s[0] = sv;
            s[1] = sv;
            s[2] = sv;
        }
        else {
            // Ortho projection.

            const float sv = perspScale()*invP;
            s[0] = sv;
            s[1] = sv;
            s[2] = sv;
        }

        const Ngl::vec3f r = (*items().begin())->rotate();
        const em::glmat44f xf = em::make_transform(t, r, s);
        memcpy(xForm, &xf[0][0], 16*sizeof(float));
    }
}


void
Ns3DRManipulator::selectionDraw(const Ngl::Viewport &vp,
                                const em::glmat44f  &mv,
                                const em::glmat44f  &proj) const
{
    if(!hasItems())
        return;

    glPushMatrix();
        GLfloat xForm[16];
        makeXForm(vp, mv, proj, xForm);   // Virtual
        glMultMatrixf(xForm);

        glDepthMask(GL_FALSE);

        if (0 != pivot())
            pivot()->selectionDraw(vp, mv, proj);

        glDepthMask(GL_TRUE);

        if (0 != xComp())
            xComp()->selectionDraw(vp, mv, proj);

        if (0 != yComp())
            yComp()->selectionDraw(vp, mv, proj);

        if (0 != zComp())
            zComp()->selectionDraw(vp, mv, proj);

    glPopMatrix();
}
