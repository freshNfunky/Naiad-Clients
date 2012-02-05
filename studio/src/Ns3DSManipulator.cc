// -----------------------------------------------------------------------------
//
// Ns3DSManipulator.cc
//
// Widget for manipulating the scale(s) of selected object(s).
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

#include "Ns3DSManipulator.h"
#include "Ns3DGraphicsItem.h"
#include "Ns3DSelectionManager.h"
#include "Ns3DOpItem.h"
#include "Ns3DOpImportTransformItem.h"
#include "Ns3DOpVectorItem.h"
#include "NsCmdSetParam.h"
#include "NsCmdSetParam3.h"
#include "NsOpObject.h"

#include <NglViewport.h>
#include <NglState.h>

#include <em_glmat44_algo.h>


// -----------------------------------------------------------------------------

// draw
// ----
//! Draw the pivot.

void
Ns3DSManipulatorPivot::draw(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj) const
{
    isPressed() ? _pressMaterial.set() : _material.set();
    _cube.draw();
}


// selectionDraw
// -------------
//! Draw the pivot for selection.

void
Ns3DSManipulatorPivot::selectionDraw(const Ngl::Viewport &vp,
                                     const em::glmat44f  &mv,
                                     const em::glmat44f  &proj) const
{
    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(selId(), &rgba);
    glColor4ubv(&rgba[0]);
    _cube.draw();
}


// mouseDrag
// ---------
//! Update transforms of items being manipulated.

void
Ns3DSManipulatorPivot::mouseDrag(const Ngl::Viewport &vp,
                                 const em::glmat44f  &mv,
                                 const em::glmat44f  &proj,
                                 const int            mdx,
                                 const int            mdy,
                                 const int            mx,
                                 const int            my)
{
    parent()->xComp()->mouseDrag(vp, mv, proj, mdx, mdy, mx, my, true);
    parent()->yComp()->mouseDrag(vp, mv, proj, mdx, mdy, mx, my, true);
    parent()->zComp()->mouseDrag(vp, mv, proj, mdx, mdy, mx, my, true);

    // Update objects being manipulated

    typedef std::set<Ns3DGraphicsItem *>::iterator ItemIter;
    typedef std::vector<Ngl::vec3f>::const_iterator ValIter;

    ItemIter itemIter = parent()->items().begin();
    ValIter valIterX = parent()->xComp()->pressValues().begin();
    ValIter valIterY = parent()->yComp()->pressValues().begin();
    ValIter valIterZ = parent()->zComp()->pressValues().begin();
    NsCmdSetParam3::ArgsList argsList3;
    NsCmdSetParam::ArgsList argsList;

    while ((itemIter != parent()->items().end()) &&
           (valIterX != parent()->xComp()->pressValues().end()) &&
           (valIterY != parent()->yComp()->pressValues().end()) &&
           (valIterZ != parent()->zComp()->pressValues().end())) {


        if ((*itemIter)->isVisible()) {

            Ns3DOpItem *opItem(dynamic_cast<Ns3DOpItem*>(*itemIter));

            if (0 != opItem) {
                const Ngl::vec3f s(
                    (*valIterX)[0]*parent()->xComp()->dragDist(),
                    (*valIterY)[1]*parent()->yComp()->dragDist(),
                    (*valIterZ)[2]*parent()->zComp()->dragDist());

                const QString paramName(opItem->scaleParamName());
                if ("Radius" == paramName || "Length" == paramName) {
                    argsList +=
                        NsCmdSetParam::Args(
                            opItem->opObject()->longName(),
                            paramName,
                            QString::number(fabs(s[0])),
                            0);
                }
                else {
                    argsList3 +=
                        NsCmdSetParam3::Args(
                            opItem->opObject()->longName(),
                            paramName,
                            QString::number(s[0]),
                            QString::number(s[1]),
                            QString::number(s[2]));
                }
            }

//            (*itemIter)->setScale(
//                    Ngl::vec3f(
//                        (*valIterX)[0]*parent()->xComp()->dragDist(),
//                        (*valIterY)[1]*parent()->yComp()->dragDist(),
//                        (*valIterZ)[2]*parent()->zComp()->dragDist()),
//                    !firstDrag());
        }

        ++itemIter;
        ++valIterX;
        ++valIterY;
        ++valIterZ;
    }


    if (!argsList.empty()) {
        if (0 != parent()->undoStack()) {
            NsCmdSetParam::exec(argsList,
                                *parent()->undoStack(),
                                !firstDrag());
        }
        else {
            NsCmdSetParam::exec(argsList);
        }
    }

    if (!argsList3.empty()) {
        if (0 != parent()->undoStack()) {
            NsCmdSetParam3::exec(argsList3,
                                 *parent()->undoStack(),
                                 !firstDrag());
        }
        else {
            NsCmdSetParam3::exec(argsList3);
        }
    }

    if (firstDrag()) {
        setFirstDrag(false);
    }
}


