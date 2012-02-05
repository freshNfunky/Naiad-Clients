// -----------------------------------------------------------------------------
//
// Ns3DOpImportTransformItem.cc
//
// DOCS
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
// -----------------------------------------------------------------------------

#include "Ns3DOpImportTransformItem.h"
#include "Ns3DSelectionManager.h"
#include "NsOpObject.h"
#include "NsBodyObject.h"
#include "NsPreferences.h"

#include <NglUtils.h>
#include <NglState.h>
#include <NglMaterial.h>
#include <NglLight.h>
#include <em_glmat44_algo.h>
#include <QtOpenGL>
#include <limits>

// -----------------------------------------------------------------------------

const Ngl::vec3f Ns3DOpImportTransformItem::ctr(0.f, 0.f, 0.f);

const GLfloat Ns3DOpImportTransformItem::rad(1.f);

//! NB: Assumes center = (0,0,0) and radius = 1.
const Ngl::vec3f Ns3DOpImportTransformItem::bbox[8] = {
    Ngl::vec3f(-1.f, -1.f, -1.f),
    Ngl::vec3f( 1.f, -1.f, -1.f),
    Ngl::vec3f( 1.f, -1.f,  1.f),
    Ngl::vec3f(-1.f, -1.f,  1.f),
    Ngl::vec3f(-1.f,  1.f, -1.f),
    Ngl::vec3f( 1.f,  1.f, -1.f),
    Ngl::vec3f( 1.f,  1.f,  1.f),
    Ngl::vec3f(-1.f,  1.f,  1.f)
};

// -----------------------------------------------------------------------------

Ns3DOpImportTransformItem::Ns3DOpImportTransformItem(
    NsOpObject       *opObject,
    const int32_t     selId)
    : Ns3DOpItem(opObject, selId)
    , _sphere(1.f, 64)
{
    /*
    const GLint subd(32);

    const GLint subdh(subd/2);
    const GLfloat factor(2.f*M_PI/subd);
    GLint lat;
    GLint lon;
    GLfloat latf;
    GLfloat lonf;
    Ngl::vec3f vtx;
    Ngl::vec3f nml;

    _vtx.reserve(2*subdh*subd);

    for (lat = 0; lat < subdh; ++lat) {
        for (lon = 0; lon <= subd; ++lon) {
            latf = lat*factor;
            lonf = lon*factor;
            GLfloat sinLat = sin(latf);
            nml[0] = cos(lonf)*sinLat;
            nml[1] = sin(lonf)*sinLat;
            nml[2] = cos(latf);
            vtx[0] = ctr[0] + rad*nml[0];
            vtx[1] = ctr[1] + rad*nml[1];
            vtx[2] = ctr[2] + rad*nml[2];
            _vtx.push_back(vtx);
            //_nml.push_back(nml);

            latf = (lat+1)*factor;
            lonf = lon*factor;
            sinLat = sin(latf);
            nml[0] = cos(lonf)*sinLat;
            nml[1] = sin(lonf)*sinLat;
            nml[2] = cos(latf);
            vtx[0] = ctr[0] + rad*nml[0];
            vtx[1] = ctr[1] + rad*nml[1];
            vtx[2] = ctr[2] + rad*nml[2];
            _vtx.push_back(vtx);
            //_nml.push_back(nml);
        }
    }
    */
}

// -----------------------------------------------------------------------------

void
Ns3DOpImportTransformItem::draw(LabelInfo *label) const
{
    Ns3DBBox bb;
    const QList<const NsBodyObject*> bodies =
        opObject()->constEmpBodyCache().constBodies();
    if (_bodyBBox(bodies, bb)) {
        Ngl::FlipState<GL_DEPTH_TEST>      depthTestState;
        Ngl::FlipState<GL_LIGHTING>        lightingState;
        Ngl::FlipState<GL_POLYGON_STIPPLE> polygonStippleState;
        Ngl::FlipState<GL_NORMALIZE>       normalizeState;

        // Normals are scaled to length 1
        Ngl::FlipState<GL_NORMALIZE>::enable();

        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);

        glPushMatrix();
            glLoadIdentity();
            Ngl::Light light(
                Ngl::vec4f(1.f, 1.f, 1.f, 1.f),
                Ngl::vec4f(1.f, 1.f, 1.f, 1.f),
                Ngl::vec4f(1.f, 1.f, 1.f, 1.f));
            //light.position = Ngl::vec4f(0.f, 0.f, 0.f, 1.f);
            light.set();
        glPopMatrix();

        QColor color;
        if (selected()) {
            color = NsPreferences::instance()->scopeViewSelectionColor();
            const Ngl::Material mat(
                Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
                Ngl::vec4f(color.redF(), color.greenF(), color.blueF(), 1.f),
                Ngl::vec4f(0.f, 0.f, -1.f, 0.f));
            mat.set();
            //glLineWidth(_selLineWidth);
        }
        else {
            color = NsPreferences::instance()->scopeViewImportTransformLineColor();
            const Ngl::Material mat(
                Ngl::vec4f(0.2f, 0.2f, 0.2f, 1.f),
                Ngl::vec4f(color.redF(), color.greenF(), color.blueF(), 1.f),
                Ngl::vec4f(0.f, 0.f, 0.f, 1.f));
            mat.set();
            //glLineWidth(_lineWidth);
        }
        //glColor4ub(color.red(), color.green(), color.blue(), 255);

        // Pass 1, draw stippled

        Ngl::FlipState<GL_DEPTH_TEST>::disable();
        Ngl::FlipState<GL_POLYGON_STIPPLE>::enable();
        //Ngl::FlipState<GL_CULL_FACE>::enable();
        //glCullFace(GL_BACK);
        static const GLubyte halftone[] = {
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
        glPolygonStipple(halftone);
        em::glmat44f xf(1.f);
        _makeXForm(bb, xf);
        _draw(xf); // No label.

        // Pass 2, draw normally

        //Ngl::FlipState<GL_CULL_FACE>::disable();
        Ngl::FlipState<GL_POLYGON_STIPPLE>::disable();
        Ngl::FlipState<GL_DEPTH_TEST>::enable();
        _draw(xf, label);

        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHTING);
    }
}

