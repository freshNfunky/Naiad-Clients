// -----------------------------------------------------------------------------
//
// Ns3DOpPlaneItem.cc
//
// Ops which have a plane shape.
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

#include "Ns3DOpPlaneItem.h"
#include "Ns3DSelectionManager.h"
#include "NsOpObject.h"
#include "NsPreferences.h"

#include <NglUtils.h>
#include <NglState.h>
#include <em_glmat44_algo.h>
#include <QtOpenGL>
#include <limits>

// -----------------------------------------------------------------------------

const Ngl::vec3f Ns3DOpPlaneItem::vtx[4] = {
    Ngl::vec3f(-0.5f, 0.f, -0.5f),
    Ngl::vec3f( 0.5f, 0.f, -0.5f),
    Ngl::vec3f( 0.5f, 0.f,  0.5f),
    Ngl::vec3f(-0.5f, 0.f,  0.5f)
};

const Ngl::vec2i Ns3DOpPlaneItem::edge[4] = {
    Ngl::vec2i(0,1), Ngl::vec2i(1,2), Ngl::vec2i(2,3), Ngl::vec2i(3,0)
};

const Ngl::vec3i Ns3DOpPlaneItem::face[2] = {    
    Ngl::vec3i(3,0,1), Ngl::vec3i(1,2,3)
};

const Ngl::vec3f Ns3DOpPlaneItem::normal[2] = {    
    Ngl::vec3f( 0.f,  1.f, 0.f),   // Y+
    Ngl::vec3f( 0.f, -1.f, 0.f)    // Y-
};

const GLfloat Ns3DOpPlaneItem::height(0.5f);

//! NB: Assumes height = 0.5.
const Ngl::vec3f Ns3DOpPlaneItem::bbox[8] = {
    Ngl::vec3f(-0.5f, 0.f, -0.5f),
    Ngl::vec3f( 0.5f, 0.f, -0.5f),
    Ngl::vec3f( 0.5f, 0.f,  0.5f),
    Ngl::vec3f(-0.5f, 0.f,  0.5f),
    Ngl::vec3f(-0.5f, 0.5f, -0.5f),
    Ngl::vec3f( 0.5f, 0.5f, -0.5f),
    Ngl::vec3f( 0.5f, 0.5f,  0.5f),
    Ngl::vec3f(-0.5f, 0.5f,  0.5f)
};


// -----------------------------------------------------------------------------

void
Ns3DOpPlaneItem::draw(LabelInfo *label) const
{
    //Ngl::PolygonModeState polygonModeState;

    glPushMatrix();

        const em::glmat44f xf(
            em::make_transform(translate(),
                               rotate(),
                               scale()));

        glMultMatrixf(&xf[0][0]);

        QColor color;
        if (selected()) {
            color = NsPreferences::instance()->scopeViewSelectionColor();
            glLineWidth(_selLineWidth);
        }
        else {
            color = NsPreferences::instance()->scopeViewPlaneLineColor();
            glLineWidth(_lineWidth);
        }
        glColor4ub(color.red(), color.green(), color.blue(), 255);
        Ns3DOpPlaneItem::drawEdge();


        if (0 != label) {
            QPointF pos;
            if (labelPos(Ns3DOpPlaneItem::vtx, 4, pos)) {
                label->setPos(pos);
                label->setText(opObject()->longName() + QString(" [Plane]"));
                label->setColor(
                    NsPreferences::instance()->scopeViewPlaneTextColor());
            }
        }

    glPopMatrix();
}

// -----------------------------------------------------------------------------

void
Ns3DOpPlaneItem::selectionDraw() const
{
    Ngl::vec4uc rgba;
    Ns3DSelectionManager::idToRgba(id(), &rgba);

    glPushMatrix();
    const em::glmat44f xf(
        em::make_transform(translate(),
                           rotate(),
                           scale()));

    glMultMatrixf(&xf[0][0]);
    glColor4ubv(&rgba[0]);

    //Ns3DOpPlaneItem::drawFace();
    Ns3DOpPlaneItem::drawEdge();

    glPopMatrix();
}

// -----------------------------------------------------------------------------

Ns3DBBox
Ns3DOpPlaneItem::worldBoundingBox() const
{
    Ngl::vec3f wsxMin(
        (std::numeric_limits<GLfloat>::max)(),
        (std::numeric_limits<GLfloat>::max)(),
        (std::numeric_limits<GLfloat>::max)());
    Ngl::vec3f wsxMax(
        -(std::numeric_limits<GLfloat>::max)(),
        -(std::numeric_limits<GLfloat>::max)(),
        -(std::numeric_limits<GLfloat>::max)());

    const em::glmat44f xf = em::make_transform(translate(), rotate(), scale());

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
Ns3DOpPlaneItem::drawFace()
{
    glBegin(GL_TRIANGLES);
    for (int f = 0; f < 2; ++f) {
        // Bit-shift divide-by-two
        glNormal3fv(&Ns3DOpPlaneItem::normal[f >> 1][0]);
        for (int v = 0; v < 3; ++v) {
            glVertex3fv(&Ns3DOpPlaneItem::vtx[Ns3DOpPlaneItem::face[f][v]][0]);
        }
    }
    glEnd();
}

// -----------------------------------------------------------------------------

void
Ns3DOpPlaneItem::drawEdge()
{
    glBegin(GL_LINES);
    for (int e = 0; e < 4; ++e) {
        for (int v = 0; v < 2; ++v) {
            glVertex3fv(&Ns3DOpPlaneItem::vtx[Ns3DOpPlaneItem::edge[e][v]][0]);
        }
    }
    glEnd();

    //static const GLfloat h2(0.5f);
    static const GLfloat h1 = 0.5f*height;//(0.25f);
    static const GLfloat h0 = 0.5f*height;//(0.25f);
    static const GLfloat w2(0.1);
    static const GLfloat w1(0.033);
    static const GLfloat w0(0.033);

    glBegin(GL_LINE_STRIP);
    glVertex3f( 0.5f, 0.f,    0.f);
    glVertex3f( w0,   0.f,    0.f);
    glVertex3f( w1,   h0,     0.f);
    glVertex3f( w2,   h1,     0.f);
    glVertex3f( 0.f,  height, 0.f);
    glVertex3f(-w2,   h1,     0.f);
    glVertex3f(-w1,   h0,     0.f);
    glVertex3f(-w0,   0.f,    0.f);
    glVertex3f(-0.5f, 0.f,    0.f);
    glEnd();
}

// -----------------------------------------------------------------------------
