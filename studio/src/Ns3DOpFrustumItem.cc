// -----------------------------------------------------------------------------
//
// Ns3DOpFrustumItem.cc
//
// Ops with a "View Frustum" parameter section
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

#include "Ns3DOpFrustumItem.h"
#include "NsOpObject.h"
#include "Ns3DSelectionManager.h"
#include "Ns3DCameraScope.h"
#include "NsPreferences.h"

#include <Ni.h>
#include <NiNg.h>

#include <em_glmat44_algo.h>

#include <NglUtils.h>
#include <NglState.h>
#include <QtOpenGL>

// -----------------------------------------------------------------------------

Ns3DOpFrustumItem::Ns3DOpFrustumItem(NsOpObject       *opObject,
                                     const int32_t     selId,
                                     const Ngl::vec4f& matAmb,
                                     const Ngl::vec4f& matDiff,
                                     const Ngl::vec4f& matSpec,
                                     const Ngl::vec4f& selMatAmb,
                                     const Ngl::vec4f& selMatDiff,
                                     const Ngl::vec4f& selMatSpec,
                                     const GLfloat lineWidth,
                                     const GLfloat selLineWidth)
    : Ns3DOpBoxItem(opObject, selId,
                    matAmb, matDiff, matSpec,
                    selMatAmb, selMatDiff, selMatSpec,
                    lineWidth, selLineWidth)
{}

// -----------------------------------------------------------------------------

void
Ns3DOpFrustumItem::draw(LabelInfo *label) const
{
    //std::cerr << "Frustum: " << this << "\n";

    Ngl::vec3f wsx[8];
    _updateFrustum(wsx);

    //Ngl::PolygonModeState polygonModeState;

    //glPushMatrix();
    //    GLfloat M[16];
    //    Ngl::makeXform(translate(), rotate(), scale(), M);
    //    glMultMatrixf(M);
        QColor color;
        if (selected()) {
            color = NsPreferences::instance()->scopeViewSelectionColor();
            glLineWidth(_selLineWidth);
        }
        else {
            color = NsPreferences::instance()->scopeViewFrustumLineColor();
            glLineWidth(_lineWidth);
        }
        glColor4ub(color.red(), color.green(), color.blue(), 255);
        Ns3DOpBoxItem::drawEdge(wsx);

        Ns3DCameraScope *cam(
            dynamic_cast<Ns3DCameraScope*>(&opObject()->mutableNgOp()));

        if (cam && cam->hasImagePlane()) {
            const Ngl::FlipState<GL_BLEND>      blendState;
            const Ngl::FlipState<GL_ALPHA_TEST> alphaTestState;
            Ngl::FlipState<GL_BLEND>::enable();
            Ngl::FlipState<GL_ALPHA_TEST>::enable();

            glAlphaFunc(GL_GREATER, 0.f);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            GLfloat modelview[16];
            GLfloat projection[16];

            glGetFloatv(GL_MODELVIEW_MATRIX, modelview);
            glGetFloatv(GL_PROJECTION_MATRIX, projection);

            const GLfloat imagePlaneVtx[] = {
                wsx[4][0], wsx[4][1], wsx[4][2],
                wsx[5][0], wsx[5][1], wsx[5][2],
                wsx[6][0], wsx[6][1], wsx[6][2],
                wsx[7][0], wsx[7][1], wsx[7][2] };

            const GLfloat imagePlaneTex[] = {
                0.f, 0.f,
                1.f, 0.f,
                1.f, 1.f,
                0.f, 1.f };

            Ngl::VertexBuffer positionVbo(12*sizeof(GLfloat), imagePlaneVtx);
            Ngl::VertexBuffer texVbo(8*sizeof(GLfloat), imagePlaneTex);

            cam->drawImagePlane(em::glmat44f(modelview),
                                em::glmat44f(projection),
                                positionVbo,
                                texVbo);
        }


        if (0 != label) {
            QPointF pos;
            if (labelPos(wsx, 8, pos)) {
                label->setPos(pos);
                label->setText(opObject()->longName() + QString(" [Frustum]"));
                label->setColor(
                    NsPreferences::instance()->scopeViewFrustumTextColor());
            }
        }
}

// -----------------------------------------------------------------------------

void
Ns3DOpFrustumItem::selectionDraw() const
{
    Ngl::vec3f wsx[8];
    _updateFrustum(wsx);

    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(id(), &rgba);

    //glPushMatrix();
    //GLfloat M[16];
    //Ngl::makeXform(translate(), rotate(), scale(), M);
    //glMultMatrixf(M);
    glColor4ubv(&rgba[0]);

    //Ns3DOpBoxItem::drawFace(verts);
    Ns3DOpBoxItem::drawEdge(wsx);

    //glPopMatrix();
}

// -----------------------------------------------------------------------------