// -----------------------------------------------------------------------------

void
Ns3DOpImportTransformItem::selectionDraw() const
{
    Ns3DBBox bb;
    const QList<const NsBodyObject*> bodies =
        opObject()->constEmpBodyCache().constBodies();
    if (_bodyBBox(bodies, bb)) {
        Ngl::vec4uc rgba;
        Ns3DSelectionManager::idToRgba(id(), &rgba);
        glColor4ubv(&rgba[0]);
        em::glmat44f xf(1.f);
        _makeXForm(bb, xf);
        _draw(xf);
    }
}

// -----------------------------------------------------------------------------

Ns3DBBox
Ns3DOpImportTransformItem::worldBoundingBox() const
{
    Ns3DBBox bb(bbox[0][0], bbox[7][0],
                bbox[0][1], bbox[7][1],
                bbox[0][2], bbox[7][2]); // Object space!
    const QList<const NsBodyObject*> bodies =
        opObject()->constEmpBodyCache().constBodies();
    if (_bodyBBox(bodies, bb)) {
        Ngl::vec3f wsxMin(
            (std::numeric_limits<GLfloat>::max)(),
            (std::numeric_limits<GLfloat>::max)(),
            (std::numeric_limits<GLfloat>::max)());
        Ngl::vec3f wsxMax(
            -(std::numeric_limits<GLfloat>::max)(),
            -(std::numeric_limits<GLfloat>::max)(),
            -(std::numeric_limits<GLfloat>::max)());
        em::glmat44f xf(1.f);
        _makeXForm(bb, xf);
        for (int i = 0; i < 8; ++i) {
            const Ngl::vec3f wsx = xf*bbox[i]; // World space!
            wsxMin[0] = std::min<GLfloat>(wsxMin[0], wsx[0]);
            wsxMin[1] = std::min<GLfloat>(wsxMin[1], wsx[1]);
            wsxMin[2] = std::min<GLfloat>(wsxMin[2], wsx[2]);
            wsxMax[0] = std::max<GLfloat>(wsxMax[0], wsx[0]);
            wsxMax[1] = std::max<GLfloat>(wsxMax[1], wsx[1]);
            wsxMax[2] = std::max<GLfloat>(wsxMax[2], wsx[2]);
        }
        bb = Ns3DBBox(wsxMin[0], wsxMax[0],
                      wsxMin[1], wsxMax[1],
                      wsxMin[2], wsxMax[2]);
    }
    return bb;
}

// -----------------------------------------------------------------------------

//Ngl::vec3f
//Ns3DOpImportTransformItem::translate() const
//{
//    Ns3DBBox bb(bbox[0][0], bbox[7][0],
//                bbox[0][1], bbox[7][1],
//                bbox[0][2], bbox[7][2]); // Object space!
//    const QList<const NsBodyObject*> bodies =
//        opObject()->constEmpBodyCache().constBodies();
//    _bodyBBox(bodies, bb);      // Ignore success.
//    return Ngl::vec3f(bb.midX(), bb.midY(), bb.midZ());
//}

Ngl::vec3f
Ns3DOpImportTransformItem::position() const
{
    Ns3DBBox bb(bbox[0][0], bbox[7][0],
                bbox[0][1], bbox[7][1],
                bbox[0][2], bbox[7][2]); // Object space!
    const QList<const NsBodyObject*> bodies =
        opObject()->constEmpBodyCache().constBodies();
    _bodyBBox(bodies, bb);      // Ignore success.
    return Ngl::vec3f(bb.midX(), bb.midY(), bb.midZ());
}

// -----------------------------------------------------------------------------

