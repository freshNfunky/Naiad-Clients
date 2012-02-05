// -----------------------------------------------------------------------------
//
// Ns3DOpSphereItem.cc
//
// Interface for renderable objects.
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

#include "Ns3DOpSphereItem.h"
#include "Ns3DSelectionManager.h"
#include "NsOpObject.h"
#include "NsPreferences.h"

#include <NglUtils.h>
#include <NglState.h>
#include <em_glmat44_algo.h>
#include <QtOpenGL>
#include <limits>

// -----------------------------------------------------------------------------

const Ngl::vec3f Ns3DOpSphereItem::ctr(0.f, 0.f, 0.f);

const GLfloat Ns3DOpSphereItem::rad(1.f);

//! NB: Assumes center = (0,0,0) and radius = 1.
const Ngl::vec3f Ns3DOpSphereItem::bbox[8] = {
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

Ns3DOpSphereItem::Ns3DOpSphereItem(NsOpObject       *opObject,
                                   const int32_t     selId,
                                   const GLfloat     lineWidth,
                                   const GLfloat     selLineWidth)
    : Ns3DOpItem(opObject, selId)
    , _lineWidth(lineWidth)
    , _selLineWidth(selLineWidth)
{
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
}

// -----------------------------------------------------------------------------

void
Ns3DOpSphereItem::draw(LabelInfo *label) const
{
    QColor color;
    if (selected()) {
        color = NsPreferences::instance()->scopeViewSelectionColor();
        glLineWidth(_selLineWidth);
    }
    else {
        color = NsPreferences::instance()->scopeViewSphereLineColor();
        glLineWidth(_lineWidth);
    }
    glColor4ub(color.red(), color.green(), color.blue(), 255);
    _draw(label);
}

// -----------------------------------------------------------------------------

void
Ns3DOpSphereItem::selectionDraw() const
{
    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(id(), &rgba);
    glColor4ubv(&rgba[0]);
    _draw();
}

// -----------------------------------------------------------------------------

Ns3DBBox
Ns3DOpSphereItem::worldBoundingBox() const
{
    Ngl::vec3f wsxMin(
        (std::numeric_limits<GLfloat>::max)(),
        (std::numeric_limits<GLfloat>::max)(),
        (std::numeric_limits<GLfloat>::max)());
    Ngl::vec3f wsxMax(
        -(std::numeric_limits<GLfloat>::max)(),
        -(std::numeric_limits<GLfloat>::max)(),
        -(std::numeric_limits<GLfloat>::max)());

    const em::glmat44f xf = 
        em::make_transform(
            translate(), 
            Ngl::vec3f(0.f, 0.f, 0.f),  // No rotation!
            scale());

    for (int i = 0; i < 8; ++i) {
        const Ngl::vec3f wsx = xf*bbox[i];
        wsxMin[0] = std::min<GLfloat>(wsxMin[0], wsx[0]);
        wsxMin[1] = std::min<GLfloat>(wsxMin[1], wsx[1]);
        wsxMin[2] = std::min<GLfloat>(wsxMin[2], wsx[2]);
        wsxMax[0] = std::max<GLfloat>(wsxMax[0], wsx[0]);
        wsxMax[1] = std::max<GLfloat>(wsxMax[1], wsx[1]);
        wsxMax[2] = std::max<GLfloat>(wsxMax[2], wsx[2]);
    }

    return Ns3DBBox(
        wsxMin[0], wsxMax[0], 
        wsxMin[1], wsxMax[1], 
        wsxMin[2], wsxMax[2]);
}

// -----------------------------------------------------------------------------

void
Ns3DOpSphereItem::_draw(LabelInfo *label) const
{
    glPushMatrix();
        const em::glmat44f xf(
            em::make_transform(translate(),
                               Ngl::vec3f(0.f, 0.f, 0.f),  // No rotation!
                               scale()));
        glMultMatrixf(&xf[0][0]);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glBegin(GL_TRIANGLE_STRIP);
        for (std::vector<Ngl::vec3f>::size_type v(0); v < _vtx.size(); ++v) {
            glVertex3fv(&_vtx[v][0]);
        }
        glEnd();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); // Default.

        if (0 != label) {
            QPointF pos;
            if (labelPos(&ctr, 1, pos)) {
                label->setPos(pos);
                label->setText(opObject()->longName() + QString(" [Sphere]"));
                label->setColor(
                    NsPreferences::instance()->scopeViewSphereTextColor());
            }
        }
    glPopMatrix();
}

// -----------------------------------------------------------------------------