// mousePress
// ----------
//! Initialize dragging when the pivot is pressed.

void
Ns3DSManipulatorPivot::mousePress(const Ngl::Viewport &vp,
                                  const em::glmat44f  &mv,
                                  const em::glmat44f  &proj,
                                  const int            mx,
                                  const int            my)
{
    setPressX(mx);
    setPressY(my);

    parent()->xComp()->mousePress(vp, mv, proj, mx, my, true);
    parent()->yComp()->mousePress(vp, mv, proj, mx, my, true);
    parent()->zComp()->mousePress(vp, mv, proj, mx, my, true);
}

// -----------------------------------------------------------------------------

// draw
// ----
//! Draw the component.

void
Ns3DSManipulatorComponent::draw(const Ngl::Viewport &vp,
                                const em::glmat44f  &mv,
                                const em::glmat44f  &proj) const
{
    const Ngl::vec3f view(-mv[0][2], -mv[1][2], -mv[2][2]);

    if (!parent()->hasItems() ||
        Ns3DManipulator::viewFacingAxis(
            view, axis(),
            Ns3DManipulator::maxViewDot)) {
        return;
    }

    if (parent()->hasItems()) {
        const Ns3DOpVectorItem *vecItem = 
            dynamic_cast<const Ns3DOpVectorItem*>(*parent()->items().begin());
        if (0 != vecItem) {
            const Ngl::Material disabledMaterial(
                Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
                Ngl::vec4f(0.5f, 0.5f, 0.5f, 1.f),
                Ngl::vec4f(0.1f, 0.1f, 0.1f, 1.f),
                0.f,
                GL_FRONT);
            disabledMaterial.set();
            _tube.draw();
            _cube.draw();
        }
        else {
            isPressed() ? _pressMaterial.set() : _material.set();
            _tube.draw();
            _cube.draw();

            if (isPressed()) {
                const Ngl::vec3f aa(absAxis());
                const GLfloat dd(dragDist()/pressDist());
                _material.set();
                glPushMatrix();
                    glTranslatef(dd*aa[0],
                                 dd*aa[1],
                                 dd*aa[2]);
                    _dragCube.draw();
                glPopMatrix();
            }
        }
    }
}


// selectionDraw
// -------------
//! Draw the component for selection.

void
Ns3DSManipulatorComponent::selectionDraw(const Ngl::Viewport &vp,
                                         const em::glmat44f  &mv,
                                         const em::glmat44f  &proj) const
{
    const Ngl::vec3f view(-mv[0][2], -mv[1][2], -mv[2][2]);

    if (!parent()->hasItems() ||
        Ns3DManipulator::viewFacingAxis(
            view, axis(),
            Ns3DManipulator::maxViewDot)) {
        return;
    }

    if (parent()->hasItems()) {
        const Ns3DOpVectorItem *vecItem = 
            dynamic_cast<const Ns3DOpVectorItem*>(*parent()->items().begin());
        if (0 != vecItem) {
            // Do nothing.
        }
        else {
            Ngl::vec4uc rgba;
            Ns3DSelectionManager::idToRgba(selId(), &rgba);
            glColor4ubv(&rgba[0]);
            _tube.draw();
            _cube.draw();
        }
    }
}