void
Ns3DOpImportTransformItem::_draw(const em::glmat44f &xf,
                                 LabelInfo          *label) const
{
    glPushMatrix();
        glMultMatrixf(&xf[0][0]);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//        glBegin(GL_TRIANGLE_STRIP);
//        for (std::vector<Ngl::vec3f>::size_type v(0); v < _vtx.size(); ++v) {
//            glVertex3fv(&_vtx[v][0]);
//        }
//        glEnd();
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default.

        _sphere.draw();

        if (0 != label) {
            // Move item label up a little bit so it doesn't collide with
            // body label.

            Ngl::vec3f labelCtr = ctr + 0.5f*rad*Ngl::vec3f(0.f, 1.f, 0.f);
            QPointF pos;
            if (labelPos(&labelCtr, 1, pos)) {
                label->setPos(pos);
                label->setText(
                    opObject()->longName() + QString(" [Import Transform]"));
                label->setColor(
                    NsPreferences::instance()->scopeViewImportTransformTextColor());
            }
        }
    glPopMatrix();
}

// -----------------------------------------------------------------------------

// _bodyCentroid
// -------------
//! Returns false if no valid bounds can be computed. [static]

bool
Ns3DOpImportTransformItem::_bodyBBox(
    const QList<const NsBodyObject*> &bodies,
    Ns3DBBox                         &bb)
{
    bool valid = false;
    if (!bodies.isEmpty()) {
        NtVec3f bodiesMin(
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max(),
            std::numeric_limits<float>::max());
        NtVec3f bodiesMax(
            -std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max(),
            -std::numeric_limits<float>::max());

        foreach (const NsBodyObject *body, bodies) {
            NtVec3f bodyMin(
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max(),
                std::numeric_limits<float>::max());
            NtVec3f bodyMax(
                -std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max(),
                -std::numeric_limits<float>::max());
            body->nbBody().geometryBounds(bodyMin, bodyMax);
            if (bodyMin[0] < bodyMax[0] &&
                bodyMin[1] < bodyMax[1] &&
                bodyMin[2] < bodyMax[2] ) {
                // Valid bounds.

                bodiesMin[0] = std::min<float>(bodiesMin[0], bodyMin[0]);
                bodiesMin[1] = std::min<float>(bodiesMin[1], bodyMin[1]);
                bodiesMin[2] = std::min<float>(bodiesMin[2], bodyMin[2]);
                bodiesMax[0] = std::max<float>(bodiesMax[0], bodyMax[0]);
                bodiesMax[1] = std::max<float>(bodiesMax[1], bodyMax[1]);
                bodiesMax[2] = std::max<float>(bodiesMax[2], bodyMax[2]);
            }
        }

        if (bodiesMin[0] < bodiesMax[0] &&
            bodiesMin[1] < bodiesMax[1] &&
            bodiesMin[2] < bodiesMax[2] ) {
            // Valid bounds.

            bb = Ns3DBBox(bodiesMin[0], bodiesMax[0],
                          bodiesMin[1], bodiesMax[1],
                          bodiesMin[2], bodiesMax[2]);
            valid = true;
        }
    }

    return valid;
}


// _makeXForm
// ----------
//! No rotation! [static]

void
Ns3DOpImportTransformItem::_makeXForm(const Ns3DBBox &bb, Ngl::mat44f &xf)
{
    GLfloat glmv[16];
    GLfloat glp[16];
    GLint glvp[4];
    glGetFloatv(GL_MODELVIEW_MATRIX, glmv);
    glGetFloatv(GL_PROJECTION_MATRIX, glp);
    glGetIntegerv(GL_VIEWPORT, glvp);
    const em::glmat44<GLfloat> mv(glmv);
    const em::glmat44<GLfloat> p(glp);
    const em::vec<4,int> vp(glvp[0], glvp[1], glvp[2], glvp[3]);
    em::vec<4,GLfloat> eye0;
    em::vec<4,GLfloat> clip0;
    em::vec<3,GLfloat> ndc0;
    em::vec<3,GLfloat> win0;
    const Ngl::vec3f t(bb.midX(), bb.midY(), bb.midZ());
    em::project(t, vp, mv, p, eye0, clip0, ndc0, win0);

    Ngl::vec3f s(1.f);
    const float invP = 1.f/std::min<float>(p[0][0], p[1][1]);
    if (-1 == p[2][3]) {
        // Perspective projection.

        const float sv = 0.3f*0.15f*0.5f*invP*fabs(eye0[2]);
        s[0] = sv;
        s[1] = sv;
        s[2] = sv;
    }
    else {
        // Ortho projection.

        const float sv = 0.3f*0.15f*0.5f*invP;
        s[0] = sv;
        s[1] = sv;
        s[2] = sv;
    }

    xf = em::make_transform(t, Ngl::vec3f(0.f), s); // No rotation!

#if 0
    std::cerr << em::glmat44<GLfloat>(glmv) << "\n";
    std::cerr << "\n";
    std::cerr << em::glmat44<GLfloat>(glp) << "\n";
#endif
}

// -----------------------------------------------------------------------------