//! Frustum vertices in NDC coordinates.
const Ngl::vec4f Ns3DOpFrustumItem::_ndc[] =
{
    Ngl::vec4f(-1.f, -1.f, -1.f, 1.f),  // Near
    Ngl::vec4f( 1.f, -1.f, -1.f, 1.f),
    Ngl::vec4f( 1.f,  1.f, -1.f, 1.f),
    Ngl::vec4f(-1.f,  1.f, -1.f, 1.f),
    Ngl::vec4f(-1.f, -1.f,  1.f, 1.f),  // Far
    Ngl::vec4f( 1.f, -1.f,  1.f, 1.f),
    Ngl::vec4f( 1.f,  1.f,  1.f, 1.f),
    Ngl::vec4f(-1.f,  1.f,  1.f, 1.f)
};


void
_rotationAxes(const NtVec3f &rotDeg,
              NtVec3f       *right,
              NtVec3f       *up,
              NtVec3f       *back)
{
    static const float deg2rad(3.141593f/180.f);

    const float s0(sinf((90.f - rotDeg[0])*deg2rad));
    const float c0(cosf((90.f - rotDeg[0])*deg2rad));
    const float s1(sinf(rotDeg[1]*deg2rad));
    const float c1(cosf(rotDeg[1]*deg2rad));
    const float s2(sinf(rotDeg[2]*deg2rad));
    const float c2(cosf(rotDeg[2]*deg2rad));

    // Determine right axis.

    (*right)[0] =  c1*c2;
    (*right)[1] =  s0*s1*c2 + c0*s2;
    (*right)[2] = -c0*s1*c2 + s0*s2;

    // Determine up axis.

    (*up)[0] = -c1*s2;
    (*up)[1] = -s0*s1*s2 + c0*c2;
    (*up)[2] =  c0*s1*s2 + s0*c2;

    // Determine back axis.

    (*back)[0] =  s1;
    (*back)[1] = -s0*c1;
    (*back)[2] =  c0*c1;

//    qDebug() << "Right:" << (*left)[0] << (*left)[1] << (*left)[2];
//    qDebug() << "Up:" << (*up)[0] << (*up)[1] << (*up)[2];
//    qDebug() << "Back:" << (*forward)[0] << (*forward)[1] << (*forward)[2];
}

void
Ns3DOpFrustumItem::_updateFrustum(Ngl::vec3f wsx3[8]) const
{
    Ns3DCameraScope *cam(
        dynamic_cast<Ns3DCameraScope*>(&opObject()->mutableNgOp()));

    if (0 != cam) {
        // HACK: Gigantic hack so that we can setup the frustum!!
        GLint v[4];
        glGetIntegerv(GL_VIEWPORT, v);
        Ngl::Viewport vp(v[0], v[1], v[2], v[3]);

        if (cam->hasImagePlane()) {
            int w(-1);
            int h(-1);

            cam->imagePlaneDimensions(w, h);

            if (w > 0 && h > 0) {
                vp.setWidth(w);
                vp.setHeight(h);
            }
        }

        cam->updateModelview();
        cam->updateProjection(vp);

        em::frustum(cam->modelviewMat(), cam->projectionMat(), wsx3);
    }

#if 0
    const Ng::BodyOp* bodyOp(NiConstBodyOp(fromQStr(opName())));

    if (bodyOp &&
        bodyOp->hasSection("View Frustum") &&
        bodyOp->hasSection("Transform")) {

        // HACK: Get current time!
        const Nb::TimeBundle ctb(Nb::ZeroTimeBundle);

        const NtVec3f translate(
            bodyOp->param3f("Translate")->eval(ctb, 0),
            bodyOp->param3f("Translate")->eval(ctb, 1),
            bodyOp->param3f("Translate")->eval(ctb, 2));

        const NtVec3f rotateDeg(
            bodyOp->param3f("Rotate")->eval(ctb, 0),
            bodyOp->param3f("Rotate")->eval(ctb, 1),
            bodyOp->param3f("Rotate")->eval(ctb, 2));


        static const float deg2rad(M_PI/180.f);
        const float phiRad(deg2rad*(90.f - rotateDeg[0]));
        const float thetaRad(deg2rad*rotateDeg[1]);

        const NtVec3f back(
            sinf(thetaRad)*sinf(phiRad),
            cosf(phiRad),
            cosf(thetaRad)*sinf(phiRad));

        const NtVec3f right(
                em::normalized(em::cross(NtVec3f(0.f, 1.f, 0.f), back)));
        const NtVec3f up(em::cross(back, right));

        const em::glmat44f mv(em::make_cam_view(right, up, back, translate));
//        GLfloat mv[16];
//        Ns3DCameraScope::setModelview(right, up, back, translate, mv);

        const GLfloat nearClip(bodyOp->param1f("Near Clip")->eval(ctb));
        const GLfloat farClip(bodyOp->param1f("Far Clip")->eval(ctb));
        const GLfloat angleOfView(bodyOp->param1f("Angle Of View")->eval(ctb));

        const GLfloat tangent(tanf(0.5f*DEG2RAD(angleOfView)));
        const GLfloat height(nearClip*tangent);
        const GLfloat width(height*Ns3DCameraScope::aspectRatio(vp));

        const em::glmat44f p(
            em::make_persp_projection(-width, width,
                                      -height, height,
                                      nearClip, farClip));

        em::frustum(mv, em::glmat44f(p), wsx3);

        return; // Done!
    }
#endif
}

// -----------------------------------------------------------------------------