// mouseDrag
// ---------
//! Update transforms of items being manipulated.

void
Ns3DSManipulatorComponent::mouseDrag(const Ngl::Viewport &vp,
                                     const em::glmat44f  &mv,
                                     const em::glmat44f  &proj,
                                     const int            mdx,
                                     const int            mdy,
                                     const int            mx,
                                     const int            my,
                                     const bool           pivot)
{
    if (parent()->hasItems()) {
        if (pivot) {
            setDragDist(dragDist() - (2.f/vp.width())*mdx);
        }
        else {
            Ngl::vec3f signedAxis(absAxis());

            if (em::dot(absAxis(), pressScale()) < 0.f) {
                signedAxis = negAxis();
            }

            setDragDist(Ns3DManipulator::dragPlaneDist(
                                vp, mv, proj,
                                mx, my,
                                dragPlaneNormal(),
                                pressTranslate(),
                                signedAxis));

            typedef std::set<Ns3DGraphicsItem *>::iterator ItemIter;
            typedef std::vector<Ngl::vec3f>::const_iterator ValIter;

            ItemIter itemIter = parent()->items().begin();
            ValIter valIter = pressValues().begin();
            NsCmdSetParam3::ArgsList argsList3;
            NsCmdSetParam::ArgsList argsList;

            while ((itemIter != parent()->items().end()) &&
                   (valIter  != pressValues().end())) {
                if ((*itemIter)->isVisible()) {
//                (*itemIter)->setScale(
//                    Ngl::vec3f(
//                       v[0]*(1.f - fabs(axisX())) + v[0]*fabs(axisX())*dd,
//                       v[1]*(1.f - fabs(axisY())) + v[1]*fabs(axisY())*dd,
//                       v[2]*(1.f - fabs(axisZ())) + v[2]*fabs(axisZ())*dd),
//                    !firstDrag()
//                    );

                    Ns3DOpItem *opItem = dynamic_cast<Ns3DOpItem*>(*itemIter);
                    Ns3DOpVectorItem *vecItem = 
                        dynamic_cast<Ns3DOpVectorItem*>(*itemIter);

                    Ngl::vec3f v = (*valIter);
#if 0
                    std::cerr << "Axis: " << axis() << "\n";
                    std::cerr << "V: " << v << "\n";
#endif

                    if (0 != vecItem) {
                        
                        //if (fabs(axisZ()) > 0.f) {
                        //    //const GLfloat len = em::mag(v);
                        //    //const Ngl::vec3f vn = v/len;
                        //    const GLfloat dd = dragDist()/pressDist();

                        //    std::cerr << "dd: " << dragDist() << "\n";
                        //    std::cerr << "pd: " << pressDist() << "\n";
                        //    //const Ngl::vec3f s(
                        //    //   v[0]*(1.f - fabs(0)/*axisX()*/) + v[0]*fabs(vn[0])/*axisX()*/*dd,
                        //    //   v[1]*(1.f - fabs(vn[1])/*axisY()*/) + v[1]*fabs(vn[1])/*axisY()*/*dd,
                        //    //   v[2]*(1.f - fabs(vn[2])/*axisZ()*/) + v[2]*fabs(vn[2])/*axisZ()*/*dd);

                        //    const Ngl::vec3f s = v*dd;

                        //    argsList3 +=
                        //        NsCmdSetParam3::Args(
                        //            opItem->opObject()->longName(),
                        //            "Vector",
                        //            QString::number(s[0]),
                        //            QString::number(s[1]),
                        //            QString::number(s[2]));
                        //}
                    }
                    else if (0 != opItem) {
                        //Ngl::vec3f v = (*valIter);
                        const GLfloat dd = dragDist()/pressDist();

                        const Ngl::vec3f s(
                           v[0]*(1.f - fabs(axisX())) + v[0]*fabs(axisX())*dd,
                           v[1]*(1.f - fabs(axisY())) + v[1]*fabs(axisY())*dd,
                           v[2]*(1.f - fabs(axisZ())) + v[2]*fabs(axisZ())*dd);

                        const QString paramName(opItem->scaleParamName());
                        if ("Radius" == paramName || "Length" == paramName) {
                            argsList +=
                                NsCmdSetParam::Args(
                                    opItem->opObject()->longName(),
                                    paramName,
                                    QString::number(fabs(s[0])),
                                    0);
                        }
                        else {
                            argsList3 +=
                                NsCmdSetParam3::Args(
                                    opItem->opObject()->longName(),
                                    paramName,
                                    QString::number(s[0]),
                                    QString::number(s[1]),
                                    QString::number(s[2]));
                        }
                    }
                }

                ++itemIter;
                ++valIter;
            }

            if (!argsList.empty()) {
                if (0 != parent()->undoStack()) {
                    NsCmdSetParam::exec(argsList,
                                        *parent()->undoStack(),
                                        !firstDrag());
                }
                else {
                    NsCmdSetParam::exec(argsList);
                }
            }

            if (!argsList3.empty()) {
                if (0 != parent()->undoStack()) {
                    NsCmdSetParam3::exec(argsList3,
                                         *parent()->undoStack(),
                                         !firstDrag());
                }
                else {
                    NsCmdSetParam3::exec(argsList3);
                }
            }

            if (firstDrag()) {
                setFirstDrag(false);
            }
        }
    }
}


