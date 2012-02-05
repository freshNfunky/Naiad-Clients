// -----------------------------------------------------------------------------
//
// Ns3DTManipulator.cc
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

#include "Ns3DTManipulator.h"
#include "Ns3DGraphicsItem.h"
#include "Ns3DSelectionManager.h"
#include "Ns3DOpItem.h"
#include "Ns3DOpImportTransformItem.h"
#include "NsCmdSetParam3.h"
#include "NsOpObject.h"

#include <NglUtils.h>
#include <NglState.h>
#include <NglViewport.h>

#include <em_glmat44_algo.h>
#include <QtOpenGL>

// -----------------------------------------------------------------------------

// draw
// ----
//! Draw the pivot to screen.

void
Ns3DTManipulatorPivot::draw(const Ngl::Viewport &vp,
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
Ns3DTManipulatorPivot::selectionDraw(const Ngl::Viewport &vp,
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
Ns3DTManipulatorPivot::mouseDrag(const Ngl::Viewport &vp,
                                 const em::glmat44f  &mv,
                                 const em::glmat44f  &proj,
                                 const int            mdx,
                                 const int            mdy,
                                 const int            mx,
                                 const int            my)
{
    if (!parent()->hasItems() || pressValues().empty())
        return;

    // Move items in view plane. Grab the normal from MV matrix.

    const Ngl::vec3f camZ(-mv[0][2], -mv[1][2], -mv[2][2]);

    Ngl::vec3f r0;
    Ngl::vec3f rD;
    Ngl::vec3f isect;

    if (Ngl::mouseRay(vp, mv, proj, mx, my, &r0, &rD) &&
        Ngl::rayPlaneIntersect(r0, rD, camZ, pressTranslate(), &isect)) {

        typedef std::set<Ns3DGraphicsItem*>::iterator  ItemIter;
        typedef std::vector<Ngl::vec3f>::const_iterator ValIter;

        // Update first item and calculate what the moved distance was.
        // Move subsequent items by the same distance.

        ItemIter itemIter = parent()->items().begin();
        ValIter valIter = pressValues().begin();

        // TODO: What if the first item is not visible?!?!

        Ns3DOpItem *opItem = dynamic_cast<Ns3DOpItem*>(*itemIter);
        Ns3DOpImportTransformItem *itItem =
            dynamic_cast<Ns3DOpImportTransformItem*>(*itemIter);
        if (0 != itItem) {
            const Ngl::vec3f t =
                *pressValues().begin() + isect - pressTranslate();
            const NsCmdSetParam3::Args args(
                opItem->opObject()->longName(),
                opItem->translateParamName(),
                QString::number(t[0]),
                QString::number(t[1]),
                QString::number(t[2]));

            //(*itemIter)->setTranslate(isect, !firstDrag());
            if (0 != parent()->undoStack()) {
                NsCmdSetParam3::exec(
                    NsCmdSetParam3::ArgsList() << args,
                    *parent()->undoStack(),
                    !firstDrag());
            }
            else {
                NsCmdSetParam3::exec(NsCmdSetParam3::ArgsList() << args);
            }
        }
        else if (0 != opItem) {
            const NsCmdSetParam3::Args args(
                opItem->opObject()->longName(),
                opItem->translateParamName(),
                QString::number(isect[0]),
                QString::number(isect[1]),
                QString::number(isect[2]));

            //(*itemIter)->setTranslate(isect, !firstDrag());
            if (0 != parent()->undoStack()) {
                NsCmdSetParam3::exec(
                    NsCmdSetParam3::ArgsList() << args,
                    *parent()->undoStack(),
                    !firstDrag());
            }
            else {
                NsCmdSetParam3::exec(NsCmdSetParam3::ArgsList() << args);
            }
        }

        ++itemIter;
        ++valIter;

        const Ngl::vec3f diff(isect - pressTranslate());
        NsCmdSetParam3::ArgsList argsList;

        while ((itemIter != parent()->items().end()) &&
               (valIter != pressValues().end())) {

            if ((*itemIter)->isVisible()) {

                Ns3DOpItem *opItem(dynamic_cast<Ns3DOpItem*>(*itemIter));

                if (0 != opItem) {
                    //(*itemIter)->setTranslate(*valIter + diff, !firstDrag());
                    const Ngl::vec3f t(*valIter + diff);
                    argsList +=
                        NsCmdSetParam3::Args(
                            opItem->opObject()->longName(),
                            opItem->translateParamName(),
                            QString::number(t[0]),
                            QString::number(t[1]),
                            QString::number(t[2]));
                }
            }

            ++itemIter;
            ++valIter;
        }

        if (0 != parent()->undoStack()) {
            NsCmdSetParam3::exec(argsList, *parent()->undoStack(),!firstDrag());
        }
        else {
            NsCmdSetParam3::exec(argsList);
        }

        if (firstDrag()) {
            setFirstDrag(false);
        }
    }
}


// mousePress
// ----------
//! Initialize dragging when the pivot is pressed.

void
Ns3DTManipulatorPivot::mousePress(const Ngl::Viewport &vp,
                                  const em::glmat44f  &mv,
                                  const em::glmat44f  &proj,
                                  const int            mx,
                                  const int            my)
{
     if (parent()->hasItems()) {
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

//            Ns3DGraphicsItem *item = (*iter);//*parent()->items().begin();
//            Ns3DOpImportTransformItem *itItem =
//                dynamic_cast<Ns3DOpImportTransformItem*>(item);
//            if (0 != itItem) {
//                //t = itItem->position();
//                addPressValue(itItem->position());
//                //addPressValue((*iter)->translate());
//            }
//            else {
//                //t = item->translate();
//                addPressValue((*iter)->translate());
//            }

            addPressValue((*iter)->translate());
         }

         Ns3DGraphicsItem *item = *parent()->items().begin();
         Ns3DOpImportTransformItem *itItem =
             dynamic_cast<Ns3DOpImportTransformItem*>(item);
         if (0 != itItem) {
             setPressTranslate(itItem->position());
         }
         else {
             setPressTranslate(item->translate());
         }
         //setPressTranslate((*parent()->items().begin())->translate());

         // Propagate to axis-components

         parent()->xComp()->mousePress(vp, mv, proj, mx, my, true);
         parent()->yComp()->mousePress(vp, mv, proj, mx, my, true);
         parent()->zComp()->mousePress(vp, mv, proj, mx, my, true);
     }
}

// -----------------------------------------------------------------------------

// draw
// ----
//! Draw the component.

void
Ns3DTManipulatorComponent::draw(const Ngl::Viewport &vp,
                                const em::glmat44f  &mv,
                                const em::glmat44f  &proj) const
{
    // If the manipulator does not have any items or the component is
    // facing towards the viewer we don't render it.

    const Ngl::vec3f view(-mv[0][2], -mv[1][2], -mv[2][2]);

    if (!parent()->hasItems() ||
        Ns3DManipulator::viewFacingAxis(
            view, axis(),
            Ns3DManipulator::maxViewDot))
        return;

    isPressed() ? _pressMaterial.set() : _material.set();
    _tube.draw();
    _cone.draw();
}


// selectionDraw
// -------------
//! Draw the component for selection.

void
Ns3DTManipulatorComponent::selectionDraw(const Ngl::Viewport &vp,
                                         const em::glmat44f  &mv,
                                         const em::glmat44f  &proj) const
{
    // If the manipulator does not have any items or the component is
    // facing towards the viewer we don't render it.

    const Ngl::vec3f view(-mv[0][2], -mv[1][2], -mv[2][2]);

    if (!parent()->hasItems() ||
        Ns3DManipulator::viewFacingAxis(
            view, axis(),
            Ns3DManipulator::maxViewDot))
        return;

    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(selId(), &rgba);
    glColor4ubv(&rgba[0]);
    _tube.draw();
    _cone.draw();
}


// mouseDrag
// ---------
//! Update transforms of items being manipulated.

void
Ns3DTManipulatorComponent::mouseDrag(const Ngl::Viewport &vp,
                                     const em::glmat44f  &mv,
                                     const em::glmat44f  &proj,
                                     const int            mdx,
                                     const int            mdy,
                                     const int            mx,
                                     const int            my,
                                     const bool           pivot)
{
    if (parent()->hasItems()) {
        // Update distace dragged from pivot

        setDragDist(
            Ns3DManipulator::dragPlaneDist(
                vp, mv, proj,
                mx, my,
                dragPlaneNormal(),
                pressTranslate(),
                axis()));

        // Iterate over items and set new values.

        typedef std::set<Ns3DGraphicsItem *>::iterator ItemIter;
        typedef std::vector<Ngl::vec3f>::const_iterator ValIter;

        ItemIter itemIter = parent()->items().begin();
        ValIter valIter = pressValues().begin();
        const GLfloat dd(dragDist() - pressDist());   // Dragged distance
        NsCmdSetParam3::ArgsList argsList;

        // Simply add the dragged distance to the items' translations.
        // We can do this because dragged distance is measured in world space.

        while ((itemIter != parent()->items().end()) &&
               (valIter  != pressValues().end())) {
//            (*itemIter)->setTranslate(
//                Ngl::vec3f((*valIter)[0] + fabs(axisX())*dd,
//                           (*valIter)[1] + fabs(axisY())*dd,
//                           (*valIter)[2] + fabs(axisZ())*dd),
//                !firstDrag()
//                );
            if ((*itemIter)->isVisible()) {
                Ns3DOpItem *opItem(dynamic_cast<Ns3DOpItem*>(*itemIter));

                if (0 != opItem) {
                    argsList +=
                        NsCmdSetParam3::Args(
                            opItem->opObject()->longName(),
                            opItem->translateParamName(),
                            QString::number((*valIter)[0] + fabs(axisX())*dd),
                            QString::number((*valIter)[1] + fabs(axisY())*dd),
                            QString::number((*valIter)[2] + fabs(axisZ())*dd));
                }
            }

            ++itemIter;
            ++valIter;
        }

        if (0 != parent()->undoStack()) {
            NsCmdSetParam3::exec(argsList, *parent()->undoStack(),!firstDrag());
        }
        else {
            NsCmdSetParam3::exec(argsList);
        }

        if (firstDrag()) {
            setFirstDrag(false);
        }
    }
}


// mousePress
// ----------
//! Initialize dragging when the component is pressed.

void
Ns3DTManipulatorComponent::mousePress(const Ngl::Viewport &vp,
                                      const em::glmat44f  &mv,
                                      const em::glmat44f  &proj,
                                      const int            mx,
                                      const int            my,
                                      const bool           pivot)
{
    if (parent()->hasItems()) {
        setPressX(mx);
        setPressY(my);

        if (!pivot) {
            // Get the values of selected items at the start of the
            // manipulation.

            clearPressValues();

            typedef std::set<Ns3DGraphicsItem *>::iterator ItemIter;

            for (ItemIter iter = parent()->items().begin();
                 iter != parent()->items().end();
                 ++iter)
            {
                addPressValue((*iter)->translate());
            }

            // Choose drag plane and initialize the distance in
            // world space to the origin of the manipulator.

            setPressTranslate((*parent()->items().begin())->translate());

            setDragPlaneNormal(
                Ns3DManipulator::dragPlaneNormal(
                    vp, mv, proj,
                    pressTranslate(),
                    nml0(),
                    nml1()));

            setPressDist(
                Ns3DManipulator::dragPlaneDist(
                    vp, mv, proj,
                    mx, my,
                    dragPlaneNormal(),
                    pressTranslate(),
                    axis()));
        }
    }
}

// -----------------------------------------------------------------------------

// Ns3DTManipulator
// ----------------
//! Constructor.

Ns3DTManipulator::Ns3DTManipulator(
        NsUndoStack   *undoStack,
        const GLfloat  pivotCubeDim,
        const GLfloat  tubeLen,
        const GLfloat  tubeThickness,
        const GLfloat  coneHeight,
        const GLfloat  coneRadius,
        const GLint    coneSubd)
    : Ns3DManipulator(
          new Ns3DTManipulatorPivot(
                  this,
                  Ns3DManipulator::pivotId,
                  Ns3DManipulator::pivotMaterial,
                  Ns3DManipulator::pressMaterial,
                  pivotCubeDim),
          new Ns3DTManipulatorComponent(
                  this,
                  Ns3DManipulator::xCompId,
                  Ns3DManipulator::xAxis,   // Axis
                  Ns3DManipulator::yAxis,   // Normal 0
                  Ns3DManipulator::zAxis,   // Normal 1
                  Ns3DManipulator::xCompMaterial,
                  Ns3DManipulator::pressMaterial,
                  pivotCubeDim,
                  tubeThickness,
                  tubeLen,
                  coneHeight,
                  coneRadius,
                  coneSubd),
          new Ns3DTManipulatorComponent(
                  this,
                  Ns3DManipulator::yCompId,
                  Ns3DManipulator::yAxis,   // Axis
                  Ns3DManipulator::zAxis,   // Normal 0
                  Ns3DManipulator::xAxis,   // Normal 1
                  Ns3DManipulator::yCompMaterial,
                  Ns3DManipulator::pressMaterial,
                  pivotCubeDim,
                  tubeThickness,
                  tubeLen,
                  coneHeight,
                  coneRadius,
                  coneSubd),
          new Ns3DTManipulatorComponent(
                  this,
                  Ns3DManipulator::zCompId,
                  Ns3DManipulator::zAxis,   // Axis
                  Ns3DManipulator::xAxis,   // Normal 0
                  Ns3DManipulator::yAxis,   // Normal 1
                  Ns3DManipulator::zCompMaterial,
                  Ns3DManipulator::pressMaterial,
                  pivotCubeDim,
                  tubeThickness,
                  tubeLen,
                  coneHeight,
                  coneRadius,
                  coneSubd),
          0.3f,
          undoStack),
      _pivotCubeDim(pivotCubeDim),
      _tubeLen(tubeLen),
      _tubeThickness(tubeThickness),
      _coneHeight(coneHeight),
      _coneRadius(coneRadius),
      _coneSubd(coneSubd)
{}


// makeXForm
// ---------
//! Create a transformation used to render the manipulator.

void
Ns3DTManipulator::makeXForm(const Ngl::Viewport &vp,
                            const em::glmat44f  &mv,
                            const em::glmat44f  &proj,
                            GLfloat             *xForm) const
{
    if (hasItems()) {
        // Find homogenuous component of pivot in clip-coords

//        float ndc0;
//        {
//        em::glmat44f id(1.f);
//        const Ngl::vec3f ctr(0.f, 0.f, -10.f);
//        const Ngl::vec3f wsx[8] = {
//            ctr - Ngl::vec3f(-0.5f, -0.5f, -0.5f),
//            ctr - Ngl::vec3f( 0.5f, -0.5f, -0.5f),
//            ctr - Ngl::vec3f( 0.5f, -0.5f,  0.5f),
//            ctr - Ngl::vec3f(-0.5f, -0.5f,  0.5f),
//            ctr - Ngl::vec3f(-0.5f,  0.5f, -0.5f),
//            ctr - Ngl::vec3f( 0.5f,  0.5f, -0.5f),
//            ctr - Ngl::vec3f( 0.5f,  0.5f,  0.5f),
//            ctr - Ngl::vec3f(-0.5f,  0.5f,  0.5f)
//        };
//        Ngl::vec4f eye[8];
//        Ngl::vec4f clip[8];
//        Ngl::vec3f ndc[8];
//        Ngl::vec3f win[8];
//        Ngl::vec3f ndcMin(
//            std::numeric_limits<float>::max(),
//            std::numeric_limits<float>::max(),
//            std::numeric_limits<float>::max());
//        Ngl::vec3f ndcMax(
//            -std::numeric_limits<float>::max(),
//            -std::numeric_limits<float>::max(),
//            -std::numeric_limits<float>::max());
//        for (int i = 0; i < 8; ++i) {
//            em::project(wsx[i], vp.toArray(), id, proj, eye[i], clip[i], ndc[i], win[i]);
//            ndcMin[0] = std::min<float>(ndcMin[0], ndc[i][0]);
//            ndcMin[1] = std::min<float>(ndcMin[1], ndc[i][1]);
//            ndcMin[2] = std::min<float>(ndcMin[2], ndc[i][2]);
//            ndcMax[0] = std::max<float>(ndcMax[0], ndc[i][0]);
//            ndcMax[1] = std::max<float>(ndcMax[1], ndc[i][1]);
//            ndcMax[2] = std::max<float>(ndcMax[2], ndc[i][2]);
//        }
//        ndc0 = (ndcMax[0] - ndcMin[0])*(ndcMax[1] - ndcMin[1])*(ndcMax[2] - ndcMin[2]);
//        qDebug() << "NDCO: " << ndc0;
//        }


//        float ndc1;
//        {
//        const Ngl::vec3f ctr = t;
//        const Ngl::vec3f wsx[8] = {
//            ctr - Ngl::vec3f(-0.5f, -0.5f, -0.5f),
//            ctr - Ngl::vec3f( 0.5f, -0.5f, -0.5f),
//            ctr - Ngl::vec3f( 0.5f, -0.5f,  0.5f),
//            ctr - Ngl::vec3f(-0.5f, -0.5f,  0.5f),
//            ctr - Ngl::vec3f(-0.5f,  0.5f, -0.5f),
//            ctr - Ngl::vec3f( 0.5f,  0.5f, -0.5f),
//            ctr - Ngl::vec3f( 0.5f,  0.5f,  0.5f),
//            ctr - Ngl::vec3f(-0.5f,  0.5f,  0.5f)
//        };
//        Ngl::vec4f eye[8];
//        Ngl::vec4f clip[8];
//        Ngl::vec3f ndc[8];
//        Ngl::vec3f win[8];
//        Ngl::vec3f ndcMin(
//            std::numeric_limits<float>::max(),
//            std::numeric_limits<float>::max(),
//            std::numeric_limits<float>::max());
//        Ngl::vec3f ndcMax(
//            -std::numeric_limits<float>::max(),
//            -std::numeric_limits<float>::max(),
//            -std::numeric_limits<float>::max());
//        for (int i = 0; i < 8; ++i) {
//            em::project(wsx[i], vp.toArray(), mv, proj, eye[i], clip[i], ndc[i], win[i]);
//            ndcMin[0] = std::min<float>(ndcMin[0], ndc[i][0]);
//            ndcMin[1] = std::min<float>(ndcMin[1], ndc[i][1]);
//            ndcMin[2] = std::min<float>(ndcMin[2], ndc[i][2]);
//            ndcMax[0] = std::max<float>(ndcMax[0], ndc[i][0]);
//            ndcMax[1] = std::max<float>(ndcMax[1], ndc[i][1]);
//            ndcMax[2] = std::max<float>(ndcMax[2], ndc[i][2]);
//        }
//        ndc1 = (ndcMax[0] - ndcMin[0])*(ndcMax[1] - ndcMin[1])*(ndcMax[2] - ndcMin[2]);
//        qDebug() << "NDC1: " << ndc1;
//        }

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
        Ngl::vec3f ndc0_tmp;
        Ngl::vec3f win0;
        em::project(t, vp.toArray(), mv, proj, eye0, clip0, ndc0_tmp, win0);


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

        // Scale away most of the perspective division.
        // Always draw "unrotated", i.e. with world-space axes.

        const em::glmat44f xf = em::make_transform(t, Ngl::vec3f(0.f), s);
        memcpy(xForm, &xf[0][0], 16*sizeof(float));
    }
}