// mousePress
// ----------
//! Initialize dragging when the component is pressed.

void
Ns3DSManipulatorComponent::mousePress(const Ngl::Viewport &vp,
                                      const em::glmat44f  &mv,
                                      const em::glmat44f  &proj,
                                      const int            mx,
                                      const int            my,
                                      const bool           pivot)
{
    if (!parent()->hasItems())
        return;

    setPressX(mx);
    setPressY(my);

    // Get the values of selected items at the start of the
    // manipulation.

    clearPressValues();

    typedef std::set<Ns3DGraphicsItem*>::iterator ItemIter;

    for (ItemIter iter  = parent()->items().begin();
         iter          != parent()->items().end();
         ++iter)
    {
        addPressValue((*iter)->scale());
    }

    if (pivot) {
        setPressDist(1.f);
        setDragDist(1.f);
    }
    else {
        // Choose drag plane and initialize the distance in
        // world space to the origin of the manipulator.

        setPressTranslate((*parent()->items().begin())->translate());
        setPressRotate((*parent()->items().begin())->rotate());
        setPressScale((*parent()->items().begin())->scale());

        Ngl::vec3f signedAxis(absAxis());

        if (em::dot(absAxis(), pressScale()) < 0.f) {
            signedAxis = negAxis();
        }

        //Ngl::vec3f rotAxis;
        //Ngl::vec3f rotNml0;
        //Ngl::vec3f rotNml1;

        em::quaternionf qr;
        qr.set_euler_angles(pressRotate());
        Ngl::mat44f rot(qr);

        //Ngl::multMat44fVec3f(rot[0], signedAxis, &rotAxis);
        //Ngl::multMat44fVec3f(rot[0], nml0(), &rotNml0);
        //Ngl::multMat44fVec3f(rot[0], nml1(), &rotNml1);
        const Ngl::vec3f rotNml0 = rot*nml0();
        const Ngl::vec3f rotNml1 = rot*nml1();




        setDragPlaneNormal(
            Ns3DManipulator::dragPlaneNormal(
                vp, mv, proj,
                pressTranslate(),
                rotNml0,
                rotNml1));

        setPressDist(
            Ns3DManipulator::dragPlaneDist(
                vp, mv, proj,
                mx, my,
                dragPlaneNormal(),
                pressTranslate(),
                signedAxis));

        setDragDist(pressDist());
    }
}

// -----------------------------------------------------------------------------

// Ns3DSManipulator
// ---------------
//! Constructor.

Ns3DSManipulator::Ns3DSManipulator(NsUndoStack   *undoStack,
                                   const GLfloat  cubeDim,
                                   const GLfloat  tubeLen,
                                   const GLfloat  tubeThickness)
    : Ns3DManipulator(
        new Ns3DSManipulatorPivot(
                this,
                Ns3DManipulator::pivotId,
                Ns3DManipulator::pivotMaterial,
                Ns3DManipulator::pressMaterial,
                cubeDim),
        new Ns3DSManipulatorComponent(
                this,
                Ns3DManipulator::xCompId,
                Ns3DManipulator::xAxis,     // Axis
                Ns3DManipulator::yAxis,     // Normal 0
                Ns3DManipulator::zAxis,     // Normal 1
                Ns3DManipulator::xCompMaterial,
                Ns3DManipulator::pressMaterial,
                cubeDim,
                tubeThickness,
                tubeLen),
        new Ns3DSManipulatorComponent(
                this,
                Ns3DManipulator::yCompId,
                Ns3DManipulator::yAxis,     // Axis
                Ns3DManipulator::zAxis,     // Normal 0
                Ns3DManipulator::xAxis,     // Normal 1
                Ns3DManipulator::yCompMaterial,
                Ns3DManipulator::pressMaterial,
                cubeDim,
                tubeThickness,
                tubeLen),
        new Ns3DSManipulatorComponent(
                this,
                Ns3DManipulator::zCompId,
                Ns3DManipulator::zAxis,     // Axis
                Ns3DManipulator::xAxis,     // Normal 0
                Ns3DManipulator::yAxis,     // Normal 1
                Ns3DManipulator::zCompMaterial,
                Ns3DManipulator::pressMaterial,
                cubeDim,
                tubeThickness,
                tubeLen),
        0.3f,
        undoStack),
      _cubeDim(cubeDim),
      _tubeLen(tubeLen),
      _tubeThickness(tubeThickness)
{}


// makeXForm
// ---------
//! Create a transformation used to render the manipulator.

void
Ns3DSManipulator::makeXForm(const Ngl::Viewport &vp,
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
        const Ngl::vec3f r = (*items().begin())->rotate();
        const Ngl::vec3f s = (*items().begin())->scale();

        Ngl::vec4f eye0;
        Ngl::vec4f clip0;
        Ngl::vec3f ndc0;
        Ngl::vec3f win0;
        em::project(t, vp.toArray(), mv, proj, eye0, clip0, ndc0, win0);

        // Scale away most of the perspective division.

        Ngl::vec3f xfs(1.f);
        const float invP = 1.f/std::min<float>(proj[0][0], proj[1][1]);
        if (-1 == proj[2][3]) {
            // Perspective projection.

            const float sv = perspScale()*invP*fabs(eye0[2]);
            xfs[0] = sv;
            xfs[1] = sv;
            xfs[2] = sv;
        }
        else {
            // Ortho projection.

            const float sv = perspScale()*invP;
            xfs[0] = sv;
            xfs[1] = sv;
            xfs[2] = sv;
        }

        if ((0 != xComp() && xComp()->isPressed()) ||
            (0 != pivot() && pivot()->isPressed())) {
            if (0.f > em::dot(xComp()->axis(), xComp()->pressScale()))
                xfs[0] = -fabs(xfs[0]);
        }
        else if (0.f > s[0]) {
            xfs[0] = -fabs(xfs[0]);
        }

        if ((0 != yComp() && yComp()->isPressed()) ||
            (0 != pivot() && pivot()->isPressed())) {
            if (0.f > em::dot(yComp()->axis(), yComp()->pressScale()))
                xfs[1] = -fabs(xfs[1]);
        }
        else if (0.f > s[1]) {
            xfs[1] = -fabs(xfs[1]);
        }

        if ((0 != zComp() && zComp()->isPressed()) ||
            (0 != pivot() && pivot()->isPressed())) {
            if (0.f > em::dot(zComp()->axis(), zComp()->pressScale()))
                xfs[2] = -fabs(xfs[2]);
        }
        else if (0.f > s[2]) {
            xfs[2] = -fabs(xfs[2]);
        }

        const em::glmat44f xf = em::make_transform(t, r, xfs);
        memcpy(xForm, &xf[0][0], 16*sizeof(float));
    }
}
